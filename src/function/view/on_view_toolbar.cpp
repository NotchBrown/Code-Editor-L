#include "main.h"
#include "widget/main_window/main_window.h"

void MainWindow::onViewToolbar()
{
    QToolBar *toolBar = findChild<QToolBar*>("mainToolBar");
    if (toolBar) {
        toolBar->setVisible(!toolBar->isVisible());
    }
}
