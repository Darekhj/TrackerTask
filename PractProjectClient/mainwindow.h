#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "authorizwindow.h"
#include "givetask.h"
#include "mytasks.h"

#include <QMainWindow>
#include <QDataStream>
#include <QDialog>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }

class QComboBox;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QString loggin,passWord;
    bool adminTask,managerTask;
    GiveTask *subordinates=nullptr;
    MyTasks *task=nullptr;
    AuthorizWindow *authoriz=nullptr;
    Ui::MainWindow *ui;





private slots:
     void logginSystem(QString log, QString pass, bool manager);
     void loggOutSystem();
     void manageTask();
     void viewTasks();

};
#endif // MAINWINDOW_H
