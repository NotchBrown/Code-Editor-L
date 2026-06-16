#include "main.h"
#include "widget/main_window/main_window.h"
#include "ui_main_window.h"
#include "widget/find_and_replace/find_and_replace.h"

void MainWindow::onEditFindNext()
{
    CodeEditor *editor = currentEditor();
    if (!editor) return;

    // Ensure FindAndReplace panel is visible and knows about this editor
    QDockWidget *dock = findChild<QDockWidget*>("dockFindReplace");
    if (dock && !dock->isVisible()) {
        dock->setVisible(true);
    }
    m_findAndReplace->setEditor(editor);
    m_findAndReplace->onFindNext();
}
