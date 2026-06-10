#include "main.h"
#include "widget/main_window/main_window.h"
#include "ui_main_window.h"
#include "widget/editor/code_editor.h"

void MainWindow::onEditorModificationChanged(CodeEditor *editor, bool modified)
{
    Q_UNUSED(editor);
    updateWindowTitle();
    ui->statusBar->showMessage(modified ? tr("Modified") : tr("Saved"));
}
