#include "main.h"
#include "widget/errors_and_warnings/errors_and_warnings.h"
#include "ui_errors_and_warnings.h"

ErrorsAndWarnings::ErrorsAndWarnings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ErrorsAndWarnings)
{
    ui->setupUi(this);
    
    // Set header labels
    QStringList headers;
    headers << "Type" << "Line" << "Message";
    ui->tableWidget->setHorizontalHeaderLabels(headers);
}

ErrorsAndWarnings::~ErrorsAndWarnings()
{
    delete ui;
}