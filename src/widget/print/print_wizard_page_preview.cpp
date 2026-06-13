#include "widget/print/print_wizard_page_preview.h"
#include "ui_print_wizard_page_preview.h"
#include "widget/editor/code_editor.h"

#include <QPrintPreviewWidget>

PrintWizardPagePreview::PrintWizardPagePreview(CodeEditor *editor, QWidget *parent)
    : QWizardPage(parent),
      ui(new Ui::PrintWizardPagePreview),
      m_editor(editor)
{
    ui->setupUi(this);
    setFinalPage(true);
}

PrintWizardPagePreview::~PrintWizardPagePreview()
{
    delete ui;
}

void PrintWizardPagePreview::setPrintRange(int rangeType, int from, int to)
{
    // Store for reference, actual handling is in PrintWizard
    Q_UNUSED(rangeType);
    Q_UNUSED(from);
    Q_UNUSED(to);
}

bool PrintWizardPagePreview::isComplete() const
{
    return true;
}
