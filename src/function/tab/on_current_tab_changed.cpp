#include "main.h"
#include "widget/main_window/main_window.h"
#include "widget/main_window/status_bar.h"
#include "widget/editor/code_editor.h"

void MainWindow::onCurrentTabChanged(int index)
{
    Q_UNUSED(index);
    updateWindowTitle();
    updateOpenTabsMenu();
    updateFileTypeMenuChecked();
    updateEncodingMenuChecked();
    updateReadOnlyMenuChecked();
    
    // Update status bar for current editor
    CodeEditor *editor = currentEditor();
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
