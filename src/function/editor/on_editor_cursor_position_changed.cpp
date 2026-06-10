#include "main.h"
#include "widget/main_window/main_window.h"
#include "ui_main_window.h"

void MainWindow::onEditorCursorPositionChanged(CodeEditor *editor, int line, int column)
{
    Q_UNUSED(editor);
    ui->statusBar->showMessage(tr("Line %1, Column %2").arg(line).arg(column));
}
