#include "main.h"
#include "widget/main_window/main_window.h"
#include "widget/main_window/status_bar.h"
#include <QDebug>

void MainWindow::onEditorCursorPositionChanged(CodeEditor *editor, int line, int column)
{
    qDebug() << "onEditorCursorPositionChanged called, editor:" << editor << "line:" << line << "column:" << column;
    // Check if this is the current editor
    CodeEditor *current = currentEditor();
    qDebug() << "Current editor:" << current;
    if (editor == current && m_statusBar) {
        // Values are already 1-based (converted in main_window.cpp lambda)
        m_statusBar->setCursorPosition(line, column);
    }
}
