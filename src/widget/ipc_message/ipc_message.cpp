#include "main.h"
#include "widget/ipc_message/ipc_message.h"
#include "ui_ipc_message.h"

IPCMessage::IPCMessage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IPCMessage)
{
    ui->setupUi(this);
}

IPCMessage::~IPCMessage()
{
    delete ui;
}

void IPCMessage::appendMessage(const QString &message)
{
    ui->textEdit->append(message);
}