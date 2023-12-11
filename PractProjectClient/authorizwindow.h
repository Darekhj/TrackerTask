#ifndef AUTHORIZWINDOW_H
#define AUTHORIZWINDOW_H


#include <QWidget>
#include <QDataStream>
#include <QDialog>
#include <QTcpSocket>

namespace Ui {
class AuthorizWindow;
}

class AuthorizWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AuthorizWindow(QWidget *parent = nullptr);
    ~AuthorizWindow();
QString loggin,passWord;
private:
    Ui::AuthorizWindow *ui;

    QTcpSocket *tcpSocket = nullptr;


signals:
    void sendNumbers(QString log, QString pass, bool manager);

private slots:


private slots:
    void requestAuthoriz();

};



#endif // AUTHORIZWINDOW_H
