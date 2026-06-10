#include "main.h"
#include "widget/main_window/main_window.h"

void MainWindow::onHelpAbout()
{
    QMessageBox::about(this, tr("About CodeEditorLite"), 
        tr("CodeEditorLite\n\nA lightweight code editor built with Qt and QScintilla.\n\nVersion: 1.0.0"));
}
