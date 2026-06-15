#include "widget/go_line/go_line.h"
#include "ui_go_line.h"

GoLineDialog::GoLineDialog(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::GoLineDialog)
{
    ui->setupUi(this);
    ui->lineSpinBox->setFocus();
}

GoLineDialog::~GoLineDialog()
{
    delete ui;
}

void GoLineDialog::setMaxLine(int maxLine)
{
    ui->lineSpinBox->setMaximum(maxLine);
}

int GoLineDialog::lineNumber() const
{
    return ui->lineSpinBox->value();
}

void GoLineDialog::setLineNumber(int line)
{
    ui->lineSpinBox->setValue(line);
}