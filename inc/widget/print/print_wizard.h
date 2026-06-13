#ifndef PRINT_WIZARD_H
#define PRINT_WIZARD_H

#include "main.h"
#include <QWizard>
#include <QPrinter>
#include <Qsci/qsciprinter.h>
#include <Qsci/qsciscintilla.h>

class CodeEditor;
class PrintWizardPagePrinter;
class PrintWizardPagePageSetup;
class PrintWizardPageOutput;
class PrintWizardPageLayout;
class PrintWizardPagePreview;

namespace Ui {
class PrintWizard;
}

class PrintWizard : public QWizard
{
    Q_OBJECT

public:
    // 打印范围
    enum PrintRange {
        AllPages = 0,        // 全部
        CurrentPage = 1,     // 当前页（光标处）
        Selection = 2,       // 选中文本
        Range = 3            // 指定范围
    };

    explicit PrintWizard(CodeEditor *editor, QWidget *parent = nullptr);
    ~PrintWizard();

    // 获取配置好的QsciPrinter
    QsciPrinter* printer() const;
    
    // 获取起始行和结束行（用于printRange）
    int fromLine() const;
    int toLine() const;
    int printRangeType() const;
    
    // 启动打印
    bool performPrint();

protected:
    void changeEvent(QEvent *e);

private slots:
    void onCurrentIdChanged(int id);
    void onPaintRequested(QPrinter *printer);

private:
    void setupPages();
    void applyPrinterSettings();
    void updatePreview();

    Ui::PrintWizard *ui;
    CodeEditor *m_editor;
    QsciPrinter *m_printer;

    PrintWizardPagePrinter *m_pagePrinter;
    PrintWizardPagePageSetup *m_pagePageSetup;
    PrintWizardPageOutput *m_pageOutput;
    PrintWizardPageLayout *m_pageLayout;
    PrintWizardPagePreview *m_pagePreview;
};

#endif // PRINT_WIZARD_H