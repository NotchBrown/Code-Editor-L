#ifndef PRINT_WIZARD_PAGE_PREVIEW_H
#define PRINT_WIZARD_PAGE_PREVIEW_H

#include "main.h"
#include <QWizardPage>
#include <QPrinter>
#include <Qsci/qsciprinter.h>

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

    // 更新预览（由wizard调用，应用所有设置）
    void updatePreview(QsciPrinter *printer);
    void setPrintRange(int rangeType, int from, int to);

    virtual bool isComplete() const;

private slots:
    void onZoomIn();
    void onZoomOut();
    void onZoomReset();
    void onPrevPage();
    void onNextPage();
    void onFirstPage();
    void onLastPage();
    void onOrientationChanged();

private:
    void setupConnections();
    void renderPreview();
    void updateNavigation();
    QImage renderPage(int pageNumber, QsciPrinter *printer);

    Ui::PrintWizardPagePreview *ui;
    CodeEditor *m_editor;
    QsciPrinter *m_printer;
    
    int m_currentPage;
    int m_totalPages;
    int m_zoomFactor;  // 百分比
    int m_rangeType;
    int m_fromLine;
    int m_toLine;
};

#endif // PRINT_WIZARD_PAGE_PREVIEW_H