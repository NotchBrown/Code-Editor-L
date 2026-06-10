#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include "main.h"

namespace Ui {
class Navigator;
}

class Navigator : public QWidget
{
    Q_OBJECT

public:
    explicit Navigator(QWidget *parent = nullptr);
    ~Navigator();

private:
    Ui::Navigator *ui;
};

#endif // NAVIGATOR_H