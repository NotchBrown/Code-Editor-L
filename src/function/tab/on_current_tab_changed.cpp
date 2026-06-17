#include "main.h"
#include "widget/main_window/main_window.h"
#include "widget/main_window/status_bar.h"
#include "widget/editor/code_editor.h"
#include "widget/find_and_replace/find_and_replace.h"
#include "widget/navigator/navigator.h"
#include "widget/segment/segment.h"

void MainWindow::onCurrentTabChanged(int index)
{
    Q_UNUSED(index);
    updateWindowTitle();
    updateOpenTabsMenu();
    updateFileTypeMenuChecked();
    updateEncodingMenuChecked();
    updateReadOnlyMenuChecked();

    // Update FindAndReplace with current editor and all editors
    CodeEditor *editor = currentEditor();
    if (editor && m_findAndReplace) {
        m_findAndReplace->setEditor(editor);
        // Collect all editors for cross-tab search
        QList<CodeEditor*> allEditors;
        QTabWidget *tabWidget = findChild<QTabWidget*>("mainTabWidget");
        if (tabWidget) {
            for (int i = 0; i < tabWidget->count(); ++i) {
                CodeEditor *ed = qobject_cast<CodeEditor*>(tabWidget->widget(i));
                if (ed) allEditors.append(ed);
            }
        }
        m_findAndReplace->setAllEditors(allEditors);
    }

    // Update Navigator and Segment
    if (editor) {
        if (m_navigator) m_navigator->updateOutline(editor);
        if (m_segment) m_segment->updateAtPosition(editor, 0, 0);
    }

    // Update status bar for current editor
    editor = currentEditor();
    if (editor && m_statusBar) {
        m_statusBar->setFilePath(editor->filePath());
        m_statusBar->setFileType(editor->currentLexerName());
        m_statusBar->setEncoding(editor->currentEncoding());
        // Get actual cursor position (0-based from editor, convert to 1-based for display)
        int line, index;
        editor->getCursorPosition(&line, &index);
        m_statusBar->setCursorPosition(line + 1, index + 1);
        m_statusBar->setReadOnly(editor->isReadOnly());
    }
}
