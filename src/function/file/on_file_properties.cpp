#include "main.h"
#include "widget/main_window/main_window.h"
#include "widget/editor/code_editor.h"

void MainWindow::onFileProperties()
{
    CodeEditor *editor = currentEditor();
    if (!editor) return;
    
    QString filePath = editor->filePath();
    if (filePath.isEmpty()) {
        QMessageBox::information(this, tr("File Properties"), tr("This is a new unsaved file."));
        return;
    }
    
    QFileInfo info(filePath);
    QString properties = QString(
        "File: %1\n"
        "Size: %2 bytes\n"
        "Created: %3\n"
        "Modified: %4\n"
        "Read Only: %5"
    ).arg(
        info.fileName(),
        QString::number(info.size()),
        info.birthTime().toString(),
        info.lastModified().toString(),
        !info.permission(QFile::WriteUser) ? tr("Yes") : tr("No")
    );
    
    QMessageBox::information(this, tr("File Properties"), properties);
}
