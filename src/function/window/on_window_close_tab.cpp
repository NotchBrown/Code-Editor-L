#include "main.h"
#include "widget/main_window/main_window.h"
#include "ui_main_window.h"

void MainWindow::onWindowCloseTab()
{
    QTabWidget *tabWidget = findChild<QTabWidget*>("mainTabWidget");
    if (tabWidget) {
        closeTab(tabWidget->currentIndex());
    }
}
