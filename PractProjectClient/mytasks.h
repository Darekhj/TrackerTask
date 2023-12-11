#ifndef MYTASKS_H
#define MYTASKS_H

#include <QWidget>
#include <QTcpSocket>
#include <QTableWidget>
#include <QStandardItemModel>
#include <QLabel>
#include <QPushButton>
namespace Ui {
class MyTasks;
}

class MyTasks : public QWidget
{
    Q_OBJECT

public:
    explicit MyTasks(QWidget *parent = nullptr);
    ~MyTasks();
    MyTasks(QString loggin,QString passWord,bool managerTask, bool adminTask, QWidget *parent=nullptr);
    QVector<QStringList> getTask(QString s="");
    void viewMyTasks();
    QStandardItemModel * model ;
    QTableView *tableView;

signals:
    void exitSystem();
    void manageTask();
    void viewTasks();

private:
    QLabel *loginLabel;
    QString loggin,passWord;
    Ui::MyTasks *ui;
    bool adminTask,managerTask;
    QTcpSocket *tcpSocket = nullptr;
    QPushButton *refreshButton=nullptr;
    QPushButton *myTasksButton =nullptr;
    QPushButton *assignTasksButton =nullptr;
    QPushButton *administrationButton=nullptr;
    QWidget *topWidget=nullptr;
    QPushButton *filtersButton =nullptr;
    void applyStyles();
    void insertRow(const QStringList &data);
    void onRowButtonClicked(const QString &data);
    void onFiltersButtonClicked();
    void onLogoutButtonClicked();
    void onRefreshButtonClicked() ;
    QString getRealName() ;

};

#endif // MYTASKS_H
