#include "main.h"
#include "widget/main_window/main_window.h"
#include "ui_main_window.h"
#include "widget/find_and_replace/find_and_replace.h"

void MainWindow::onWindowFindAndReplace()
{
    QDockWidget *dock = findChild<QDockWidget*>("dockFindReplace");
    if (dock && ui->actionWindowFindAndReplace) {
        bool visible = !dock->isVisible();
        dock->setVisible(visible);
        ui->actionWindowFindAndReplace->setChecked(visible);
        if (visible) {
            dock->raise();
            m_findAndReplace->activateReplaceMode();
            m_findAndReplace->focusOnSearch();
        }
    }
}
