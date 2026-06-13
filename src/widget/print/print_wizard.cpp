#include "widget/print/print_wizard.h"
#include "ui_print_wizard.h"
#include "widget/print/print_wizard_page_printer.h"
#include "widget/print/print_wizard_page_page_setup.h"
#include "widget/print/print_wizard_page_output.h"
#include "widget/print/print_wizard_page_layout.h"
#include "widget/print/print_wizard_page_preview.h"
#include "widget/editor/code_editor.h"

#include <QPrintPreviewWidget>
#include <QFileInfo>
#include <QDebug>

PrintWizard::PrintWizard(CodeEditor *editor, QWidget *parent)
    : QWizard(parent),
      ui(new Ui::PrintWizard),
      m_editor(editor),
      m_printer(nullptr),
      m_pagePrinter(nullptr),
      m_pagePageSetup(nullptr),
      m_pageOutput(nullptr),
      m_pageLayout(nullptr),
      m_pagePreview(nullptr)
{
    ui->setupUi(this);

    // Set wizard style to classic (no ? button)
    setWizardStyle(QWizard::ClassicStyle);
    setButtonText(QWizard::NextButton, tr("&Next >"));
    setButtonText(QWizard::BackButton, tr("< &Back"));
    setButtonText(QWizard::FinishButton, tr("&Print"));
    setButtonText(QWizard::CancelButton, tr("&Cancel"));

    setOption(QWizard::NoBackButtonOnStartPage, true);

    // Setup printer
    QPrinter::PrinterMode mode = QPrinter::HighResolution;
    m_printer = new QsciPrinter(mode);
    if (m_editor) {
        QString docName = m_editor->filePath().isEmpty()
            ? "Untitled"
            : QFileInfo(m_editor->filePath()).fileName();
        m_printer->setDocName(docName);
    }
    m_printer->setCreator("Code Editor Lite");

    // Create and add pages
    m_pagePrinter = new PrintWizardPagePrinter(this);
    m_pagePageSetup = new PrintWizardPagePageSetup(this);
    m_pageOutput = new PrintWizardPageOutput(this);
    m_pageLayout = new PrintWizardPageLayout(this);
    m_pagePreview = new PrintWizardPagePreview(m_editor, this);

    addPage(m_pagePrinter);
    addPage(m_pagePageSetup);
    addPage(m_pageOutput);
    addPage(m_pageLayout);
    addPage(m_pagePreview);

    // Set editor info on layout page
    if (m_editor) {
        int totalLines = m_editor->lines();
        int currentLine = m_editor->lines() > 0 
            ? m_editor->SendScintilla(QsciScintilla::SCI_LINEFROMPOSITION, 
                                     m_editor->SendScintilla(QsciScintilla::SCI_GETCURRENTPOS))
            : 0;
        bool hasSel = m_editor->hasSelectedText();
        int selStart = -1, selEnd = -1;
        if (hasSel) {
            int startPos = m_editor->SendScintilla(QsciScintilla::SCI_GETSELECTIONSTART);
            int endPos = m_editor->SendScintilla(QsciScintilla::SCI_GETSELECTIONEND);
            selStart = m_editor->SendScintilla(QsciScintilla::SCI_LINEFROMPOSITION, startPos);
            selEnd = m_editor->SendScintilla(QsciScintilla::SCI_LINEFROMPOSITION, endPos);
        }
        m_pageLayout->setEditorLineInfo(totalLines, currentLine, hasSel, selStart, selEnd);
    }

    // Load page setup from printer default
    m_pagePageSetup->loadFromPrinter(m_printer);

    // Setup preview connection - use direct connection to avoid issues
    QPrintPreviewWidget *pw = m_pagePreview->findChild<QPrintPreviewWidget*>("previewWidget");
    if (pw) {
        connect(pw, &QPrintPreviewWidget::paintRequested,
                this, &PrintWizard::onPaintRequested);
    }

    setWindowTitle(tr("Print Wizard - %1").arg(m_editor
        ? (m_editor->filePath().isEmpty() ? "Untitled" : QFileInfo(m_editor->filePath()).fileName())
        : ""));
    resize(820, 620);
}

PrintWizard::~PrintWizard()
{
    delete ui;
}

void PrintWizard::onPaintRequested(QPrinter *printer)
{
    if (!printer || !m_editor) return;
    
    // Save current printer settings
    QPrinter::OutputFormat originalFormat = m_printer->outputFormat();
    QString originalOutputFile = m_printer->outputFileName();
    
    try {
        // Apply settings from all pages to our printer
        applyPrinterSettings();
        
        // Apply same settings to the preview printer
        printer->setPrinterName(m_printer->printerName());
        printer->setColorMode(m_printer->colorMode());
        printer->setPageLayout(m_printer->pageLayout());
        printer->setFullPage(m_printer->fullPage());
        printer->setCopyCount(m_printer->copyCount());
        printer->setCollateCopies(m_printer->collateCopies());
        printer->setDuplex(m_printer->duplex());
        printer->setPageOrder(m_printer->pageOrder());
        
        // Create a temporary QsciPrinter for preview
        QsciPrinter previewPrinter(QPrinter::HighResolution);
        // Set the same settings on preview printer
        previewPrinter.setPrinterName(printer->printerName());
        previewPrinter.setColorMode(printer->colorMode());
        previewPrinter.setPageLayout(printer->pageLayout());
        previewPrinter.setFullPage(printer->fullPage());
        
        // Apply layout settings
        previewPrinter.setMagnification(m_pageLayout->magnification());
        previewPrinter.setWrapMode(m_pageLayout->wrapMode());
        
        // Handle print range
        int from = -1;
        int to = -1;
        int rangeType = m_pageLayout->printRangeType();
        
        if (rangeType == CurrentPage) {
            from = m_pageLayout->fromLine();
            to = m_pageLayout->toLine();
        } else if (rangeType == Selection) {
            from = m_pageLayout->fromLine();
            to = m_pageLayout->toLine();
        } else if (rangeType == Range) {
            from = m_pageLayout->fromLine();
            to = m_pageLayout->toLine();
        }
        
        // Print to the preview printer
        previewPrinter.printRange(m_editor, from, to);
    } catch (...) {
        // Ignore errors during preview
    }
    
    // Restore original settings
    m_printer->setOutputFormat(originalFormat);
    m_printer->setOutputFileName(originalOutputFile);
}

void PrintWizard::changeEvent(QEvent *e)
{
    QWizard::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

void PrintWizard::onCurrentIdChanged(int id)
{
    // When entering preview page, apply settings and update preview
    if (id >= 0) {
        QWizardPage *page = this->page(id);
        if (page == m_pagePreview) {
            applyPrinterSettings();
            QPrintPreviewWidget *pw = m_pagePreview->findChild<QPrintPreviewWidget*>("previewWidget");
            if (pw) {
                pw->updatePreview();
            }
        }
    }
}

QsciPrinter* PrintWizard::printer() const
{
    return m_printer;
}

int PrintWizard::fromLine() const
{
    if (!m_pageLayout) return -1;
    int type = m_pageLayout->printRangeType();
    if (type == AllPages) return -1;
    return m_pageLayout->fromLine();
}

int PrintWizard::toLine() const
{
    if (!m_pageLayout) return -1;
    int type = m_pageLayout->printRangeType();
    if (type == AllPages) return -1;
    return m_pageLayout->toLine();
}

int PrintWizard::printRangeType() const
{
    if (!m_pageLayout) return 0;
    return m_pageLayout->printRangeType();
}

bool PrintWizard::performPrint()
{
    if (!m_editor || !m_printer) return false;

    applyPrinterSettings();

    int from = fromLine();
    int to = toLine();

    return m_printer->printRange(m_editor, from, to) != 0;
}

void PrintWizard::applyPrinterSettings()
{
    if (!m_printer) return;

    // Printer settings
    m_printer->setPrinterName(m_pagePrinter->printerName());
    m_printer->setColorMode(m_pagePrinter->colorMode());
    m_printer->setFontEmbeddingEnabled(m_pagePrinter->isFontEmbeddingEnabled());

    // Page setup
    QPageSize ps(m_pagePageSetup->paperSize());
    QPageLayout pl(ps, m_pagePageSetup->orientation(),
                   m_pagePageSetup->customMargins(), QPageLayout::Millimeter);
    m_printer->setPageLayout(pl);
    m_printer->setFullPage(m_pagePageSetup->fullPage());

    // Output settings
    m_printer->setOutputFormat(m_pageOutput->outputFormat());
    if (m_pageOutput->outputFormat() == QPrinter::PdfFormat) {
        m_printer->setOutputFileName(m_pageOutput->outputFileName());
    } else {
        m_printer->setOutputFileName(QString());
    }
    m_printer->setCopyCount(m_pageOutput->copyCount());
    m_printer->setCollateCopies(m_pageOutput->collateCopies());
    m_printer->setDuplex(m_pageOutput->duplexMode());
    m_printer->setPageOrder(QPrinter::PageOrder(m_pageOutput->pageOrder()));
}
