#ifndef SEGMENT_H
#define SEGMENT_H

#include "main.h"

namespace Ui {
class Segment;
}

class Segment : public QWidget
{
    Q_OBJECT

public:
    explicit Segment(QWidget *parent = nullptr);
    ~Segment();

private:
    Ui::Segment *ui;
};

#endif // SEGMENT_H