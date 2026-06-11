#ifndef PRINT_WIZARD_PAGE_PRINTER_H
#define PRINT_WIZARD_PAGE_PRINTER_H

#include "main.h"
#include <QWizardPage>
#include <QPrinter>

namespace Ui {
class PrintWizardPagePrinter;
}

class PrintWizardPagePrinter : public QWizardPage
{
    Q_OBJECT

public:
    explicit PrintWizardPagePrinter(QWidget *parent = nullptr);
    ~PrintWizardPagePrinter();

    // Getter
    QString printerName() const;
    QPrinter::PrinterMode printerMode() const;
    QPrinter::ColorMode colorMode() const;
    bool isFontEmbeddingEnabled() const;

    // Setter
    void setPrinterName(const QString &name);
    void setPrinterMode(QPrinter::PrinterMode mode);
    void setColorMode(QPrinter::ColorMode mode);
    void setFontEmbeddingEnabled(bool enabled);

    virtual bool isComplete() const;

private slots:
    void onPrinterSelectionChanged();
    void refreshPrinterInfo();

private:
    void populatePrinters();
    void updatePrinterInfo();

    Ui::PrintWizardPagePrinter *ui;
    QString m_defaultPrinter;
};

#endif // PRINT_WIZARD_PAGE_PRINTER_H