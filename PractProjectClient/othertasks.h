#ifndef OTHERTASKS_H
#define OTHERTASKS_H

#include <QWidget>
#include <QTcpSocket>
#include <QTableWidget>
#include <QStandardItemModel>
#include <QLabel>
namespace Ui {
class OtherTasks;
}

class OtherTasks : public QWidget
{
    Q_OBJECT

public:
    explicit OtherTasks(QWidget *parent = nullptr);
     OtherTasks(QString loggin,QString passWord,QString nameId, QWidget *parent=nullptr);
    ~OtherTasks();

     QVector<QStringList> getTask(QString s="");
     QStandardItemModel * model ;
     QTableView *tableView;


private:
    Ui::OtherTasks *ui;

    QTcpSocket *tcpSocket = nullptr;
    QString loggin,passWord,nameID;
    QLabel *loginLabel;
    void insertRow(const QStringList &data);
    void onRowButtonClicked(const QString &data);
    void onFiltersButtonClicked();
    void onRefreshButtonClicked() ;
    void onTaskGiveButtonClicked() ;
    QString getRealName() ;
};

#endif // OTHERTASKS_H
