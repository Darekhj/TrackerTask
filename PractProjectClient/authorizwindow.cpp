#include "authorizwindow.h"
#include "ui_authorizwindow.h"

#include<QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
AuthorizWindow::AuthorizWindow(QWidget *parent) ://Виджет авторизации
    QWidget(parent),
    ui(new Ui::AuthorizWindow)
    , tcpSocket(new QTcpSocket(this))
{

    ui->setupUi(this);
    connect(ui->authorizationButton,&QPushButton::clicked,this,&AuthorizWindow::requestAuthoriz);

}


AuthorizWindow::~AuthorizWindow()
{   delete tcpSocket;
    delete ui;
}


void AuthorizWindow::requestAuthoriz()//нажатие на кнопку входа
{
    this->setWindowTitle("Вход");
    //getFortuneButton->setEnabled(false);
    tcpSocket->abort();
    tcpSocket->connectToHost("127.0.0.1",55555);//подключаемся к серверу

    //передаем
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    loggin=ui->authName->text();
    passWord=ui->authPass->text();
    QByteArray reqCli=QByteArray()+"{\"name\":\""+(ui->authName->text().toUtf8())+"\",\"pass\":\""+ui->authPass->text().toUtf8()+"\",\"operation\":\"loggin\"}";
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
        }
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
    QString answerServer=reqClientJson.object().value("answer").toString();
    if(answerServer=="yes"){

        emit sendNumbers(ui->authName->text().toUtf8(),ui->authPass->text().toUtf8(),(reqClientJson.object().value("isManager").toString()=="1"));
    }
    else{

        QMessageBox::warning(this, "Ошибка логина или пароля", "Пожалуйста, попробуйте повторно.");
    }
}

