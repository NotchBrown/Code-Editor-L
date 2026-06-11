#include "widget/print/print_wizard_page_page_setup.h"
#include "ui_print_wizard_page_page_setup.h"

#include <QPageSize>
#include <QPainter>
#include <QMarginsF>

PrintWizardPagePageSetup::PrintWizardPagePageSetup(QWidget *parent)
    : QWizardPage(parent),
      ui(new Ui::PrintWizardPagePageSetup)
{
    ui->setupUi(this);

    populatePaperSizes();
    populateUnits();

    // Default
    ui->comboPaperSize->setCurrentIndex(ui->comboPaperSize->findData(
        static_cast<int>(QPageSize::A4)));
    ui->comboUnit->setCurrentIndex(1); // Inches
    ui->comboPreset->setCurrentIndex(1); // Standard

    onPresetChanged(1);
    onOrientationChanged();

    connect(ui->comboPaperSize, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onPaperSizeChanged(int)));
    connect(ui->comboPreset, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onPresetChanged(int)));
    connect(ui->comboUnit, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUnitChanged(int)));
    connect(ui->radioPortrait, SIGNAL(toggled(bool)),
            this, SLOT(onOrientationChanged()));
    connect(ui->radioLandscape, SIGNAL(toggled(bool)),
            this, SLOT(onOrientationChanged()));

    updatePagePreview();

    registerField("paperSize", ui->comboPaperSize, "currentIndex");
    registerField("orientation", this, "orientationField");
    registerField("marginLeft*", ui->spinLeft, "value");
    registerField("marginRight*", ui->spinRight, "value");
    registerField("marginTop*", ui->spinTop, "value");
    registerField("marginBottom*", ui->spinBottom, "value");
    registerField("fullPage", ui->checkFullPage);
}

PrintWizardPagePageSetup::~PrintWizardPagePageSetup()
{
    delete ui;
}

void PrintWizardPagePageSetup::populatePaperSizes()
{
    struct PaperEntry {
        QPageSize::PageSizeId id;
        const char *name;
    };
    static const PaperEntry papers[] = {
        {QPageSize::A0,        "A0 (841 x 1189 mm)"},
        {QPageSize::A1,        "A1 (594 x 841 mm)"},
        {QPageSize::A2,        "A2 (420 x 594 mm)"},
        {QPageSize::A3,        "A3 (297 x 420 mm)"},
        {QPageSize::A4,        "A4 (210 x 297 mm)"},
        {QPageSize::A5,        "A5 (148 x 210 mm)"},
        {QPageSize::A6,        "A6 (105 x 148 mm)"},
        {QPageSize::B0,        "B0 (1000 x 1414 mm)"},
        {QPageSize::B1,        "B1 (707 x 1000 mm)"},
        {QPageSize::B2,        "B2 (500 x 707 mm)"},
        {QPageSize::B3,        "B3 (353 x 500 mm)"},
        {QPageSize::B4,        "B4 (250 x 353 mm)"},
        {QPageSize::B5,        "B5 (176 x 250 mm)"},
        {QPageSize::Letter,    "Letter (8.5 x 11 in)"},
        {QPageSize::Legal,     "Legal (8.5 x 14 in)"},
        {QPageSize::Executive, "Executive (7.25 x 10.5 in)"},
        {QPageSize::Tabloid,   "Tabloid (11 x 17 in)"},
        {QPageSize::Ledger,    "Ledger (17 x 11 in)"},
        {QPageSize::Statement, "Statement (5.5 x 8.5 in)"},
        {QPageSize::Folio,     "Folio (8.5 x 13 in)"},
        {QPageSize::C5E,       "C5E (163 x 229 mm)"},
        {QPageSize::Comm10E,   "Comm10E (105 x 241 mm)"},
        {QPageSize::DLE,       "DLE (110 x 220 mm)"},
    };
    for (size_t i = 0; i < sizeof(papers)/sizeof(papers[0]); ++i) {
        ui->comboPaperSize->addItem(QString::fromUtf8(papers[i].name),
                                    static_cast<int>(papers[i].id));
    }
}

void PrintWizardPagePageSetup::populateUnits()
{
    // Already populated in .ui
}

QSizeF PrintWizardPagePageSetup::paperSizeToQSize(QPageSize::PageSizeId id) const
{
    QPageSize ps(id);
    QSizeF s = ps.size(QPageSize::Millimeter);
    return s;
}

void PrintWizardPagePageSetup::onPaperSizeChanged(int index)
{
    Q_UNUSED(index);
    updatePagePreview();
    emit completeChanged();
}

void PrintWizardPagePageSetup::onOrientationChanged()
{
    m_currentOrientation = ui->radioLandscape->isChecked()
        ? QPageLayout::Landscape : QPageLayout::Portrait;
    updatePagePreview();
    emit completeChanged();
}

void PrintWizardPagePageSetup::onPresetChanged(int index)
{
    // Set margins in mm based on preset
    double l = 25.4, r = 25.4, t = 25.4, b = 25.4;
    switch (index) {
        case 0: // None
            l = r = t = b = 0.0;
            break;
        case 1: // Standard (1 inch)
            l = r = t = b = 25.4;
            break;
        case 2: // Narrow (0.5 inch)
            l = r = t = b = 12.7;
            break;
        case 3: // Moderate (0.75 inch)
            l = r = t = b = 19.05;
            break;
        case 4: // Wide (1.5 inch)
            l = r = t = b = 38.1;
            break;
    }
    setMargins(l, r, t, b);
    emit completeChanged();
}

void PrintWizardPagePageSetup::onUnitChanged(int index)
{
    // Convert current margin values
    double curLeft = ui->spinLeft->value();
    double curRight = ui->spinRight->value();
    double curTop = ui->spinTop->value();
    double curBottom = ui->spinBottom->value();

    // Old unit
    int oldUnit = ui->comboUnit->currentIndex();
    double factor = 1.0;
    switch (index) {
        case 0: factor = 1.0; break;     // to mm
        case 1: factor = 25.4; break;    // to in
        case 2: factor = 25.4 / 72.0; break; // to pt
        case 3: factor = 25.4 * 12.0 / 72.0; break; // to pc
    }
    double oldFactor = 1.0;
    switch (oldUnit) {
        case 0: oldFactor = 1.0; break;
        case 1: oldFactor = 25.4; break;
        case 2: oldFactor = 25.4 / 72.0; break;
        case 3: oldFactor = 25.4 * 12.0 / 72.0; break;
    }
    double toMM = oldFactor;
    double fromMM = factor;

    double l = curLeft * toMM / fromMM;
    double r = curRight * toMM / fromMM;
    double t = curTop * toMM / fromMM;
    double b = curBottom * toMM / fromMM;

    // Set decimals & suffix based on unit
    int decimals = 2;
    QString suffix;
    switch (index) {
        case 0: suffix = " mm"; decimals = 1; break;
        case 1: suffix = " in"; decimals = 2; break;
        case 2: suffix = " pt"; decimals = 1; break;
        case 3: suffix = " pc"; decimals = 2; break;
    }
    ui->spinLeft->setDecimals(decimals);
    ui->spinRight->setDecimals(decimals);
    ui->spinTop->setDecimals(decimals);
    ui->spinBottom->setDecimals(decimals);
    ui->spinLeft->setSuffix(suffix);
    ui->spinRight->setSuffix(suffix);
    ui->spinTop->setSuffix(suffix);
    ui->spinBottom->setSuffix(suffix);

    ui->spinLeft->setValue(l);
    ui->spinRight->setValue(r);
    ui->spinTop->setValue(t);
    ui->spinBottom->setValue(b);
}

void PrintWizardPagePageSetup::updatePagePreview()
{
    int idx = ui->comboPaperSize->currentIndex();
    if (idx < 0) return;
    QPageSize::PageSizeId psId =
        static_cast<QPageSize::PageSizeId>(ui->comboPaperSize->itemData(idx).toInt());
    QPageSize ps(psId);
    QSizeF s = ps.size(QPageSize::Millimeter);
    m_currentPaperSize = s.toSize();
    if (m_currentOrientation == QPageLayout::Landscape) {
        m_currentPaperSize.transpose();
    }

    // Draw preview
    int previewW = ui->labelPreview->width();
    int previewH = ui->labelPreview->height();
    if (previewW < 50 || previewH < 50) {
        previewW = 200;
        previewH = 260;
    }
    QPixmap pix(previewW, previewH);
    pix.fill(QColor(240, 240, 240));
    QPainter p(&pix);

    // Calculate paper rect
    double ratio = double(m_currentPaperSize.width()) / m_currentPaperSize.height();
    int paperW, paperH;
    if (ratio > double(previewW) / previewH) {
        paperW = previewW - 20;
        paperH = int(paperW / ratio);
    } else {
        paperH = previewH - 20;
        paperW = int(paperH * ratio);
    }
    QRect paperRect((previewW - paperW)/2, (previewH - paperH)/2, paperW, paperH);
    p.fillRect(paperRect, Qt::white);
    p.setPen(QPen(Qt::black, 1));
    p.drawRect(paperRect);

    // Draw margins
    if (!ui->checkFullPage->isChecked()) {
        int unit = ui->comboUnit->currentIndex();
        double l = ui->spinLeft->value();
        double r = ui->spinRight->value();
        double t = ui->spinTop->value();
        double b = ui->spinBottom->value();
        double toMM = 1.0;
        switch (unit) {
            case 0: toMM = 1.0; break;
            case 1: toMM = 25.4; break;
            case 2: toMM = 25.4 / 72.0; break;
            case 3: toMM = 25.4 * 12.0 / 72.0; break;
        }
        double lPx = l * toMM / m_currentPaperSize.width() * paperW;
        double rPx = r * toMM / m_currentPaperSize.width() * paperW;
        double tPx = t * toMM / m_currentPaperSize.height() * paperH;
        double bPx = b * toMM / m_currentPaperSize.height() * paperH;
        QRect marginRect(paperRect.left() + int(lPx),
                         paperRect.top() + int(tPx),
                         paperW - int(lPx) - int(rPx),
                         paperH - int(tPx) - int(bPx));
        p.setPen(QPen(QColor(150, 150, 250), 1, Qt::DashLine));
        p.drawRect(marginRect);
    }

    p.end();
    ui->labelPreview->setPixmap(pix);

    QString dim = QString("%1 x %2 mm")
        .arg(m_currentOrientation == QPageLayout::Portrait ? s.width() : s.height(), 0, 'f', 1)
        .arg(m_currentOrientation == QPageLayout::Portrait ? s.height() : s.width(), 0, 'f', 1);
    ui->labelDimensions->setText(dim);
}

void PrintWizardPagePageSetup::updatePresetVisibility()
{
    // Could be used to enable/disable margin spinboxes when fullPage is checked
    bool enabled = !ui->checkFullPage->isChecked();
    ui->spinLeft->setEnabled(enabled);
    ui->spinRight->setEnabled(enabled);
    ui->spinTop->setEnabled(enabled);
    ui->spinBottom->setEnabled(enabled);
    ui->comboPreset->setEnabled(enabled);
}

QPageSize::PageSizeId PrintWizardPagePageSetup::paperSize() const
{
    int idx = ui->comboPaperSize->currentIndex();
    if (idx < 0) return QPageSize::A4;
    return static_cast<QPageSize::PageSizeId>(ui->comboPaperSize->itemData(idx).toInt());
}

QPageLayout::Orientation PrintWizardPagePageSetup::orientation() const
{
    return m_currentOrientation;
}

qreal PrintWizardPagePageSetup::marginLeft() const
{
    return ui->spinLeft->value();
}

qreal PrintWizardPagePageSetup::marginRight() const
{
    return ui->spinRight->value();
}

qreal PrintWizardPagePageSetup::marginTop() const
{
    return ui->spinTop->value();
}

qreal PrintWizardPagePageSetup::marginBottom() const
{
    return ui->spinBottom->value();
}

bool PrintWizardPagePageSetup::fullPage() const
{
    return ui->checkFullPage->isChecked();
}

QMarginsF PrintWizardPagePageSetup::customMargins() const
{
    int unit = ui->comboUnit->currentIndex();
    double toMM = 1.0;
    switch (unit) {
        case 0: toMM = 1.0; break;
        case 1: toMM = 25.4; break;
        case 2: toMM = 25.4 / 72.0; break;
        case 3: toMM = 25.4 * 12.0 / 72.0; break;
    }
    return QMarginsF(ui->spinLeft->value() * toMM,
                     ui->spinTop->value() * toMM,
                     ui->spinRight->value() * toMM,
                     ui->spinBottom->value() * toMM);
}

void PrintWizardPagePageSetup::setPaperSize(QPageSize::PageSizeId size)
{
    int idx = ui->comboPaperSize->findData(static_cast<int>(size));
    if (idx >= 0) ui->comboPaperSize->setCurrentIndex(idx);
    updatePagePreview();
}

void PrintWizardPagePageSetup::setOrientation(QPageLayout::Orientation orient)
{
    ui->radioLandscape->setChecked(orient == QPageLayout::Landscape);
    ui->radioPortrait->setChecked(orient == QPageLayout::Portrait);
    onOrientationChanged();
}

void PrintWizardPagePageSetup::setMargins(qreal left, qreal right, qreal top, qreal bottom)
{
    ui->spinLeft->setValue(left);
    ui->spinRight->setValue(right);
    ui->spinTop->setValue(top);
    ui->spinBottom->setValue(bottom);
}

void PrintWizardPagePageSetup::setFullPage(bool full)
{
    ui->checkFullPage->setChecked(full);
    updatePresetVisibility();
}

void PrintWizardPagePageSetup::loadFromPrinter(const QPrinter *printer)
{
    if (!printer) return;
    setPaperSize(printer->pageLayout().pageSize().id());
    setOrientation(printer->pageLayout().orientation());
    QMarginsF m = printer->pageLayout().margins(QPageLayout::Millimeter);
    // Switch to mm
    ui->comboUnit->setCurrentIndex(0);
    setMargins(m.left(), m.right(), m.top(), m.bottom());
    setFullPage(printer->fullPage());
    updatePagePreview();
}

bool PrintWizardPagePageSetup::isComplete() const
{
    return ui->comboPaperSize->currentIndex() >= 0;
}