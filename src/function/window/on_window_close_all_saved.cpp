#include "main.h"
#include "widget/main_window/main_window.h"
#include "widget/editor/code_editor.h"

void MainWindow::onWindowCloseAllSaved()
{
    QTabWidget *tabWidget = findChild<QTabWidget*>("mainTabWidget");
    for (int i = tabWidget->count() - 1; i >= 0; --i) {
        CodeEditor *editor = qobject_cast<CodeEditor*>(tabWidget->widget(i));
        if (editor && !editor->isModified()) {
            closeTab(i);
        }
    }
}
