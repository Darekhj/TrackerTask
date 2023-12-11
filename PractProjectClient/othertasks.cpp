#include "othertasks.h"
#include "ui_othertasks.h"
#include "filterdialog.h"

#include<QMessageBox>
#include <QDateEdit>
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


class GiveTaskDialog : public QDialog {
public:
    GiveTaskDialog(QWidget *parent = nullptr) : QDialog(parent) {
        this->setWindowTitle("Добавить задачу");
        // Создаем виджеты для ввода данных
        QLabel *titleLabel = new QLabel("Название:", this);
        titleEdit = new QLineEdit(this);
        QLabel *dateLabel = new QLabel("Дата окончания:", this);
        dateEdit = new QDateEdit(this);
        dateEdit->setDisplayFormat("yyyy-MM-dd");


        // Кнопки "Применить" и "Отмена"
        QPushButton *applyButton = new QPushButton("Применить", this);
        QPushButton *cancelButton = new QPushButton("Отмена", this);

        // Подключаем сигналы от кнопок к слотам
        connect(applyButton, &QPushButton::clicked, this, &GiveTaskDialog::accept);
        connect(cancelButton, &QPushButton::clicked, this, &GiveTaskDialog::reject);

        // Размещаем виджеты на диалоге
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->addWidget(titleLabel);
        layout->addWidget(titleEdit);
        layout->addWidget(dateLabel);
        layout->addWidget(dateEdit);

        // Размещаем кнопки в горизонтальном слое
        QHBoxLayout *buttonLayout = new QHBoxLayout(this);
        buttonLayout->addWidget(applyButton);
        buttonLayout->addWidget(cancelButton);
        layout->addLayout(buttonLayout);

        setLayout(layout);
    }

    // Геттеры для получения значений полей
    QString getTitle() const {
        return titleEdit->text();
    }
    QString getDate() const {
        return dateEdit->text();
    }



private:
    QLineEdit *titleEdit;
    QDateEdit *dateEdit;
};


OtherTasks::OtherTasks(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OtherTasks)
{
    ui->setupUi(this);
}

OtherTasks::OtherTasks(QString loggin,QString passWord,QString nameId,QWidget *parent) :
    QWidget(parent),

    ui(new Ui::OtherTasks),
    tcpSocket(new QTcpSocket(this))

{
    this->nameID=nameId;
    this->loggin=loggin;
    this->passWord=passWord;
    model = new QStandardItemModel(this);

    model->setColumnCount(6);  // Два столбца: Данные, Действие

    // Задаем заголовки столбцов
    model->setHorizontalHeaderLabels(QStringList() <<"Имя задачи"<< "Статус задачи"<<"ФИО автора задачи"<< "Дата выдачи"<< "Дата окончания"<<"выполнить задачу?");

    // Создаем вид для таблицы данных
    tableView = new QTableView(this);
    tableView->setModel(model);

    // Устанавливаем ширину столбцов (можно настроить по своему усмотрению)
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    QVector<QStringList> tableTask=getTask();
    for(auto& x:tableTask){
         insertRow(x);
    }
    // Добавляем данные в модель
    QPushButton *refreshButton = new QPushButton("Обновить", this);
    QPushButton *filtersButton = new QPushButton("Фильтры", this);


    connect(refreshButton, &QPushButton::clicked, this, &OtherTasks::onRefreshButtonClicked);
    connect(filtersButton, &QPushButton::clicked, this, &OtherTasks::onFiltersButtonClicked);




    // Создаем верхний виджет для отображения логина и кнопки добавить задачу
    QString RealName=getRealName();
    QWidget *topWidget = new QWidget(this);
    QHBoxLayout *topLayout = new QHBoxLayout(topWidget);
    // Добавляем текст логина и кнопку добавиь задачу в верхний виджет
    loginLabel = new QLabel("ФИО: "+RealName, this);
    loginLabel->setStyleSheet("font-weight: bold;");  // Настройка стиля, по желанию
    QPushButton *giveTaskButton = new QPushButton("Дать задачу", this);
    connect(giveTaskButton, &QPushButton::clicked, this, &OtherTasks::onTaskGiveButtonClicked);

    topLayout->addWidget(loginLabel);
    topLayout->addStretch();  // Пространство для выравнивания вправо
    topLayout->addWidget(giveTaskButton);



    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(topWidget);
    layout->addWidget(tableView);

    // Добавляем кнопки внизу виджета
    QHBoxLayout *buttonLayout = new QHBoxLayout(this);
    buttonLayout->addWidget(refreshButton);
    buttonLayout->addWidget(filtersButton);
    layout->addLayout(buttonLayout);

    setLayout(layout);
    QString topWidgetStyle = "QWidget {"

                             "   border-bottom: 2px solid #ccc;"
                             "}";

    // Применяем стиль к верхнему виджету
    topWidget->setStyleSheet(topWidgetStyle);

    // Стиль для лейбла с логином
    QString loginLabelStyle = "QLabel {"
                              "   font-weight: bold;"
                              "   font-size: 18px;"
                              "   color: #333;"
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
    if(giveTaskButton)giveTaskButton->setStyleSheet(buttonStyle);
    if(refreshButton)refreshButton->setStyleSheet(buttonStyle);
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



    ui->setupUi(this);
}


void OtherTasks::onTaskGiveButtonClicked() {


    GiveTaskDialog TaskDialog(this);
    if (TaskDialog.exec() == QDialog::Accepted) {
         // Получаем значения из диалога
         tcpSocket->abort();
         tcpSocket->connectToHost("127.0.0.1",55555);//подключаемся к серверу

         //передаем
         QByteArray block;
         QDataStream out(&block, QIODevice::WriteOnly);
         QByteArray reqCli=QByteArray()+"{\"name\":\""+loggin.toUtf8()+"\",\"pass\":\""+passWord.toUtf8()+"\",\"operation\":\"GiveTask\",\"employee\":\""+nameID.toUtf8()+"\",\"nameTask\":\""+TaskDialog.getTitle().toUtf8()+"\",\"dateEnd\":\""+TaskDialog.getDate().toUtf8()+"\"}";

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
                 return;
             };
             in.startTransaction();
             in >> resultRequestServer;

         }while(!in.commitTransaction());

         tcpSocket->disconnectFromHost();
         QJsonParseError reqClientJsonEr;
         QJsonDocument reqClientJson=QJsonDocument::fromJson(resultRequestServer.data(),&reqClientJsonEr);
         if (reqClientJsonEr.error != QJsonParseError::NoError) {
             QMessageBox::warning(this, "Ошибка считывания", "Пожалуйста, попробуйте повторно.");
             return;
         }


        onRefreshButtonClicked();


    }
}


QString OtherTasks::getRealName() {


    tcpSocket->abort();
    tcpSocket->connectToHost("127.0.0.1",55555);//подключаемся к серверу

    //передаем
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    QByteArray reqCli=QByteArray()+"{\"name\":\""+loggin.toUtf8()+"\",\"pass\":\""+passWord.toUtf8()+"\",\"operation\":\"getRealName\",\"employee\":\""+nameID.toUtf8()+"\"}";

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


QVector<QStringList> OtherTasks::getTask(QString s){
    tcpSocket->abort();
    tcpSocket->connectToHost("127.0.0.1",55555);//подключаемся к серверу

    //передаем
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    QByteArray reqCli=QByteArray()+"{\"name\":\""+loggin.toUtf8()+"\",\"pass\":\""+passWord.toUtf8()+"\",\"operation\":\"getOtherTask\""+s.toUtf8()+",\"employee\":\""+nameID.toUtf8()+"\"}";
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
             return {};
         };
        in.startTransaction();
        in >> resultRequestServer;

    }while(!in.commitTransaction());

    tcpSocket->disconnectFromHost();
    QJsonParseError reqClientJsonEr;
    QJsonDocument reqClientJson=QJsonDocument::fromJson(resultRequestServer.data(),&reqClientJsonEr);
    QStringList answerServer=reqClientJson.object().keys();
    if (reqClientJsonEr.error != QJsonParseError::NoError) {
        QMessageBox::warning(this, "Ошибка считывания", "Пожалуйста, попробуйте повторно.");
        return {};
    }
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




void OtherTasks::insertRow(const QStringList &data) {
    QList<QStandardItem *> items;
    QString idTask=data.first();
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
    QPushButton *rowButton = new QPushButton("Завершить", this);
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
        onRowButtonClicked(idTask);
    });
     tableView->setIndexWidget(model->index(model->rowCount() - 1, 5), rowButton);

}

void OtherTasks::onRowButtonClicked(const QString &data) {


    tcpSocket->abort();
    tcpSocket->connectToHost("127.0.0.1",55555);//подключаемся к серверу

    //передаем
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    QByteArray reqCli=QByteArray()+"{\"name\":\""+loggin.toUtf8()+"\",\"pass\":\""+passWord.toUtf8()+"\",\"operation\":\"deleteTask\",\"id\":\""+data.toUtf8()+"\",\"employee\":\""+nameID.toUtf8()+"\"}";

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
            return;
        };
        in.startTransaction();
        in >> resultRequestServer;

    }while(!in.commitTransaction());

    tcpSocket->disconnectFromHost();
    QJsonParseError reqClientJsonEr;
    QJsonDocument reqClientJson=QJsonDocument::fromJson(resultRequestServer.data(),&reqClientJsonEr);
    if (reqClientJsonEr.error != QJsonParseError::NoError) {
        QMessageBox::warning(this, "Ошибка считывания", "Пожалуйста, попробуйте повторно.");

    }
    onRefreshButtonClicked();

}



void OtherTasks::onRefreshButtonClicked() {
    QVector<QStringList> tableTask=getTask();
    model->clear();
    model->setColumnCount(6);
    model->setHorizontalHeaderLabels(QStringList() <<"Имя задачи"<< "Статус задачи"<<"ФИО автора задачи"<< "Дата выдачи"<< "Дата окончания"<<"выполнить задачу?");


    for(auto& x:tableTask){

        insertRow(x);
     }
}




void OtherTasks::onFiltersButtonClicked() {
    // Создаем диалог и открываем его модально
    FilterDialog filterDialog(this);
    if (filterDialog.exec() == QDialog::Accepted) {
        // Получаем значения из диалога
        QString temp=",\"filter\":\"on\",\"title\":\""+filterDialog.getTitle()+"\",\"status\":\""+filterDialog.getStatus()+"\"";
        QVector<QStringList> tableTask=getTask(temp);
        model->clear();
        model->setColumnCount(5);
        model->setHorizontalHeaderLabels(QStringList() <<"Имя задачи"<< "Статус задачи"<<"ФИО автора задачи"<< "Дата выдачи"<< "Дата окончания"<<"выполнить задачу?");
        for(auto& x:tableTask){

            insertRow(x);
        }





    }
}


OtherTasks::~OtherTasks()
{
    delete ui;
}
