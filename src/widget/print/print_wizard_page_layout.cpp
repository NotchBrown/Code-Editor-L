#include "widget/print/print_wizard_page_layout.h"
#include "ui_print_wizard_page_layout.h"

#include <Qsci/qsciscintilla.h>

PrintWizardPageLayout::PrintWizardPageLayout(QWidget *parent)
    : QWizardPage(parent),
      ui(new Ui::PrintWizardPageLayout),
      m_totalLines(0),
      m_currentLine(0),
      m_selStart(-1),
      m_selEnd(-1)
{
    ui->setupUi(this);

    // Default values
    ui->comboWrapMode->setCurrentIndex(1); // Word
    ui->spinFrom->setValue(1);
    ui->spinTo->setValue(1);
    ui->spinFrom->setEnabled(false);
    ui->spinTo->setEnabled(false);

    connect(ui->spinMagnification, SIGNAL(valueChanged(int)),
            this, SLOT(onMagnificationChanged(int)));
    connect(ui->comboRangeType, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onRangeTypeChanged(int)));

    registerField("magnification", ui->spinMagnification, "value");
    registerField("wrapMode", ui->comboWrapMode, "currentIndex");
    registerField("lineNumbers", ui->checkLineNumbers);
    registerField("background", ui->checkBackground);
    registerField("headerLeft", ui->editHeaderLeft, "text");
    registerField("headerCenter", ui->editHeaderCenter, "text");
    registerField("headerRight", ui->editHeaderRight, "text");
    registerField("footerLeft", ui->editFooterLeft, "text");
    registerField("footerCenter", ui->editFooterCenter, "text");
    registerField("footerRight", ui->editFooterRight, "text");
    registerField("fromLine", ui->spinFrom, "value");
    registerField("toLine", ui->spinTo, "value");
    registerField("rangeType", ui->comboRangeType, "currentIndex");
}

PrintWizardPageLayout::~PrintWizardPageLayout()
{
    delete ui;
}

void PrintWizardPageLayout::onMagnificationChanged(int value)
{
    Q_UNUSED(value);
    emit completeChanged();
}

void PrintWizardPageLayout::onRangeTypeChanged(int index)
{
    updateRangeControls();
    emit completeChanged();
}

void PrintWizardPageLayout::updateRangeControls()
{
    int idx = ui->comboRangeType->currentIndex();
    bool enableRange = (idx == 3); // Range (From-To)
    bool enableFromTo = (idx == 3);

    ui->spinFrom->setEnabled(enableFromTo);
    ui->spinTo->setEnabled(enableFromTo);

    if (idx == 1) { // Current Page
        ui->spinFrom->setValue(m_currentLine + 1);
        ui->spinTo->setValue(m_currentLine + 1);
    } else if (idx == 2 && m_selStart >= 0) { // Selection
        ui->spinFrom->setValue(m_selStart + 1);
        ui->spinTo->setValue(m_selEnd + 1);
    } else if (idx == 0) { // All
        ui->spinFrom->setValue(1);
        ui->spinTo->setValue(m_totalLines);
    }
    Q_UNUSED(enableRange);
}

int PrintWizardPageLayout::magnification() const
{
    return ui->spinMagnification->value();
}

QsciScintilla::WrapMode PrintWizardPageLayout::wrapMode() const
{
    switch (ui->comboWrapMode->currentIndex()) {
        case 0: return QsciScintilla::WrapNone;
        case 1: return QsciScintilla::WrapWord;
        case 2: return QsciScintilla::WrapCharacter;
        case 3: return QsciScintilla::WrapWhitespace;
        default: return QsciScintilla::WrapWord;
    }
}

bool PrintWizardPageLayout::printLineNumbers() const
{
    return ui->checkLineNumbers->isChecked();
}

bool PrintWizardPageLayout::printBackground() const
{
    return ui->checkBackground->isChecked();
}

QString PrintWizardPageLayout::headerLeft() const { return ui->editHeaderLeft->text(); }
QString PrintWizardPageLayout::headerCenter() const { return ui->editHeaderCenter->text(); }
QString PrintWizardPageLayout::headerRight() const { return ui->editHeaderRight->text(); }
QString PrintWizardPageLayout::footerLeft() const { return ui->editFooterLeft->text(); }
QString PrintWizardPageLayout::footerCenter() const { return ui->editFooterCenter->text(); }
QString PrintWizardPageLayout::footerRight() const { return ui->editFooterRight->text(); }

int PrintWizardPageLayout::fromLine() const { return ui->spinFrom->value() - 1; }
int PrintWizardPageLayout::toLine() const { return ui->spinTo->value() - 1; }
int PrintWizardPageLayout::printRangeType() const { return ui->comboRangeType->currentIndex(); }

void PrintWizardPageLayout::setMagnification(int mag)
{
    ui->spinMagnification->setValue(mag);
}

void PrintWizardPageLayout::setWrapMode(QsciScintilla::WrapMode mode)
{
    int idx = 1;
    switch (mode) {
        case QsciScintilla::WrapNone:       idx = 0; break;
        case QsciScintilla::WrapWord:       idx = 1; break;
        case QsciScintilla::WrapCharacter:  idx = 2; break;
        case QsciScintilla::WrapWhitespace: idx = 3; break;
    }
    ui->comboWrapMode->setCurrentIndex(idx);
}

void PrintWizardPageLayout::setLineNumbersEnabled(bool enabled)
{
    ui->checkLineNumbers->setChecked(enabled);
}

void PrintWizardPageLayout::setBackgroundEnabled(bool enabled)
{
    ui->checkBackground->setChecked(enabled);
}

void PrintWizardPageLayout::setHeader(const QString &left, const QString &center, const QString &right)
{
    ui->editHeaderLeft->setText(left);
    ui->editHeaderCenter->setText(center);
    ui->editHeaderRight->setText(right);
}

void PrintWizardPageLayout::setFooter(const QString &left, const QString &center, const QString &right)
{
    ui->editFooterLeft->setText(left);
    ui->editFooterCenter->setText(center);
    ui->editFooterRight->setText(right);
}

void PrintWizardPageLayout::setRange(int from, int to, int rangeType)
{
    ui->comboRangeType->setCurrentIndex(rangeType);
    ui->spinFrom->setValue(from + 1);
    ui->spinTo->setValue(to + 1);
    updateRangeControls();
}

void PrintWizardPageLayout::setEditorLineInfo(int totalLines, int currentLine, bool hasSelection,
                                              int selStart, int selEnd)
{
    m_totalLines = totalLines;
    m_currentLine = currentLine;
    m_selStart = selStart;
    m_selEnd = selEnd;

    ui->spinFrom->setMaximum(totalLines > 0 ? totalLines : 1);
    ui->spinTo->setMaximum(totalLines > 0 ? totalLines : 1);

    QString info = QString("Total lines: %1, current line: %2")
        .arg(totalLines)
        .arg(currentLine + 1);
    if (hasSelection) {
        info += QString(", selection: %1-%2").arg(selStart + 1).arg(selEnd + 1);
    }
    ui->labelLineInfo->setText(info);

    // Update range
    ui->spinFrom->setValue(1);
    ui->spinTo->setValue(totalLines > 0 ? totalLines : 1);
}

bool PrintWizardPageLayout::isComplete() const
{
    if (ui->comboRangeType->currentIndex() == 3) {
        if (ui->spinFrom->value() > ui->spinTo->value()) {
            return false;
        }
        if (ui->spinFrom->value() < 1) return false;
        if (ui->spinTo->value() > m_totalLines) return false;
    }
    return true;
}