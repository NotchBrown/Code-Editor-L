#include "widget/print/print_wizard_page_output.h"
#include "ui_print_wizard_page_output.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QStandardPaths>

PrintWizardPageOutput::PrintWizardPageOutput(QWidget *parent)
    : QWizardPage(parent),
      ui(new Ui::PrintWizardPageOutput)
{
    ui->setupUi(this);

    m_lastDirectory = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

    updateFileOutputEnabled();

    connect(ui->comboOutputFormat, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onOutputFormatChanged()));
    connect(ui->btnBrowse, SIGNAL(clicked()),
            this, SLOT(onBrowseFile()));

    registerField("outputFormat", ui->comboOutputFormat, "currentIndex");
    registerField("outputFile*", ui->editOutputFile, "text");
    registerField("copyCount*", ui->spinCopies, "value");
    registerField("collate", ui->checkCollate);
    registerField("duplexMode", ui->comboDuplex, "currentIndex");
    registerField("pageOrder", ui->comboPageOrder, "currentIndex");
}

PrintWizardPageOutput::~PrintWizardPageOutput()
{
    delete ui;
}

void PrintWizardPageOutput::onOutputFormatChanged()
{
    updateFileOutputEnabled();
    updateLineEditFormat();
    emit completeChanged();
}

void PrintWizardPageOutput::updateFileOutputEnabled()
{
    bool isFile = ui->comboOutputFormat->currentIndex() == 1; // PDF
    ui->editOutputFile->setEnabled(isFile);
    ui->btnBrowse->setEnabled(isFile);
}

void PrintWizardPageOutput::updateLineEditFormat()
{
    if (ui->comboOutputFormat->currentIndex() == 1) {
        QString current = ui->editOutputFile->text();
        if (current.isEmpty()) {
            ui->editOutputFile->setText(m_lastDirectory + "/document.pdf");
        } else if (!current.endsWith(".pdf", Qt::CaseInsensitive)) {
            QFileInfo fi(current);
            ui->editOutputFile->setText(fi.absolutePath() + "/" + fi.completeBaseName() + ".pdf");
        }
    }
}

void PrintWizardPageOutput::onBrowseFile()
{
    QString filter;
    if (ui->comboOutputFormat->currentIndex() == 1) {
        filter = "PDF Document (*.pdf)";
    } else {
        filter = "All Files (*)";
    }
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                                                    m_lastDirectory, filter);
    if (!fileName.isEmpty()) {
        m_lastDirectory = QFileInfo(fileName).absolutePath();
        ui->editOutputFile->setText(fileName);
    }
}

void PrintWizardPageOutput::onPrintRangeChanged()
{
    // No-op - kept for future use
}

QPrinter::OutputFormat PrintWizardPageOutput::outputFormat() const
{
    return ui->comboOutputFormat->currentIndex() == 1
        ? QPrinter::PdfFormat : QPrinter::NativeFormat;
}

QString PrintWizardPageOutput::outputFileName() const
{
    return ui->editOutputFile->text();
}

int PrintWizardPageOutput::copyCount() const
{
    return ui->spinCopies->value();
}

bool PrintWizardPageOutput::collateCopies() const
{
    return ui->checkCollate->isChecked();
}

QPrinter::DuplexMode PrintWizardPageOutput::duplexMode() const
{
    switch (ui->comboDuplex->currentIndex()) {
        case 0:  return QPrinter::DuplexNone;
        case 1:  return QPrinter::DuplexAuto;
        case 2:  return QPrinter::DuplexLongSide;
        case 3:  return QPrinter::DuplexShortSide;
        default: return QPrinter::DuplexNone;
    }
}

bool PrintWizardPageOutput::printInColor() const
{
    return true; // Color is set on printer page
}

int PrintWizardPageOutput::pageOrder() const
{
    return ui->comboPageOrder->currentIndex();
}

void PrintWizardPageOutput::setOutputFormat(QPrinter::OutputFormat format)
{
    ui->comboOutputFormat->setCurrentIndex(format == QPrinter::PdfFormat ? 1 : 0);
    updateFileOutputEnabled();
}

void PrintWizardPageOutput::setOutputFileName(const QString &name)
{
    ui->editOutputFile->setText(name);
}

void PrintWizardPageOutput::setCopyCount(int count)
{
    ui->spinCopies->setValue(count);
}

void PrintWizardPageOutput::setCollateCopies(bool collate)
{
    ui->checkCollate->setChecked(collate);
}

void PrintWizardPageOutput::setDuplexMode(QPrinter::DuplexMode mode)
{
    int idx = 0;
    switch (mode) {
        case QPrinter::DuplexNone:      idx = 0; break;
        case QPrinter::DuplexAuto:      idx = 1; break;
        case QPrinter::DuplexLongSide:  idx = 2; break;
        case QPrinter::DuplexShortSide: idx = 3; break;
    }
    ui->comboDuplex->setCurrentIndex(idx);
}

bool PrintWizardPageOutput::isComplete() const
{
    if (ui->comboOutputFormat->currentIndex() == 1) {
        if (ui->editOutputFile->text().isEmpty()) {
            return false;
        }
    }
    return true;
}