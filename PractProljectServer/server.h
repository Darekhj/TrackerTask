#ifndef SERVER_H
#define SERVER_H

#include <QStringList>
#include <QTcpServer>

class Server : public QTcpServer
{
    Q_OBJECT

public:
    Server(QObject *parent = nullptr);

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private:

};


#endif // SERVER_H
