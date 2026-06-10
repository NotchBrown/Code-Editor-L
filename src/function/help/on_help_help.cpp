#include "main.h"
#include "widget/main_window/main_window.h"

void MainWindow::onHelpHelp()
{
    QMessageBox::information(this, tr("Help"), tr("CodeEditorLite Help\n\nFor more information, please visit the project documentation."));
}
