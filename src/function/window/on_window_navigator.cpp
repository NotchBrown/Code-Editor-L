#include "main.h"
#include "widget/main_window/main_window.h"
#include "ui_main_window.h"
#include "widget/navigator/navigator.h"

void MainWindow::onWindowNavigator()
{
    QDockWidget *dock = findChild<QDockWidget*>("dockNavigator");
    if (dock && ui->actionWindowNavigator) {
        bool visible = !dock->isVisible();
        dock->setVisible(visible);
        ui->actionWindowNavigator->setChecked(visible);
    }
}
