#ifndef GO_LINE_DIALOG_H
#define GO_LINE_DIALOG_H

#include "main.h"
#include <QDialog>

namespace Ui {
class GoLineDialog;
}

class GoLineDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GoLineDialog(QWidget *parent = nullptr);
    ~GoLineDialog();

    void setMaxLine(int maxLine);
    int lineNumber() const;
    void setLineNumber(int line);

private:
    Ui::GoLineDialog *ui;
};

#endif // GO_LINE_DIALOG_H