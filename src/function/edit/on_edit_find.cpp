#include "main.h"
#include "widget/main_window/main_window.h"
#include "widget/editor/code_editor.h"
#include "ui_main_window.h"
#include "widget/find_and_replace/find_and_replace.h"

void MainWindow::onEditFind()
{
    // Show the Find and Replace dock in Find mode (replace disabled)
    QDockWidget *dock = findChild<QDockWidget*>("dockFindReplace");
    if (dock) {
        dock->setVisible(true);
        dock->raise();
        m_findAndReplace->activateFindMode();

        // Pass current editor selection as initial search text
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
