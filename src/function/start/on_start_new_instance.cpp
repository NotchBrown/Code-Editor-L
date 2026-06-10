#include "main.h"
#include "widget/main_window/main_window.h"

void MainWindow::onStartNewInstance()
{
    QProcess::startDetached(QCoreApplication::applicationFilePath());
}
