#include "main.h"
#include "widget/main_window/main_window.h"
#include "ui_main_window.h"

void MainWindow::onViewStatusbar()
{
    ui->statusBar->setVisible(!ui->statusBar->isVisible());
}
