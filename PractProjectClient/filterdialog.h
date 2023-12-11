#ifndef FILTERDIALOG_H
#define FILTERDIALOG_H

#include <QDialog>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QDateEdit>
#include <QComboBox>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>

class FilterDialog: public QDialog
{
public:

    FilterDialog(QWidget *parent = nullptr);
    QString getTitle() const ;
    /*
    QDate getStartDateFrom() const {
        return startDateEditFrom->date();
    }

    QDate getEndDateFrom() const {
        return endDateEditFrom->date();
    }

    QDate getStartDateTo() const {
        return startDateEditTo->date();
    }

    QDate getEndDateTo() const {
        return endDateEditTo->date();
    }
*/

    QString getStatus() const;
private:
    QLineEdit *titleEdit;
    QDateEdit *startDateEditFrom;
    QDateEdit *endDateEditFrom;
    QDateEdit *startDateEditTo;
    QDateEdit *endDateEditTo;
    QComboBox *statusComboBox;
};

#endif // FILTERDIALOG_H
