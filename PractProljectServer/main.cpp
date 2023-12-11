#include <QCoreApplication>

#include <trackerserver.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    trackerServer server;
    if (!server.listen(QHostAddress::Any,55555)) {
        qDebug()<<"Error open server\n";
        return 0;
    }

    return a.exec();
}
