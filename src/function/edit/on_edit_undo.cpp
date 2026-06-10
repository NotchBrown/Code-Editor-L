#include "main.h"
#include "widget/main_window/main_window.h"
#include "widget/editor/code_editor.h"

void MainWindow::onEditUndo()
{
    CodeEditor *editor = currentEditor();
    if (editor) editor->undo();
}
