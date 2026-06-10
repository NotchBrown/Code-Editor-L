#ifndef IPC_MESSAGE_H
#define IPC_MESSAGE_H

#include "main.h"

namespace Ui {
class IPCMessage;
}

class IPCMessage : public QWidget
{
    Q_OBJECT

public:
    explicit IPCMessage(QWidget *parent = nullptr);
    ~IPCMessage();

    void appendMessage(const QString &message);

private:
    Ui::IPCMessage *ui;
};

#endif // IPC_MESSAGE_H