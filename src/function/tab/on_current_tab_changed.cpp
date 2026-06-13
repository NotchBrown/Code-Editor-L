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
    
    // Update status bar
    CodeEditor *editor = currentEditor();
    if (editor && m_statusBar) {
        m_statusBar->setFilePath(editor->filePath());
        m_statusBar->setFileType(editor->currentLexerName());
        m_statusBar->setEncoding(editor->currentEncoding());
        m_statusBar->setCursorPosition(editor->currentLine(), editor->currentColumn());
    }
}
