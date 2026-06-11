#ifndef PRINT_WIZARD_PAGE_PAGE_SETUP_H
#define PRINT_WIZARD_PAGE_PAGE_SETUP_H

#include "main.h"
#include <QWizardPage>
#include <QPrinter>

namespace Ui {
class PrintWizardPagePageSetup;
}

class PrintWizardPagePageSetup : public QWizardPage
{
    Q_OBJECT

public:
    explicit PrintWizardPagePageSetup(QWidget *parent = nullptr);
    ~PrintWizardPagePageSetup();

    // Getter
    QPageSize::PageSizeId paperSize() const;
    QPageLayout::Orientation orientation() const;
    qreal marginLeft() const;
    qreal marginRight() const;
    qreal marginTop() const;
    qreal marginBottom() const;
    bool fullPage() const;
    QMarginsF customMargins() const;

    // Setter
    void setPaperSize(QPageSize::PageSizeId size);
    void setOrientation(QPageLayout::Orientation orient);
    void setMargins(qreal left, qreal right, qreal top, qreal bottom);
    void setFullPage(bool full);
    void loadFromPrinter(const QPrinter *printer);

    virtual bool isComplete() const;

private slots:
    void onPaperSizeChanged(int index);
    void onOrientationChanged();
    void onPresetChanged(int index);
    void onUnitChanged(int index);
    void updatePagePreview();

private:
    void populatePaperSizes();
    void populateUnits();
    void updatePresetVisibility();
    void setupPagePreview();
    QSizeF paperSizeToQSize(QPageSize::PageSizeId id) const;

    Ui::PrintWizardPagePageSetup *ui;

    // 当前页面的像素尺寸（用于预览）
    QSize m_currentPaperSize;
    QPageLayout::Orientation m_currentOrientation;
};

#endif // PRINT_WIZARD_PAGE_PAGE_SETUP_H