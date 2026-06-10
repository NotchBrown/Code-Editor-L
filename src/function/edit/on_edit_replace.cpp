#include "main.h"
#include "widget/main_window/main_window.h"
#include "widget/editor/code_editor.h"

void MainWindow::onEditReplace()
{
    CodeEditor *editor = currentEditor();
    if (!editor) return;
    
    QString findText = QInputDialog::getText(this, tr("Replace"), tr("Find what:"), QLineEdit::Normal, "", nullptr);
    
    if (!findText.isEmpty()) {
        QString replaceText = QInputDialog::getText(this, tr("Replace"), tr("Replace with:"), QLineEdit::Normal, "", nullptr);
        if (editor->findFirst(findText, false, false, false, false, false, 0, 0, false, false, false)) {
            editor->replace(replaceText);
        }
    }
}
