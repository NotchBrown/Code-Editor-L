#include "main.h"
#include "widget/main_window/main_window.h"
#include "widget/editor/code_editor.h"

void MainWindow::onMarkCreateBookmark()
{
    CodeEditor *editor = currentEditor();
    if (editor) editor->toggleBookmark(editor->currentLine());
}
