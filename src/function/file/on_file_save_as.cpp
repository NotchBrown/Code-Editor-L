#include "main.h"
#include "widget/main_window/main_window.h"
#include "widget/editor/code_editor.h"
#include "ui_main_window.h"

void MainWindow::onFileSaveAs()
{
    CodeEditor *editor = currentEditor();
    if (!editor) return;
    
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save File"), 
        editor->filePath(), 
        tr("All Files (*);;C/C++ Files (*.cpp *.h *.hpp *.cxx);;Python Files (*.py);;HTML Files (*.html *.htm);;JavaScript Files (*.js);;JSON Files (*.json);;XML Files (*.xml);;SQL Files (*.sql);;Lua Files (*.lua);;Shell Files (*.sh *.bash)"));
    
    if (!filePath.isEmpty()) {
        if (!editor->filePath().isEmpty()) {
            m_fileToTabs.remove(editor->filePath());
        }
        
        QTabWidget *tabWidget = findChild<QTabWidget*>("mainTabWidget");
        
        editor->setFilePath(filePath);
        QFileInfo info(filePath);
        m_fileToTabs[filePath] = QPair<CodeEditor*, int>(editor, tabWidget->currentIndex());
        
        if (editor->saveFile(filePath)) {
            tabWidget->setTabText(tabWidget->currentIndex(), info.fileName());
            editor->setModified(false);
            updateWindowTitle();
            updateOpenTabsMenu();
            ui->statusBar->showMessage(tr("Saved as: %1").arg(filePath));
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Cannot save file"));
        }
    }
}
