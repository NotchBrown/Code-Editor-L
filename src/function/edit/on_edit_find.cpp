#include "main.h"
#include "widget/main_window/main_window.h"
#include "widget/editor/code_editor.h"

void MainWindow::onEditFind()
{
    CodeEditor *editor = currentEditor();
    if (!editor) return;
    
    QString text = QInputDialog::getText(this, tr("Find"), tr("Find what:"), QLineEdit::Normal, "", nullptr);
    
    if (!text.isEmpty()) {
        editor->findFirst(text, false, false, false, false, false, 0, 0, false, false, false);
    }
}
