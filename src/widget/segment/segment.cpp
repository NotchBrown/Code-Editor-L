#include "main.h"
#include "widget/segment/segment.h"
#include "ui_segment.h"

Segment::Segment(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Segment)
{
    ui->setupUi(this);
}

Segment::~Segment()
{
    delete ui;
}