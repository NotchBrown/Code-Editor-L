#ifndef ERRORS_AND_WARNINGS_H
#define ERRORS_AND_WARNINGS_H

#include "main.h"

namespace Ui {
class ErrorsAndWarnings;
}

class ErrorsAndWarnings : public QWidget
{
    Q_OBJECT

public:
    explicit ErrorsAndWarnings(QWidget *parent = nullptr);
    ~ErrorsAndWarnings();

private:
    Ui::ErrorsAndWarnings *ui;
};

#endif // ERRORS_AND_WARNINGS_H