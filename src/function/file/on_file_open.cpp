#include "main.h"
#include "widget/main_window/main_window.h"

void MainWindow::onFileOpen()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), "", 
        tr("All Files (*);;C/C++ Files (*.cpp *.h *.hpp *.cxx);;Python Files (*.py);;HTML Files (*.html *.htm);;JavaScript Files (*.js);;JSON Files (*.json);;XML Files (*.xml);;SQL Files (*.sql);;Lua Files (*.lua);;Shell Files (*.sh *.bash)"));
    
    if (!filePath.isEmpty()) {
        createNewEditor(filePath);
    }
}
