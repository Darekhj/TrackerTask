#include <trackerserver.h>
#include <trackerthread.h>
#include <QtSql>


trackerServer::trackerServer(QObject *parent)
    : QTcpServer(parent){
    QSqlDatabase dbase = QSqlDatabase::addDatabase("QSQLITE");//если не созданы таблицы, тут создаются
    dbase.setDatabaseName("Tracker.sqlite");
    if (!dbase.open()) {
        return;
        qDebug() << "Error open database";
    }
    QSqlQuery a_query;




    QString str="CREATE TABLE departments ("
            "id INTEGER PRIMARY KEY NOT NULL,"
            "departments_name varchar(20) NOT NULL"
            ");";
    bool b = a_query.exec(str);
    if (!b) {
        qDebug() << "Error create table";
    }
    str="CREATE TABLE positions  ("
                "id INTEGER PRIMARY KEY NOT NULL,"
                  "position_subordinate INTEGER, "

                  "positionss_name varchar(20) NOT NULL,"
                 "FOREIGN KEY (position_subordinate)  REFERENCES positions(id)"
                  ");";
    b = a_query.exec(str);
    if (!b) {
        //qDebug() << "Error create table";
    }

    str="CREATE TABLE role  ("
          "id INTEGER PRIMARY KEY NOT NULL,"
          "role_name varchar(20) NULL "
          ");";
    b = a_query.exec(str);
    if (!b) {
        //qDebug() << "Error create table";
    }
    str = "CREATE TABLE user ("
          "user_name varchar(20) PRIMARY KEY NOT NULL, "
          "user_realname varchar(64) NOT NULL, "
          "user_password varchar(64) NOT NULL,"
          "positions INTEGER NOT NULL,"
          "role INTEGER NOT NULL,"
          "departments INTEGER NOT NULL,"
          "FOREIGN KEY (departments)  REFERENCES departments(id),"
          "FOREIGN KEY (positions)  REFERENCES positions(id),"
          "FOREIGN KEY (role)  REFERENCES role(id)"
          ");";
    b = a_query.exec(str);
    if (!b) {
        //qDebug() << "Error create table";
    }
    str="CREATE TABLE task  ("
          "id INTEGER PRIMARY KEY NOT NULL AUTOINCREMENT,"
          "user_name varchar(20),"
          "task_name varchar(20) NOT NULL,"
          "departments INTEGER NOT NULL,"
          "user_name_manager varchar(20),"
          "positionss_name INTEGER NOT NULL,"
          "status varchar(20),"
          "date_begin date NOT NULL default current_timestamp,"
          "date_end date NOT NULL default current_timestamp,"
          "FOREIGN KEY (user_name)  REFERENCES user (user_name),"
          "FOREIGN KEY (user_name_manager)  REFERENCES user (user_name),"
          "FOREIGN KEY (departments)  REFERENCES departments (id),"
          "FOREIGN KEY (positionss_name)  REFERENCES positions(id)"
          ");";
    b = a_query.exec(str);
    if (!b) {
        //qDebug() << "Error create table";
    }


    //добавим пользователя
    /*QByteArray hash = QCryptographicHash::hash(QString("1qwerty").toUtf8(), QCryptographicHash::Sha256);
    a_query.prepare("INSERT INTO user (user_name, user_password)"
                    "VALUES (?, ?);");
    a_query.addBindValue("a");
    a_query.addBindValue("1");
    a_query.exec();
    dbase.close();*/
}

void trackerServer::incomingConnection(qintptr socketDescriptor)
{
    TrackerThread *thread = new TrackerThread(socketDescriptor, this);
    connect(thread, &TrackerThread::finished, thread, &TrackerThread::deleteLater);//связь сигнала финиша и высвобождения
    thread->start();
}
