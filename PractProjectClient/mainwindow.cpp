#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "authorizwindow.h"
#include "givetask.h"

#include "mytasks.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{


    authoriz= new AuthorizWindow();
    MainWindow::setCentralWidget(authoriz);
    connect(authoriz, &AuthorizWindow::sendNumbers, this, &MainWindow::logginSystem);




}

void MainWindow::logginSystem(QString log, QString pass, bool manager){
    this->setWindowTitle("Трекер задач");
    delete authoriz;
    authoriz=nullptr;
    loggin=log,passWord=pass;
    adminTask=0,managerTask=manager;
    task=new MyTasks(loggin,passWord,managerTask,false);
    MainWindow::setCentralWidget(task);
    connect(task, &MyTasks::exitSystem, this, &MainWindow::loggOutSystem);
    connect(task, &MyTasks::manageTask, this, &MainWindow::manageTask);
    connect(task, &MyTasks::viewTasks, this, &MainWindow::viewTasks);

    task->viewMyTasks();
}

void MainWindow::loggOutSystem(){
    loggin="",passWord="";
    adminTask=0,managerTask=0;
    delete authoriz;
    authoriz=nullptr;
    delete subordinates;
    subordinates=nullptr;
    delete task;
    task=nullptr;
    authoriz= new AuthorizWindow();
    MainWindow::setCentralWidget(authoriz);
    connect(authoriz, &AuthorizWindow::sendNumbers, this, &MainWindow::logginSystem);


}
void MainWindow::viewTasks(){
    delete authoriz;
    authoriz=nullptr;
    delete subordinates;
    subordinates=nullptr;
    delete task;
    task=nullptr;
    subordinates=new GiveTask(loggin,passWord,managerTask,false);
    MainWindow::setCentralWidget(subordinates);
    connect(subordinates, &GiveTask::exitSystem, this, &MainWindow::loggOutSystem);
    connect(subordinates, &GiveTask::manageTask, this, &MainWindow::manageTask);
    connect(subordinates, &GiveTask::viewTasks, this, &MainWindow::viewTasks);
    subordinates->viewySubordinates();


}
void MainWindow::manageTask(){
    delete authoriz;
    authoriz=nullptr;
    delete subordinates;
    subordinates=nullptr;
    delete task;
    task=nullptr;
    task=new MyTasks(loggin,passWord,managerTask,false);
    MainWindow::setCentralWidget(task);
    connect(task, &MyTasks::exitSystem, this, &MainWindow::loggOutSystem);
    connect(task, &MyTasks::manageTask, this, &MainWindow::manageTask);
    connect(task, &MyTasks::viewTasks, this, &MainWindow::viewTasks);
    task->viewMyTasks();

}


MainWindow::~MainWindow()
{
    delete ui;
}




