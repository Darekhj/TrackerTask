#include "filterdialog.h"

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QDateEdit>
#include <QComboBox>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>

FilterDialog::FilterDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Фильтры");

    // Создаем виджеты для ввода данных
    QLabel *titleLabel = new QLabel("Название:", this);
    titleEdit = new QLineEdit(this);

   /* QLabel *startDateLabelFrom = new QLabel("Дата начала от:", this);
    startDateEditFrom = new QDateEdit(this);

    QLabel *startDateLabeTo = new QLabel("Дата начала до:", this);
    startDateEditTo= new QDateEdit(this);


    QLabel *endDateLabelFrom = new QLabel("Дата конца от:", this);
    endDateEditFrom = new QDateEdit(this);

    QLabel *endDateLabelTo = new QLabel("Дата конца До:", this);
    endDateEditTo = new QDateEdit(this);
    */

    QLabel *statusLabel = new QLabel("Статус:", this);
    statusComboBox = new QComboBox(this);
    statusComboBox->addItem("Выполняется");
    statusComboBox->addItem("Выполнено");

    // Кнопки "Применить" и "Отмена"
    QPushButton *applyButton = new QPushButton("Применить", this);
    QPushButton *cancelButton = new QPushButton("Отмена", this);

    // Подключаем сигналы от кнопок к слотам
    connect(applyButton, &QPushButton::clicked, this, &FilterDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &FilterDialog::reject);

    // Размещаем виджеты на диалоге
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(titleLabel);
    layout->addWidget(titleEdit);

    /*layout->addWidget(startDateLabelFrom);
    layout->addWidget(startDateEditFrom);

    layout->addWidget(startDateLabeTo);
    layout->addWidget(startDateEditTo);

    layout->addWidget(endDateLabelFrom);
    layout->addWidget(endDateEditFrom);

    layout->addWidget(endDateLabelTo);
    layout->addWidget(endDateEditTo);

    */
    layout->addWidget(statusLabel);
    layout->addWidget(statusComboBox);

    // Размещаем кнопки в горизонтальном слое
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(applyButton);
    buttonLayout->addWidget(cancelButton);
    layout->addLayout(buttonLayout);

    setLayout(layout);
}

QString FilterDialog::getTitle() const {
    return titleEdit->text();
}

QString FilterDialog::getStatus() const {
    return statusComboBox->currentText();
}
