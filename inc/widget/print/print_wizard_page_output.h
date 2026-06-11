#ifndef PRINT_WIZARD_PAGE_OUTPUT_H
#define PRINT_WIZARD_PAGE_OUTPUT_H

#include "main.h"
#include <QWizardPage>
#include <QPrinter>

namespace Ui {
class PrintWizardPageOutput;
}

class PrintWizardPageOutput : public QWizardPage
{
    Q_OBJECT

public:
    explicit PrintWizardPageOutput(QWidget *parent = nullptr);
    ~PrintWizardPageOutput();

    // Getter
    QPrinter::OutputFormat outputFormat() const;
    QString outputFileName() const;
    int copyCount() const;
    bool collateCopies() const;
    QPrinter::DuplexMode duplexMode() const;
    bool printInColor() const;
    int pageOrder() const;  // 0: FirstPageFirst, 1: LastPageFirst

    // Setter
    void setOutputFormat(QPrinter::OutputFormat format);
    void setOutputFileName(const QString &name);
    void setCopyCount(int count);
    void setCollateCopies(bool collate);
    void setDuplexMode(QPrinter::DuplexMode mode);

    virtual bool isComplete() const;

private slots:
    void onOutputFormatChanged();
    void onBrowseFile();
    void onPrintRangeChanged();

private:
    void updateFileOutputEnabled();
    void updateLineEditFormat();

    Ui::PrintWizardPageOutput *ui;
    QString m_lastDirectory;
};

#endif // PRINT_WIZARD_PAGE_OUTPUT_H