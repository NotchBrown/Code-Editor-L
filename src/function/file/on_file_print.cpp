#include "main.h"
#include "widget/main_window/main_window.h"
#include "widget/editor/code_editor.h"
#include "widget/print/print_wizard.h"
#include <QMessageBox>
#include <QPrinterInfo>
#include <QPrintPreviewDialog>

void MainWindow::onFilePrint()
{
    CodeEditor *editor = currentEditor();
    if (!editor) {
        QMessageBox::warning(this, "Print", "No active editor to print.");
        return;
    }

    PrintWizard wizard(editor, this);
    if (wizard.exec() == QWizard::Accepted) {
        wizard.performPrint();
    }
}

void MainWindow::onFilePrintPreview()
{
    CodeEditor *editor = currentEditor();
    if (!editor) {
        QMessageBox::warning(this, "Print Preview", "No active editor to preview.");
        return;
    }

    PrintWizard wizard(editor, this);
    if (wizard.exec() == QWizard::Accepted) {
        // Preview already shown in wizard's last page
        // Just call performPrint if user wants to print
    }
}