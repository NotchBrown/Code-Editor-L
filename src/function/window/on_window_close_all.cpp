#include "main.h"
#include "widget/main_window/main_window.h"

void MainWindow::onWindowCloseAll()
{
    QTabWidget *tabWidget = findChild<QTabWidget*>("mainTabWidget");
    for (int i = tabWidget->count() - 1; i >= 0; --i) {
        closeTab(i);
    }
}
