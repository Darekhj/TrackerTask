#include "givetask.h"
#include "ui_givetask.h"
#include "filterdialog.h"
#include "mytasks.h"
#include "othertasks.h"

#include<QMessageBox>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QDateEdit>
#include <QComboBox>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>


class FilterDialogEmployee : public QDialog {
public:
    FilterDialogEmployee(QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("Фильтры");

        // Создаем виджеты для ввода данных
        QLabel *titleLabel = new QLabel("Имя:", this);
        titleEdit = new QLineEdit(this);

        QLabel *positionLabel = new QLabel("Должность:", this);
        positionEdit = new QLineEdit(this);


        // Кнопки "Применить" и "Отмена"
        QPushButton *applyButton = new QPushButton("Применить", this);
        QPushButton *cancelButton = new QPushButton("Отмена", this);

        // Подключаем сигналы от кнопок к слотам
        connect(applyButton, &QPushButton::clicked, this, &FilterDialogEmployee::accept);
        connect(cancelButton, &QPushButton::clicked, this, &FilterDialogEmployee::reject);

        // Размещаем виджеты на диалоге
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->addWidget(titleLabel);
        layout->addWidget(titleEdit);

        layout->addWidget(positionLabel);
        layout->addWidget(positionEdit);


        // Размещаем кнопки в горизонтальном слое
        QHBoxLayout *buttonLayout = new QHBoxLayout(this);
        buttonLayout->addWidget(applyButton);
        buttonLayout->addWidget(cancelButton);
        layout->addLayout(buttonLayout);

        setLayout(layout);
    }

    // Гетты для получения значений полей
    QString getTitle() const {
        return titleEdit->text();
    }

    QString getPosition() const {
        return positionEdit->text();
    }

private:
    QLineEdit *titleEdit;
    QLineEdit *positionEdit;
};


GiveTask::GiveTask(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GiveTask)
    , tcpSocket(new QTcpSocket(this))
{


    ui->setupUi(this);
}

GiveTask::GiveTask(QString loggin,QString passWord,bool managerTask,bool adminTask,QWidget *parent) :
    QWidget(parent),

    ui(new Ui::GiveTask),
    tcpSocket(new QTcpSocket(this))
{
    this->loggin=loggin;
    this->passWord=passWord;
    this->adminTask=adminTask;
    this->managerTask=managerTask;

    ui->setupUi(this);
}


QVector<QStringList> GiveTask::getMySubordinates(QString s){
    tcpSocket->abort();
    tcpSocket->connectToHost("127.0.0.1",55555);//подключаемся к серверу

    //передаем

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    QByteArray reqCli=QByteArray()+"{\"name\":\""+loggin.toUtf8()+"\",\"pass\":\""+passWord.toUtf8()+"\",\"operation\":\"getsubordinates\""+s.toUtf8()+"}";
    out.setVersion(QDataStream::Qt_6_5);
    out << reqCli;
    tcpSocket->write(block);
    //получаем ответ
    QDataStream in;
    in.setDevice(tcpSocket);
    in.setVersion(QDataStream::Qt_6_5);
    QByteArray resultRequestServer;
    do{
        if(!tcpSocket->waitForReadyRead(500)){

            QMessageBox::warning(this, "Timeout", "Время ожидания подключения истекло. Пожалуйста, попробуйте повторно подключиться.");
            return {};
        };
        in.startTransaction();
        in >> resultRequestServer;

    }while(!in.commitTransaction());

    tcpSocket->disconnectFromHost();
    QJsonParseError reqClientJsonEr;
    QJsonDocument reqClientJson=QJsonDocument::fromJson(resultRequestServer.data(),&reqClientJsonEr);
    if (reqClientJsonEr.error != QJsonParseError::NoError) {
        QMessageBox::warning(this, "Ошибка считывания", "Пожалуйста, попробуйте повторно.");
        return {};
    }
    QStringList answerServer=reqClientJson.object().keys();
    QVector<QStringList>res(answerServer.size()-1);
    qint32  count =0;
    for(auto &x:answerServer){
        if(x=="answer")continue;
        QJsonArray temp =reqClientJson.object().value(x).toArray();

        for(int i=0;i<temp.size();i++){
            res[count]<<temp[i].toString();
        }

        count++;
    }
    return res;
}


void GiveTask::viewySubordinates(){
    model = new QStandardItemModel(this);

    model->setColumnCount(4);  // Два столбца: Данные, Действие

    // Задаем заголовки столбцов
    model->setHorizontalHeaderLabels(QStringList() <<"Имя"<< "Должность"<<"Отдел"<< "Посмотреть задачи");

    // Создаем вид для таблицы данных
    tableView = new QTableView(this);
    tableView->setModel(model);

    // Устанавливаем ширину столбцов (можно настроить по своему усмотрению)
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    QVector<QStringList> tableTask=getMySubordinates();
    for(auto& x:tableTask){

         insertRow(x);
    }
    // Добавляем данные в модель
    QPushButton *refreshButton = new QPushButton("Обновить", this);
    QPushButton *filtersButton = new QPushButton("Фильтры", this);


    connect(refreshButton, &QPushButton::clicked, this, &GiveTask::onRefreshButtonClicked);
    connect(filtersButton, &QPushButton::clicked, this, &GiveTask::onFiltersButtonClicked);

    // Создаем верхний виджет для отображения логина и кнопки выхода
    QString RealName=getRealName();
    QWidget *topWidget = new QWidget(this);
    QHBoxLayout *topLayout = new QHBoxLayout(topWidget);
    // Добавляем текст логина и кнопку добавиь задачу в верхний виджет
    loginLabel = new QLabel("ФИО: "+RealName, this);
    loginLabel->setStyleSheet("font-weight: bold;");  // Настройка стиля, по желанию
    QPushButton *logoutButton = new QPushButton("Выход", this);
    connect(logoutButton, &QPushButton::clicked, this, &GiveTask::onLogoutButtonClicked);

    topLayout->addWidget(loginLabel);
    topLayout->addStretch();  // Пространство для выравнивания вправо
    topLayout->addWidget(logoutButton);

    // Создаем горизонтальный слой для размещения кнопок "Мои задачи", "Назначение задач" и "Администрирование"


    QHBoxLayout *buttonRowLayout = new QHBoxLayout;

    // Создаем кнопки "Мои задачи", "Назначение задач" и "Администрирование"
    QPushButton *myTasksButton = new QPushButton("Мои задачи", this);
    buttonRowLayout->addWidget(myTasksButton);
    connect(myTasksButton, &QPushButton::clicked, this, &GiveTask::manageTask);
    QPushButton *assignTasksButton=nullptr;

    if(managerTask){
         assignTasksButton = new QPushButton("Назначение задач", this);
         buttonRowLayout->addWidget(assignTasksButton);
        connect(assignTasksButton, &QPushButton::clicked, this, &GiveTask::viewTasks);
    }
    QPushButton *administrationButton=nullptr;
    if(adminTask){
        administrationButton = new QPushButton("Администрирование", this);
        // connect(administrationButton, &QPushButton::clicked, this, &MyWidget::onAdministrationButtonClicked);
        buttonRowLayout->addWidget(administrationButton);
    }




    // Создаем вертикальный слой для размещения верхнего виджета, кнопок и таблицы
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(topWidget);
    layout->addLayout(buttonRowLayout);  // Добавляем горизонтальный слой с кнопками
    layout->addWidget(tableView);

    // Добавляем кнопки внизу виджета
    QHBoxLayout *buttonLayout = new QHBoxLayout(this);
    buttonLayout->addWidget(refreshButton);
    buttonLayout->addWidget(filtersButton);
    layout->addLayout(buttonLayout);


    QString logoutButtonStyle = "QPushButton {"
                                "   background-color: #f44336;"  // Красный цвет
                                "   border: none;"
                                "   color: white;"
                                "   padding: 10px 20px;"
                                "   text-align: center;"
                                "   text-decoration: none;"
                                "   font-size: 16px;"
                                "   margin: 4px 2px;"
                                "border-radius: 10px;"
                                "}"
                                "QPushButton:hover {"
                                "   background-color: #d32f2f;"  // Темно-красный цвет при наведении
                                "}";

    logoutButton->setStyleSheet(logoutButtonStyle);
    QString topWidgetStyle = "QWidget {"

                             "   border-bottom: 2px solid #ccc;" // Линия под верхним виджетом
                             "}";

    // Применяем стиль к верхнему виджету
    topWidget->setStyleSheet(topWidgetStyle);

    // Стиль для лейбла с логином
    QString loginLabelStyle = "QLabel {"
                              "   font-weight: bold;"
                              "   font-size: 18px;"
                              "   color: #333;" // Темно-серый цвет
                              "}";

    // Применяем стиль к лейблу с логином
    loginLabel->setStyleSheet(loginLabelStyle);
    QString buttonStyle = "QPushButton {"
                          "   background-color: #4CAF50;"
                          "   border: none;"
                          "   color: white;"
                          "   padding: 10px 20px;"
                          "   text-align: center;"
                          "   text-decoration: none;"

                          "   font-size: 16px;"
                          "   margin: 4px 2px;"

                          "border-radius: 10px;"
                          "}"
                          "QPushButton:hover {"
                          "   background-color: #008000;"
                          "}";

    // Применяем стиль к кнопкам
    if(refreshButton)refreshButton->setStyleSheet(buttonStyle);
    if(myTasksButton)myTasksButton->setStyleSheet(buttonStyle);
    if(assignTasksButton)assignTasksButton->setStyleSheet(buttonStyle);
    if(administrationButton )administrationButton->setStyleSheet(buttonStyle);
    if(filtersButton)filtersButton->setStyleSheet(buttonStyle);
    QString tableStyle = "QTableView {"
                         "   background-color: #ecf0f1;"
                         "   border: 1px solid #bdc3c7;"
                         "   gridline-color: #bdc3c7;"
                         "   selection-background-color: #2980b9;"
                         "   selection-color: white;"
                         "}"
                         "QTableView::item {"
                         "   padding: 5px;"
                         "}";

    // Применяем стиль к таблице
    tableView->setStyleSheet(tableStyle);
    setLayout(layout);
}







void GiveTask::insertRow(const QStringList &data) {
    QList<QStandardItem *> items;
    QString idName=data.first();
    bool b=1;
    qint32 n=0;
    for (const QString &text : data) {
        if(b){
            b=0;
            continue;
        }
        QStandardItem *item = new QStandardItem(text);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        items.append(item);
        n++;
    }



    // Устанавливаем данные в модель

     model->appendRow(items);
    // Устанавливаем кнопку в ячейку таблицы
    // Создаем кнопку для действия
    QPushButton *rowButton = new QPushButton("Посмотреть", this);
    QString buttonStyle = "QPushButton {"
                       "   background-color: #4CAF50;"
                       "   color: white;"
                       "   text-align: center;"
                       "   text-decoration: none;"
                       "border-radius: 10px;"
                       "}"
                       "QPushButton:hover {"
                       "   background-color: #008000;"
                       "}";
     rowButton->setStyleSheet(buttonStyle);
    connect(rowButton, &QPushButton::clicked, [=]() {
        onRowButtonClicked(idName);
    });
    tableView->setIndexWidget(model->index(model->rowCount() - 1, 3), rowButton);

}

void GiveTask::onRowButtonClicked(const QString &data) {


    OtherTasks* OtherTask =new OtherTasks(loggin,passWord,data);
    OtherTask->setWindowTitle("Просмотр задач");
    OtherTask->setVisible(true);

}




QString GiveTask::getRealName() {
    tcpSocket->abort();
    tcpSocket->connectToHost("127.0.0.1",55555);//подключаемся к серверу
    //передаем

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    QByteArray reqCli=QByteArray()+"{\"name\":\""+loggin.toUtf8()+"\",\"pass\":\""+passWord.toUtf8()+"\",\"operation\":\"getRealName\",\"employee\":\""+loggin.toUtf8()+"\"}";

    out.setVersion(QDataStream::Qt_6_5);
    out << reqCli;
    tcpSocket->write(block);
    //получаем ответ
    QDataStream in;
    in.setDevice(tcpSocket);
    in.setVersion(QDataStream::Qt_6_5);
    QByteArray resultRequestServer;
    do{
        if(!tcpSocket->waitForReadyRead(500)){

            tcpSocket->disconnectFromHost();
            QMessageBox::warning(this, "Timeout", "Время ожидания подключения истекло. Пожалуйста, попробуйте повторно подключиться.");
            return "none";
        };
        in.startTransaction();
        in >> resultRequestServer;

    }while(!in.commitTransaction());

    tcpSocket->disconnectFromHost();
    QJsonParseError reqClientJsonEr;
    QJsonDocument reqClientJson=QJsonDocument::fromJson(resultRequestServer.data(),&reqClientJsonEr);
    if (reqClientJsonEr.error != QJsonParseError::NoError) {
        QMessageBox::warning(this, "Ошибка считывания", "Пожалуйста, попробуйте повторно.");
        return "none";
    }
    return reqClientJson.object().value("realName").toString();


}


void GiveTask::onRefreshButtonClicked() {
    QVector<QStringList> tableTask=getMySubordinates();
    model->clear();
    model->setColumnCount(4);
    model->setHorizontalHeaderLabels(QStringList() <<"Имя"<< "Должность"<<"Отдел"<< "Посмотреть задачи");



    for(auto& x:tableTask){

        insertRow(x);
     }

}

void GiveTask::onLogoutButtonClicked() {

    emit exitSystem();
}


void GiveTask::onFiltersButtonClicked() {
    // Создаем диалог и открываем его модально
    FilterDialogEmployee filterDialog(this);
    if (filterDialog.exec() == QDialog::Accepted) {
        QString temp=",\"filter\":\"on\",\"realName\":\""+filterDialog.getTitle()+"\",\"position\":\""+filterDialog.getPosition()+"\"";
        QVector<QStringList> tableTask=getMySubordinates(temp);
        // Получаем значения из диалога

        model->clear();
        model->setColumnCount(4);
        model->setHorizontalHeaderLabels(QStringList() <<"ФИО выполняющего задачу"<< "Должность"<<"Отдел"<< "Посмотреть задачи");

        for(auto& x:tableTask){

            insertRow(x);
        }





    }
}
//









GiveTask::~GiveTask()
{
    delete ui;
}
