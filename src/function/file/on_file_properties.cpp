#include "main.h"
#include "widget/properties/properties.h"
#include "widget/main_window/main_window.h"
#include "widget/editor/code_editor.h"

void MainWindow::onFileProperties()
{
    // Get current file path from active editor
    QString filePath;
    CodeEditor *editor = MainWindow::currentEditor();
    if (editor) {
        filePath = editor->filePath();
    }
    
    // Create properties dialog
    Properties *properties = new Properties(this);
    properties->setWindowFlags(Qt::Window);
    properties->setAttribute(Qt::WA_DeleteOnClose);
    
    if (!filePath.isEmpty()) {
        properties->setFilePath(filePath);
    } else {
        properties->setWindowTitle("File Properties (No File)");
    }
    
    properties->show();
    properties->raise();
    properties->activateWindow();
}