#include "main.h"
#include "widget/main_window/main_window.h"
#include "ui_main_window.h"
#include "widget/ipc_message/ipc_message.h"

void MainWindow::onWindowIPCMessage()
{
    QDockWidget *dock = findChild<QDockWidget*>("dockIPCMessage");
    if (dock && ui->actionWindowIPCMessage) {
        bool visible = !dock->isVisible();
        dock->setVisible(visible);
        ui->actionWindowIPCMessage->setChecked(visible);
    }
}
