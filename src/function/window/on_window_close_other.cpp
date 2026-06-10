#include "main.h"
#include "widget/main_window/main_window.h"

void MainWindow::onWindowCloseOther()
{
    QTabWidget *tabWidget = findChild<QTabWidget*>("mainTabWidget");
    int currentIndex = tabWidget->currentIndex();
    
    for (int i = tabWidget->count() - 1; i >= 0; --i) {
        if (i != currentIndex) {
            closeTab(i);
        }
    }
}
