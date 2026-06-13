#ifndef PRINT_WIZARD_PAGE_PREVIEW_H
#define PRINT_WIZARD_PAGE_PREVIEW_H

#include "main.h"
#include <QWizardPage>

class CodeEditor;

namespace Ui {
class PrintWizardPagePreview;
}

class PrintWizardPagePreview : public QWizardPage
{
    Q_OBJECT

public:
    explicit PrintWizardPagePreview(CodeEditor *editor, QWidget *parent = nullptr);
    ~PrintWizardPagePreview();

    void setPrintRange(int rangeType, int from, int to);
    virtual bool isComplete() const override;

private:
    Ui::PrintWizardPagePreview *ui;
    CodeEditor *m_editor;
};

#endif // PRINT_WIZARD_PAGE_PREVIEW_H
