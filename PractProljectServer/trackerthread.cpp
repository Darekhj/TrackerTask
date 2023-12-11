#include <trackerthread.h>

#include <QtNetwork>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

#include <QtSql>
TrackerThread::TrackerThread(qintptr socketDescriptor, QObject *parent)
    : QThread(parent), socketDescriptor(socketDescriptor)
{

}

void TrackerThread::queryThreadTrackerSelect(QString &query, QList<QList<QString>>& result){
     {
        QSqlDatabase::addDatabase("QSQLITE",QString::number(socketDescriptor));
        QSqlDatabase dbase =QSqlDatabase::database(QString::number(socketDescriptor)) ;
        dbase.setDatabaseName("Tracker.sqlite");
        if (!dbase.open()) {
            return;
        }

        QSqlQuery a_query(dbase);

        // DDL query
        bool b=a_query.exec(query);
        if (!b) {
            qDebug() << "Error request";
        }
        qint64 n= a_query.record().count();
        while(a_query.next()){
            result.push_back(QList<QString>());
            for(int i=0;i<n;i++){
                result.back().push_back(a_query.value(i).toString());
            }
        }
        dbase.close();
    }
    QSqlDatabase::removeDatabase(QString::number(socketDescriptor));
}


QString TrackerThread::logginTracker(QJsonDocument &reqServJson){
    QString queryServ="SELECT user_name, user_password "
                       "FROM user "
                       "WHERE user_name = \""+reqServJson.object().value("name").toString()+"\" AND user_password = \""+reqServJson.object().value("pass").toString()+"\";";

    QList<QList<QString>> result;
    queryThreadTrackerSelect(queryServ,result);
    if(result.size()==0)return "{\"answer\":\"no\"}";
    return "{\"answer\":\"yes\"}";
}




QString TrackerThread::getRealName(QString name){
    QString queryServ="SELECT user_realname "
                          "FROM user "
                          "WHERE user_name = \""+name+"\";";

    QList<QList<QString>> result;
    queryThreadTrackerSelect(queryServ,result);
    return result[0][0];
}

QString TrackerThread::getRealName(QJsonDocument &reqServJson){
    QString tempRaelName=getRealName(reqServJson.object().value("employee").toString());
    if(reqServJson.object().value("employee").toString()==reqServJson.object().value("name").toString()||isSubordinate(reqServJson)){
        return "{\"answer\":\"yes\", \"realName\":\""+tempRaelName+"\"}";
    }
     return "{\"answer\":\"no\"}";
}

QString TrackerThread::getPositing(QString name){
    QString queryServ="SELECT positions "
                          "FROM user "
                          "WHERE user_name = \""+name+"\";";

    QList<QList<QString>> result;
    queryThreadTrackerSelect(queryServ,result);
    return result[0][0];
}

bool TrackerThread::isSubordinate(QJsonDocument &reqServJson){
    QString queryServ="SELECT id "
                          "FROM positions "
                          "WHERE position_subordinate = \""+getPositing(reqServJson.object().value("name").toString())+"\" AND id = \""+getPositing(reqServJson.object().value("employee").toString())+"\";";
    QList<QList<QString>> result;
    queryThreadTrackerSelect(queryServ,result);
    return !result.empty();
}
bool TrackerThread::isManager(QJsonDocument &reqServJson){
    QString queryServ="SELECT user_name, user_realname, positionss_name, departments_name "
                          "FROM user "
                          "JOIN positions ON user.positions = positions.id,"
                          "departments ON user.departments = departments.id "
                          "WHERE positions.position_subordinate = \""+getPositing(reqServJson.object().value("name").toString())+"\";";
    QList<QList<QString>> result;
    queryThreadTrackerSelect(queryServ,result);
    return !result.empty();
}


QString TrackerThread::getOtherTask(QJsonDocument &reqServJson){
    QString queryServ="SELECT id, task_name, status,user_name_manager, date_begin, date_end "
                          "FROM task "
                          "WHERE user_name = \""+reqServJson.object().value("employee").toString()+"\" ";
    if(reqServJson.object().value("filter").toString()!="on")queryServ+=";";
    else{
         queryServ+="AND LOWER(task_name) LIKE LOWER(\'%"+reqServJson.object().value("title").toString()+"%\') AND status=\""+reqServJson.object().value("status").toString()+"\";";

    }

    qint64 n=0;
    QList<QList<QString>> result;
    queryThreadTrackerSelect(queryServ,result);
    QString realName =getRealName(reqServJson.object().value("employee").toString());
    QString res="{\"answer\":\"yes\"";
    for(auto &x:result){
        res+=",\""+QString::number(n++)+"\":[";
        bool b=0;
        qint64 m=0;
        for(auto &y:x){
            if(b)res+=",";
            if(m==3&&y!="NULL")res+="\""+realName+"\"";
            else res+="\""+y+"\"";
            b=1;
            m++;
        }
        res+="]";
    }
    res+="}";

    return res;
}


QString TrackerThread::deleteTask(QJsonDocument &reqServJson){
    QString queryServ="DELETE  "
                          "FROM task "
                          "WHERE user_name = \""+reqServJson.object().value("employee").toString()+"\" AND  id=\""+reqServJson.object().value("id").toString()+"\";";

    qint64 n=0;
    QList<QList<QString>> result;
    queryThreadTrackerSelect(queryServ,result);
    QString res="{\"answer\":\"yes\"}";

    return res;
}


QString TrackerThread::getSubordinates(QJsonDocument &reqServJson){

    QString queryServ="SELECT user_name, user_realname, positionss_name, departments_name "
                          "FROM user "
                          "JOIN positions ON user.positions = positions.id,"
                          "departments ON user.departments = departments.id "
                          "WHERE positions.position_subordinate = \""+getPositing(reqServJson.object().value("name").toString())+"\" ";
    if(reqServJson.object().value("filter").toString()!="on")queryServ+=";";
    else{
        queryServ+="AND LOWER(user_realname) LIKE LOWER(\'%"+reqServJson.object().value("realName").toString()+"%\') AND LOWER(positionss_name) LIKE LOWER(\'%"+reqServJson.object().value("position").toString()+"%\');";

    }

    qint64 n=0;
    QList<QList<QString>> result;
    queryThreadTrackerSelect(queryServ,result);
    QString res="{\"answer\":\"yes\"";
    for(auto &x:result){
        res+=",\""+QString::number(n++)+"\":[";
        bool b=0;
        qint64 m=0;
        for(auto &y:x){
            if(b)res+=",";
            res+="\""+y+"\"";
            b=1;
            m++;
        }
        res+="]";
    }
    res+="}";

    return res;
}

QString TrackerThread::UpdateTaskStatus(QJsonDocument &reqServJson){
    QString queryServ="UPDATE task "
                          "SET status = \"Выполнено\" "
                          "WHERE id = \""+reqServJson.object().value("id").toString()+"\";";

    QList<QList<QString>> result;
    queryThreadTrackerSelect(queryServ,result);
    return "{\"answer\":\"yes\"}";
}


QString TrackerThread::GiveTask(QJsonDocument &reqServJson){

    QString queryServ="INSERT INTO task "
        "(id, user_name, task_name, departments, user_name_manager, positionss_name, status, date_begin, date_end) "
        "VALUES ("
        "null, \"" + reqServJson.object().value("employee").toString() + "\", \"" + reqServJson.object().value("nameTask").toString() + "\", "
        "(SELECT departments FROM user WHERE user_name = \"" + reqServJson.object().value("employee").toString() + "\"), \"" + reqServJson.object().value("name").toString() + "\", "
        "(SELECT positions FROM user WHERE user_name = \"" + reqServJson.object().value("employee").toString() + "\"), 'Выполняется', DATE('now'),DATE('"+reqServJson.object().value("dateEnd").toString()+"')"
        ");";

    QList<QList<QString>> result;
    queryThreadTrackerSelect(queryServ,result);
    return "{\"answer\":\"yes\"}";
}


QString TrackerThread::getTaskTracker(QJsonDocument &reqServJson){
    QString queryServ="SELECT id, task_name, status,user_name_manager, date_begin, date_end "
                          "FROM task "
                          "WHERE user_name = \""+reqServJson.object().value("name").toString()+"\" ";
    if(reqServJson.object().value("filter").toString()!="on")queryServ+=";";
    else{
        queryServ+="AND LOWER(task_name) LIKE LOWER(\'%"+reqServJson.object().value("title").toString()+"%\') AND status=\""+reqServJson.object().value("status").toString()+"\";";

    }

    qint64 n=0;
    QList<QList<QString>> result;
    queryThreadTrackerSelect(queryServ,result);
    QString res="{\"answer\":\"yes\"";
    for(auto &x:result){
        res+=",\""+QString::number(n++)+"\":[";
        bool b=0;
        qint64 m=0;
        for(auto &y:x){
            if(b)res+=",";
            if(m==3&&y!="NULL")res+="\""+getRealName(y)+"\"";
            else res+="\""+y+"\"";
            b=1;
            m++;
        }
        res+="]";
    }
    res+="}";

    return res;
}

void TrackerThread::run()
{
    QTcpSocket tcpSocket;
    if (!tcpSocket.setSocketDescriptor(socketDescriptor)) {

        return;
    }


    //считываем команду

    QDataStream in;
    in.setDevice(&tcpSocket);
    in.setVersion(QDataStream::Qt_6_5);
    QByteArray nextRequestServer;
    do{
        if(!tcpSocket.waitForReadyRead(500)){
            tcpSocket.disconnectFromHost();
            tcpSocket.waitForDisconnected();
            return;
        };
        in.startTransaction();
        in >> nextRequestServer;

    }while(!in.commitTransaction());

    QJsonParseError reqServJsonEr;
    QJsonDocument reqServJson=QJsonDocument::fromJson(nextRequestServer.data(),&reqServJsonEr);
    if (reqServJsonEr.error != QJsonParseError::NoError) {
        tcpSocket.disconnectFromHost();
        tcpSocket.waitForDisconnected();
        return;
    }

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_5);


    QString operationSelection=reqServJson.object().value("operation").toString();//что нибудь более быстрое-> добавить
    //передаем
    if(operationSelection=="loggin"){
        if(logginTracker(reqServJson)=="{\"answer\":\"no\"}"){
            out<<"{\"answer\":\"no\"}";
        }
        else{
            out<<"{\"answer\":\"yes\",\"isManager\":\""+QString::number(isManager(reqServJson)).toUtf8()+"\"}";
        }

    }
    else if(operationSelection=="getMyTask"){
        if(logginTracker(reqServJson)=="{\"answer\":\"no\"}"){
            out<<"{\"answer\":\"no\"}";
        }
        else{
            out<<getTaskTracker(reqServJson).toUtf8();
        }
    }
    else if(operationSelection=="completed"){
        if(logginTracker(reqServJson)=="{\"answer\":\"no\"}"){
            out<<"{\"answer\":\"no\"}";
        }
        else{
            out<<UpdateTaskStatus(reqServJson).toUtf8();
        }
    }
    else if(operationSelection=="getsubordinates"){
        if(logginTracker(reqServJson)=="{\"answer\":\"no\"}"){
            out<<"{\"answer\":\"no\"}";
        }
        else{
            out<<getSubordinates(reqServJson).toUtf8();
        }
    }
    else if(operationSelection=="getOtherTask"){
        if(!isSubordinate(reqServJson)||logginTracker(reqServJson)=="{\"answer\":\"no\"}"){
            out<<"{\"answer\":\"no\"}";
        }
        else{
            out<<getOtherTask(reqServJson).toUtf8();
        }
    }
    else if(operationSelection=="deleteTask"){
        if(!isSubordinate(reqServJson)||logginTracker(reqServJson)=="{\"answer\":\"no\"}"){
            out<<"{\"answer\":\"no\"}";
        }
        else{
            out<<deleteTask(reqServJson).toUtf8();
        }
    }
    else if(operationSelection=="getRealName"){
        if(logginTracker(reqServJson)=="{\"answer\":\"no\"}"){
            out<<"{\"answer\":\"no\"}";
        }
        else{
            out<<getRealName(reqServJson).toUtf8();
        }
    }
    else if(operationSelection=="GiveTask"){
        if(!isSubordinate(reqServJson)||logginTracker(reqServJson)=="{\"answer\":\"no\"}"){
            out<<"{\"answer\":\"no\"}";
        }
        else{
            out<<GiveTask(reqServJson).toUtf8();
        }
    }

    tcpSocket.write(block);

    tcpSocket.disconnectFromHost();
    tcpSocket.waitForDisconnected();

}




