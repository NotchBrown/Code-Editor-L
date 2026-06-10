#include "main.h"
#include "widget/main_window/main_window.h"
#include "ui_main_window.h"
#include "widget/segment/segment.h"

void MainWindow::onWindowSegment()
{
    QDockWidget *dock = findChild<QDockWidget*>("dockSegment");
    if (dock && ui->actionWindowSegment) {
        bool visible = !dock->isVisible();
        dock->setVisible(visible);
        ui->actionWindowSegment->setChecked(visible);
    }
}
