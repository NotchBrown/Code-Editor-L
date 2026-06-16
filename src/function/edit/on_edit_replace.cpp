#include "main.h"
#include "widget/main_window/main_window.h"
#include "widget/editor/code_editor.h"
#include "ui_main_window.h"
#include "widget/find_and_replace/find_and_replace.h"

void MainWindow::onEditReplace()
{
    // Show the Find and Replace dock in Replace mode (replace enabled)
    QDockWidget *dock = findChild<QDockWidget*>("dockFindReplace");
    if (dock) {
        dock->setVisible(true);
        dock->raise();
        m_findAndReplace->activateReplaceMode();

        CodeEditor *editor = currentEditor();
        if (editor) {
            m_findAndReplace->setEditor(editor);
            if (editor->hasSelectedText()) {
                QString selText = editor->selectedText().trimmed();
                if (!selText.contains('\n')) {
                    m_findAndReplace->setSearchText(selText);
                }
            }
        }
        m_findAndReplace->focusOnSearch();
    }
}
