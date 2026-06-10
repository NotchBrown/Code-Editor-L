#include "main.h"
#include "widget/main_window/main_window.h"

void MainWindow::onCurrentTabChanged(int index)
{
    Q_UNUSED(index);
    updateWindowTitle();
    updateOpenTabsMenu();
}
