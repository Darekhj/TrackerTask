#include "mytasks.h"
#include "ui_mytasks.h"
#include "filterdialog.h"

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


MyTasks::MyTasks(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyTasks)
    , tcpSocket(new QTcpSocket(this))
{



    ui->setupUi(this);
}

MyTasks::MyTasks(QString loggin,QString passWord,bool managerTask,bool adminTask,QWidget *parent) :
    QWidget(parent),

    ui(new Ui::MyTasks),
    tcpSocket(new QTcpSocket(this))
{
    this->loggin=loggin;
    this->passWord=passWord;
    this->adminTask=adminTask;
    this->managerTask=managerTask;
    ui->setupUi(this);
}


QVector<QStringList> MyTasks::getTask(QString s){
    tcpSocket->abort();
    tcpSocket->connectToHost("127.0.0.1",55555);//подключаемся к серверу

    //передаем
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    QByteArray reqCli=QByteArray()+"{\"name\":\""+loggin.toUtf8()+"\",\"pass\":\""+passWord.toUtf8()+"\",\"operation\":\"getMyTask\""+s.toUtf8()+"}";
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


void MyTasks::viewMyTasks(){
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
    refreshButton = new QPushButton("Обновить", this);
    filtersButton = new QPushButton("Фильтры", this);


    connect(refreshButton, &QPushButton::clicked, this, &MyTasks::onRefreshButtonClicked);
    connect(filtersButton, &QPushButton::clicked, this, &MyTasks::onFiltersButtonClicked);

    // Создаем верхний виджет для отображения логина и кнопки выхода
    QString RealName=getRealName();
    topWidget = new QWidget(this);
    QHBoxLayout *topLayout = new QHBoxLayout(topWidget);
    // Добавляем текст логина и кнопку добавиь задачу в верхний виджет
    loginLabel = new QLabel("ФИО: "+RealName, this);
    loginLabel->setStyleSheet("font-weight: bold;");  // Настройка стиля, по желанию
    QPushButton *logoutButton = new QPushButton("Выход", this);
    connect(logoutButton, &QPushButton::clicked, this, &MyTasks::onLogoutButtonClicked);

    topLayout->addWidget(loginLabel);
    topLayout->addStretch();  // Пространство для выравнивания вправо
    topLayout->addWidget(logoutButton);

    // Создаем горизонтальный слой для размещения кнопок "Мои задачи", "Назначение задач" и "Администрирование"

    QWidget *underTopWidget = new QWidget(this);
    QHBoxLayout *buttonRowLayout = new QHBoxLayout(underTopWidget);

    // Создаем кнопки "Мои задачи", "Назначение задач" и "Администрирование"
    myTasksButton = new QPushButton("Мои задачи", this);
    buttonRowLayout->addWidget(myTasksButton);
    connect(myTasksButton, &QPushButton::clicked, this, &MyTasks::manageTask);
    if(managerTask){
        assignTasksButton = new QPushButton("Назначение задач", this);
        buttonRowLayout->addWidget(assignTasksButton);
        connect(assignTasksButton, &QPushButton::clicked, this, &MyTasks::viewTasks);
    }
    if(adminTask){
        administrationButton = new QPushButton("Администрирование", this);
        // connect(administrationButton, &QPushButton::clicked, this, &MyWidget::onAdministrationButtonClicked);
        buttonRowLayout->addWidget(administrationButton);
    }




    // Создаем вертикальный слой для размещения верхнего виджета, кнопок и таблицы
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(topWidget);
    layout->addWidget(underTopWidget);  // Добавляем горизонтальный слой с кнопками
    layout->addWidget(tableView);

    // Добавляем кнопки внизу виджета
    QHBoxLayout *buttonLayout = new QHBoxLayout(this);
    buttonLayout->addWidget(refreshButton);
    buttonLayout->addWidget(filtersButton);
    layout->addLayout(buttonLayout);

    /*стили*/
    QString logoutButtonStyle = "QPushButton {"
                                "   background-color: #f44336;"
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
                                "   background-color: #d32f2f;"
                                "}";

    logoutButton->setStyleSheet(logoutButtonStyle);
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



void MyTasks::insertRow(const QStringList &data) {
    QList<QStandardItem *> items;
    QString idTask=data.first();
    bool b=1;
    bool boolStatus=0;
    qint32 n=0;
    for (const QString &text : data) {
        if(b){
            b=0;
            continue;
        }
        if(n==1&&text=="Выполнено")boolStatus=1;
        QStandardItem *item = new QStandardItem(text);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        items.append(item);
        n++;
    }



    // Устанавливаем данные в модель

     model->appendRow(items);
    // Устанавливаем кнопку в ячейку таблицы
    if(boolStatus){
        tableView->setIndexWidget(model->index(model->rowCount() - 1, 5), new QLabel("Выполнено",this));
    }
    else{// Создаем кнопку для действия
        QPushButton *rowButton = new QPushButton("Выполнить", this);
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

}

void MyTasks::onRowButtonClicked(const QString &data) {


    tcpSocket->abort();
    tcpSocket->connectToHost("127.0.0.1",55555);//подключаемся к серверу

    //передаем
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    QByteArray reqCli=QByteArray()+"{\"name\":\""+loggin.toUtf8()+"\",\"pass\":\""+passWord.toUtf8()+"\",\"operation\":\"completed\",\"id\":\""+data.toUtf8()+"\"}";
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
        return ;
    }
    onRefreshButtonClicked();

}


void MyTasks::onRefreshButtonClicked() {
    QVector<QStringList> tableTask=getTask();
    model->clear();
    model->setColumnCount(6);
    model->setHorizontalHeaderLabels(QStringList() <<"Имя задачи"<< "Статус задачи"<<"ФИО автора задачи"<< "Дата выдачи"<< "Дата окончания"<<"выполнить задачу?");


    for(auto& x:tableTask){

        insertRow(x);
     }

}

void MyTasks::onLogoutButtonClicked() {

    emit exitSystem();

}


void MyTasks::onFiltersButtonClicked() {
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
//

QString MyTasks::getRealName() {


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




MyTasks::~MyTasks()
{
    delete ui;
}
