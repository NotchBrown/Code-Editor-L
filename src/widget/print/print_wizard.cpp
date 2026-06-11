#include "widget/print/print_wizard.h"
#include "ui_print_wizard.h"
#include "widget/print/print_wizard_page_printer.h"
#include "widget/print/print_wizard_page_page_setup.h"
#include "widget/print/print_wizard_page_output.h"
#include "widget/print/print_wizard_page_layout.h"
#include "widget/print/print_wizard_page_preview.h"
#include "widget/editor/code_editor.h"

#include <QPrinterInfo>
#include <QPageSize>
#include <QPageLayout>
#include <QPrintPreviewWidget>
#include <QPainter>
#include <QFileInfo>

PrintWizard::PrintWizard(CodeEditor *editor, QWidget *parent)
    : QWizard(parent),
      ui(new Ui::PrintWizard),
      m_editor(editor),
      m_printer(nullptr)
{
    ui->setupUi(this);

    // Set wizard buttons text
    setButtonText(QWizard::NextButton, tr("&Next >"));
    setButtonText(QWizard::BackButton, tr("< &Back"));
    setButtonText(QWizard::FinishButton, tr("&Print"));
    setButtonText(QWizard::CancelButton, tr("&Cancel"));
    setButtonText(QWizard::HelpButton, tr("&Help"));

    setOption(QWizard::NoBackButtonOnStartPage, true);
    setOption(QWizard::HaveHelpButton, true);

    // Setup default printer based on editor info
    QPrinter::PrinterMode mode = QPrinter::HighResolution;
    m_printer = new QsciPrinter(mode);
    if (m_editor) {
        QString docName = m_editor->filePath().isEmpty()
            ? "Untitled"
            : QFileInfo(m_editor->filePath()).fileName();
        m_printer->setDocName(docName);
    }
    m_printer->setCreator("Code Editor by WaveIn");

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
        int currentLine = m_editor->lines() > 0 ? m_editor->SendScintilla(QsciScintilla::SCI_LINEFROMPOSITION, m_editor->SendScintilla(QsciScintilla::SCI_GETCURRENTPOS)) : 0;
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

    // Connect signals
    connect(this, &QWizard::currentIdChanged, this, &PrintWizard::onCurrentIdChanged);

    // Setup print preview connections
    if (m_pagePreview) {
        QPrintPreviewWidget *pw = m_pagePreview->findChild<QPrintPreviewWidget*>();
        if (pw) {
            connect(pw, &QPrintPreviewWidget::paintRequested,
                    this, [this](QPrinter *printer) {
                        QsciPrinter *qsp = static_cast<QsciPrinter*>(printer);
                        int from = m_pageLayout->fromLine();
                        int to = m_pageLayout->toLine();
                        if (m_pageLayout->printRangeType() == PrintWizard::AllPages) {
                            from = -1;
                            to = -1;
                        } else if (m_pageLayout->printRangeType() == PrintWizard::CurrentPage) {
                            from = m_pageLayout->fromLine();
                            to = m_pageLayout->toLine();
                        } else if (m_pageLayout->printRangeType() == PrintWizard::Selection) {
                            from = m_pageLayout->fromLine();
                            to = m_pageLayout->toLine();
                        } else if (m_pageLayout->printRangeType() == PrintWizard::Range) {
                            from = m_pageLayout->fromLine();
                            to = m_pageLayout->toLine();
                        }
                        qsp->setMagnification(m_pageLayout->magnification());
                        qsp->setWrapMode(m_pageLayout->wrapMode());
                        qsp->printRange(m_editor, from, to);
                    });
        }
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

void PrintWizard::setupPages()
{
    // Already setup in constructor
}

void PrintWizard::onCurrentIdChanged(int id)
{
    // When entering preview page, update settings
    if (id >= 0) {
        QWizardPage *page = this->page(id);
        if (page == m_pagePreview) {
            applyPrinterSettings();
            m_pagePreview->updatePreview(m_printer);
        } else if (page == m_pageLayout) {
            // Already initialized
        }
    }
}

void PrintWizard::onPrintClicked()
{
    applyPrinterSettings();
    performPrint();
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

    // Layout settings
    m_printer->setMagnification(m_pageLayout->magnification());
    m_printer->setWrapMode(m_pageLayout->wrapMode());
    // printRangeType is handled by performPrint using from/to
}

void PrintWizard::updatePreview()
{
    if (m_pagePreview) {
        m_pagePreview->updatePreview(m_printer);
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
    if (type == PrintWizard::AllPages) return -1;
    return m_pageLayout->fromLine();
}

int PrintWizard::toLine() const
{
    if (!m_pageLayout) return -1;
    int type = m_pageLayout->printRangeType();
    if (type == PrintWizard::AllPages) return -1;
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