#include "widget/print/print_wizard_page_printer.h"
#include "ui_print_wizard_page_printer.h"

#include <QPrinterInfo>

PrintWizardPagePrinter::PrintWizardPagePrinter(QWidget *parent)
    : QWizardPage(parent),
      ui(new Ui::PrintWizardPagePrinter)
{
    ui->setupUi(this);

    populatePrinters();

    // Default settings
    m_defaultPrinter = QPrinterInfo::defaultPrinter().printerName();
    if (m_defaultPrinter.isEmpty() && !QPrinterInfo::availablePrinters().isEmpty()) {
        m_defaultPrinter = QPrinterInfo::availablePrinters().first().printerName();
    }
    if (!m_defaultPrinter.isEmpty()) {
        int index = ui->comboPrinterName->findText(m_defaultPrinter);
        if (index >= 0) {
            ui->comboPrinterName->setCurrentIndex(index);
        }
    }
    ui->comboPrinterMode->setCurrentIndex(2); // High Resolution
    ui->comboColorMode->setCurrentIndex(0);    // Color

    connect(ui->comboPrinterName, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onPrinterSelectionChanged()));

    updatePrinterInfo();

    registerField("printerName*", ui->comboPrinterName, "currentText");
    registerField("printerMode", ui->comboPrinterMode, "currentIndex");
    registerField("colorMode", ui->comboColorMode, "currentIndex");
    registerField("fontEmbedding", ui->checkFontEmbedding);
}

PrintWizardPagePrinter::~PrintWizardPagePrinter()
{
    delete ui;
}

void PrintWizardPagePrinter::populatePrinters()
{
    ui->comboPrinterName->clear();
    QList<QPrinterInfo> printers = QPrinterInfo::availablePrinters();
    foreach (const QPrinterInfo &info, printers) {
        ui->comboPrinterName->addItem(info.printerName());
    }
    // Add PDF option if no printers available
    if (ui->comboPrinterName->count() == 0) {
        ui->comboPrinterName->addItem("Microsoft Print to PDF");
        ui->comboPrinterName->addItem("Save as PDF");
    }
}

void PrintWizardPagePrinter::onPrinterSelectionChanged()
{
    updatePrinterInfo();
}

void PrintWizardPagePrinter::updatePrinterInfo()
{
    QString name = ui->comboPrinterName->currentText();
    QPrinterInfo info = QPrinterInfo::printerInfo(name);
    if (info.isNull()) {
        ui->labelLocation->setText("(N/A)");
        ui->labelMakeModel->setText("(N/A)");
        ui->labelState->setText("(N/A)");
        ui->labelDefault->setText(name == m_defaultPrinter ? "Yes" : "No");
        return;
    }
    ui->labelLocation->setText(info.location().isEmpty() ? "-" : info.location());
    ui->labelMakeModel->setText(info.makeAndModel().isEmpty() ? "-" : info.makeAndModel());
    QString state;
    switch (info.state()) {
        case QPrinter::Idle:    state = "Idle"; break;
        case QPrinter::Active:  state = "Active"; break;
        case QPrinter::Aborted: state = "Aborted"; break;
        case QPrinter::Error:   state = "Error"; break;
        default:                state = "-";
    }
    ui->labelState->setText(state);
    ui->labelDefault->setText(info.isDefault() ? "Yes" : "No");
}

QString PrintWizardPagePrinter::printerName() const
{
    return ui->comboPrinterName->currentText();
}

QPrinter::PrinterMode PrintWizardPagePrinter::printerMode() const
{
    switch (ui->comboPrinterMode->currentIndex()) {
        case 0: return QPrinter::ScreenResolution;
        case 1: return QPrinter::PrinterResolution;
        case 2: return QPrinter::HighResolution;
        default: return QPrinter::ScreenResolution;
    }
}

QPrinter::ColorMode PrintWizardPagePrinter::colorMode() const
{
    return ui->comboColorMode->currentIndex() == 0
        ? QPrinter::Color : QPrinter::GrayScale;
}

bool PrintWizardPagePrinter::isFontEmbeddingEnabled() const
{
    return ui->checkFontEmbedding->isChecked();
}

void PrintWizardPagePrinter::setPrinterMode(QPrinter::PrinterMode mode)
{
    int index = 0;
    switch (mode) {
        case QPrinter::ScreenResolution: index = 0; break;
        case QPrinter::PrinterResolution: index = 1; break;
        case QPrinter::HighResolution: index = 2; break;
        default: index = 0;
    }
    ui->comboPrinterMode->setCurrentIndex(index);
}

void PrintWizardPagePrinter::setColorMode(QPrinter::ColorMode mode)
{
    ui->comboColorMode->setCurrentIndex(mode == QPrinter::Color ? 0 : 1);
}

void PrintWizardPagePrinter::refreshPrinterInfo()
{
    updatePrinterInfo();
}

bool PrintWizardPagePrinter::isComplete() const
{
    return !ui->comboPrinterName->currentText().isEmpty();
}
