#ifndef TRACKERSERVER_H
#define TRACKERSERVER_H

#include <QTcpServer>

class trackerServer : public QTcpServer
{
    Q_OBJECT

public:
    trackerServer(QObject *parent = nullptr);

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private:
};

#endif // TRACKERSERVER_H
