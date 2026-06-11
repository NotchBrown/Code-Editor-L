#ifndef PRINT_WIZARD_PAGE_LAYOUT_H
#define PRINT_WIZARD_PAGE_LAYOUT_H

#include "main.h"
#include <QWizardPage>
#include <Qsci/qsciscintilla.h>

namespace Ui {
class PrintWizardPageLayout;
}

class PrintWizardPageLayout : public QWizardPage
{
    Q_OBJECT

public:
    explicit PrintWizardPageLayout(QWidget *parent = nullptr);
    ~PrintWizardPageLayout();

    // Getter
    int magnification() const;
    QsciScintilla::WrapMode wrapMode() const;
    bool printLineNumbers() const;
    bool printBackground() const;
    QString headerLeft() const;
    QString headerCenter() const;
    QString headerRight() const;
    QString footerLeft() const;
    QString footerCenter() const;
    QString footerRight() const;
    int fromLine() const;
    int toLine() const;
    int printRangeType() const;  // 0: All, 1: Current, 2: Selection, 3: Range

    // Setter
    void setMagnification(int mag);
    void setWrapMode(QsciScintilla::WrapMode mode);
    void setLineNumbersEnabled(bool enabled);
    void setBackgroundEnabled(bool enabled);
    void setHeader(const QString &left, const QString &center, const QString &right);
    void setFooter(const QString &left, const QString &center, const QString &right);
    void setRange(int from, int to, int rangeType);
    void setEditorLineInfo(int totalLines, int currentLine, bool hasSelection,
                           int selStart, int selEnd);

    virtual bool isComplete() const;

private slots:
    void onMagnificationChanged(int value);
    void onRangeTypeChanged(int index);

private:
    void updateRangeControls();

    Ui::PrintWizardPageLayout *ui;
    int m_totalLines;
    int m_currentLine;
    int m_selStart;
    int m_selEnd;
};

#endif // PRINT_WIZARD_PAGE_LAYOUT_H