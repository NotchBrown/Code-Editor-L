#include "main.h"
#include "widget/main_window/main_window.h"
#include "widget/editor/code_editor.h"

void MainWindow::onEditGoLine()
{
    CodeEditor *editor = currentEditor();
    if (!editor) return;
    
    int maxLine = editor->lines();
    bool ok;
    int line = QInputDialog::getInt(this, tr("Go to Line"), tr("Line number:"), 1, 1, maxLine, 1, &ok);
    
    if (ok) {
        editor->goToLine(line);
    }
}
