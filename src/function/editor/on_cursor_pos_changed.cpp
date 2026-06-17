#include "main.h"
#include "widget/main_window/main_window.h"
#include "widget/main_window/status_bar.h"
#include "widget/editor/code_editor.h"
#include "widget/segment/segment.h"
#include <QDebug>

void MainWindow::onCursorPosChanged(int line, int index)
{
    qDebug() << "onCursorPosChanged, line:" << line << "index:" << index;
    CodeEditor *editor = qobject_cast<CodeEditor*>(sender());
    if (!editor || !m_statusBar) return;

    CodeEditor *current = currentEditor();
    qDebug() << "sender editor:" << editor << "current editor:" << current;
    if (editor == current) {
        // QScintilla uses 0-based indexing, convert to 1-based for display
        m_statusBar->setCursorPosition(line + 1, index + 1);
        // Update Segment with new cursor position
        if (m_segment) m_segment->updateAtPosition(editor, line, index);
    }
}