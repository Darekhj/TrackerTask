#ifndef GIVETASK_H
#define GIVETASK_H

#include <QWidget>
#include <QTcpSocket>
#include <QTableWidget>
#include <QStandardItemModel>
#include <QLabel>
namespace Ui {
class GiveTask;
}

class GiveTask : public QWidget
{
    Q_OBJECT

public:
    explicit GiveTask(QWidget *parent = nullptr);
    GiveTask(QString loggin,QString passWord,bool managerTask,bool adminTask,QWidget *parent= nullptr);
    ~GiveTask();

    QStandardItemModel * model ;
    QTableView *tableView;

    QVector<QStringList> getMySubordinates(QString s="");
    void viewySubordinates();

signals:
    void exitSystem();
    void manageTask();
    void viewTasks();

private:
    Ui::GiveTask *ui;

    QLabel *loginLabel;
    QString loggin,passWord;
    bool adminTask,managerTask;
    QTcpSocket *tcpSocket = nullptr;

    void insertRow(const QStringList &data);
    void onRowButtonClicked(const QString &data);
    void onBackButtonClicked();
    void onFiltersButtonClicked();
    void onLogoutButtonClicked();
    void onRefreshButtonClicked() ;
    QString getRealName() ;



};

#endif // GIVETASK_H
