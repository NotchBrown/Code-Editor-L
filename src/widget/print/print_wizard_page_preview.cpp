#include "widget/print/print_wizard_page_preview.h"
#include "ui_print_wizard_page_preview.h"
#include "widget/editor/code_editor.h"

#include <QPrintPreviewWidget>
#include <QPainter>

PrintWizardPagePreview::PrintWizardPagePreview(CodeEditor *editor, QWidget *parent)
    : QWizardPage(parent),
      ui(new Ui::PrintWizardPagePreview),
      m_editor(editor),
      m_printer(nullptr),
      m_currentPage(1),
      m_totalPages(1),
      m_zoomFactor(100),
      m_rangeType(0),
      m_fromLine(-1),
      m_toLine(-1)
{
    ui->setupUi(this);
}

PrintWizardPagePreview::~PrintWizardPagePreview()
{
    delete ui;
}

void PrintWizardPagePreview::updatePreview(QsciPrinter *printer)
{
    m_printer = printer;
    if (m_printer && ui->previewWidget) {
        ui->previewWidget->updatePreview();
    }
}

void PrintWizardPagePreview::setPrintRange(int rangeType, int from, int to)
{
    m_rangeType = rangeType;
    m_fromLine = from;
    m_toLine = to;
}

void PrintWizardPagePreview::onZoomIn() { /* handled by QPrintPreviewWidget */ }
void PrintWizardPagePreview::onZoomOut() { /* handled by QPrintPreviewWidget */ }
void PrintWizardPagePreview::onZoomReset() { /* handled by QPrintPreviewWidget */ }
void PrintWizardPagePreview::onPrevPage() { /* handled by QPrintPreviewWidget */ }
void PrintWizardPagePreview::onNextPage() { /* handled by QPrintPreviewWidget */ }
void PrintWizardPagePreview::onFirstPage() { /* handled by QPrintPreviewWidget */ }
void PrintWizardPagePreview::onLastPage() { /* handled by QPrintPreviewWidget */ }
void PrintWizardPagePreview::onOrientationChanged() { /* handled by QPrintPreviewWidget */ }

void PrintWizardPagePreview::setupConnections() { /* not needed */ }
void PrintWizardPagePreview::renderPreview() { /* not needed */ }
void PrintWizardPagePreview::updateNavigation() { /* not needed */ }
QImage PrintWizardPagePreview::renderPage(int, QsciPrinter *) { return QImage(); }

bool PrintWizardPagePreview::isComplete() const
{
    return true;
}