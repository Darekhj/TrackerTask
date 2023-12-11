#ifndef TRACKERTHREAD_H
#define TRACKERTHREAD_H

#include <QThread>
#include <QTcpSocket>

class TrackerThread: public QThread{

    Q_OBJECT
public:
    TrackerThread(qintptr socketDescriptor, QObject *parent);

    void run() override;

signals:
    void error(QTcpSocket::SocketError socketError);

private:
    QString logginTracker(QJsonDocument &reqServJson);
    void queryThreadTrackerSelect(QString &query, QList<QList<QString>>& result);
    QByteArray hashSalt="qwerty";
    qintptr socketDescriptor;
    QString getTaskTracker(QJsonDocument &reqServJson);
    QString getRealName(QString name);
    QString UpdateTaskStatus(QJsonDocument &reqServJson);

    QString getSubordinates(QJsonDocument &reqServJson);
    QString getPositing(QString name);
    QString getOtherTask(QJsonDocument &reqServJson);
    QString deleteTask(QJsonDocument &reqServJson);
    QString getRealName(QJsonDocument &reqServJson);
    QString GiveTask(QJsonDocument &reqServJson);
    bool isSubordinate(QJsonDocument &reqServJson);
    bool isManager(QJsonDocument &reqServJson);

};


#endif // TRACKERTHREAD_H
