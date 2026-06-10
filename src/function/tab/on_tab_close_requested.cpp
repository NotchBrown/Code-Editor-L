#include "main.h"
#include "widget/main_window/main_window.h"

void MainWindow::onTabCloseRequested(int index)
{
    closeTab(index);
}
