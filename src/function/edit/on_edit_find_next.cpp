#include "main.h"
#include "widget/main_window/main_window.h"
#include "widget/editor/code_editor.h"

void MainWindow::onEditFindNext()
{
    CodeEditor *editor = currentEditor();
    if (editor) {
        editor->findFirst("", false, false, false, false, false, 0, 0, false, false, false);
    }
}
