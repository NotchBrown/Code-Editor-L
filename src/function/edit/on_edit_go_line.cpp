#include "main.h"
#include "widget/main_window/main_window.h"
#include "widget/editor/code_editor.h"
#include "widget/go_line/go_line.h"

void MainWindow::onEditGoLine()
{
    CodeEditor *editor = currentEditor();
    if (!editor) return;
    
    GoLineDialog dialog(this);
    dialog.setMaxLine(editor->lines());
    dialog.setLineNumber(editor->currentLine() + 1);
    
    if (dialog.exec() == QDialog::Accepted) {
        editor->goToLine(dialog.lineNumber());
    }
}