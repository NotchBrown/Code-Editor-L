#include "main.h"
#include "widget/main_window/main_window.h"
#include "ui_main_window.h"
#include "widget/errors_and_warnings/errors_and_warnings.h"

void MainWindow::onWindowErrorsWarnings()
{
    QDockWidget *dock = findChild<QDockWidget*>("dockErrorsWarnings");
    if (dock && ui->actionWindowErrorsWarnings) {
        bool visible = !dock->isVisible();
        dock->setVisible(visible);
        ui->actionWindowErrorsWarnings->setChecked(visible);
    }
}
