#include "widget/settings/general_settings_page.h"
#include "ui_general_settings_page.h"
#include "util/font_manager.h"
#include "util/settings_manager.h"
#include <QFont>
#include <QFontDatabase>
#include <QListWidgetItem>
#include <QDebug>

GeneralSettingsPage::GeneralSettingsPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GeneralSettingsPage)
{
    ui->setupUi(this);

    connect(ui->cmbFontCategory, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &GeneralSettingsPage::onFontCategoryChanged);
    connect(ui->cmbFontFamily, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &GeneralSettingsPage::onFontFamilyChanged);
    connect(ui->cmbFontSize, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &GeneralSettingsPage::onFontSizeChanged);
    connect(ui->txtFontPreview, &QLineEdit::textChanged,
            this, &GeneralSettingsPage::onPreviewTextChanged);
}

GeneralSettingsPage::~GeneralSettingsPage()
{
    delete ui;
}

void GeneralSettingsPage::populateFontCategories()
{
    ui->cmbFontCategory->clear();
    ui->cmbFontCategory->addItem(tr("System Fonts"), "system");
    ui->cmbFontCategory->addItem(tr("Installed Fonts"), "installed");
}

void GeneralSettingsPage::populateFontFamilies()
{
    ui->cmbFontFamily->clear();
    QString category = ui->cmbFontCategory->currentData().toString();
    QStringList families;

    if (category == "system") {
        // 系统字体：所有 QFontDatabase 中的字体
        families = QFontDatabase().families();
    } else {
        // 安装字体：FontManager 管理的自定义字体
        const auto &fm = FontManager::instance();
        QStringList all = fm.allFamilies();
        for (const QString &f : all) {
            if (fm.isInstalledFont(f))
                families.append(f);
        }
    }

    families.sort();
    ui->cmbFontFamily->addItems(families);

    // 选中当前设置
    QString current = SettingsManager::instance().appFont().family();
    int idx = families.indexOf(current);
    if (idx >= 0)
        ui->cmbFontFamily->setCurrentIndex(idx);
    else if (families.contains("Consolas"))
        ui->cmbFontFamily->setCurrentText("Consolas");
}

void GeneralSettingsPage::populateFontSizes()
{
    ui->cmbFontSize->clear();
    QList<int> sizes = FontManager::instance().standardSizes();
    for (int s : sizes)
        ui->cmbFontSize->addItem(QString::number(s));

    int currentSize = SettingsManager::instance().appFontSize();
    int idx = sizes.indexOf(currentSize);
    if (idx >= 0)
        ui->cmbFontSize->setCurrentIndex(idx);
    else
        ui->cmbFontSize->setEditText(QString::number(currentSize));
}

void GeneralSettingsPage::updateFontPreview()
{
    QString family = ui->cmbFontFamily->currentText();
    int size = ui->cmbFontSize->currentText().toInt();
    if (size <= 0) size = 10;

    QFont font(family, size);
    ui->txtFontPreview->setFont(font);
}

void GeneralSettingsPage::populatePanelList()
{
    ui->listPanels->clear();

    // 5 个面板
    struct PanelInfo {
        QString id;
        QString label;
    };
    QList<PanelInfo> panels = {
        {"navigator",        "Navigator (Symbol Outline)"},
        {"segment",          "Segment (Context Info)"},
        {"find_and_replace", "Find and Replace"},
        {"errors_warnings",  "Errors & Warnings"},
        {"ipc_message",      "IPC Message"}
    };

    QStringList enabled = SettingsManager::instance().defaultPanels();

    for (const auto &p : panels) {
        auto *item = new QListWidgetItem(p.label, ui->listPanels);
        item->setData(Qt::UserRole, p.id);
        item->setSelected(enabled.contains(p.id));
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(enabled.contains(p.id) ? Qt::Checked : Qt::Unchecked);
    }
}

void GeneralSettingsPage::loadSettings()
{
    populateFontCategories();
    populateFontFamilies();
    populateFontSizes();

    // 自动保存
    ui->chkAutoSave->setChecked(SettingsManager::instance().autoSaveEnabled());
    ui->spinAutoSaveInterval->setValue(SettingsManager::instance().autoSaveInterval());

    // 面板列表
    populatePanelList();

    updateFontPreview();
}

void GeneralSettingsPage::saveSettings()
{
    // 字体
    QString family = ui->cmbFontFamily->currentText();
    int size = ui->cmbFontSize->currentText().toInt();
    if (size <= 0) size = 10;
    QFont font(family, size);
    SettingsManager::instance().setAppFont(font);
    SettingsManager::instance().setAppFontSize(size);

    // 自动保存
    SettingsManager::instance().setAutoSaveEnabled(ui->chkAutoSave->isChecked());
    SettingsManager::instance().setAutoSaveInterval(ui->spinAutoSaveInterval->value());

    // 面板
    QStringList panels;
    for (int i = 0; i < ui->listPanels->count(); ++i) {
        auto *item = ui->listPanels->item(i);
        if (item->checkState() == Qt::Checked)
            panels << item->data(Qt::UserRole).toString();
    }
    SettingsManager::instance().setDefaultPanels(panels);

    SettingsManager::instance().save();

    // 立即应用字体
    FontManager::instance().applyAppFont(font);
}

void GeneralSettingsPage::onFontCategoryChanged(int)
{
    // 切换分类时重新填充字体列表
    QString currentFamily = SettingsManager::instance().appFont().family();
    populateFontFamilies();

    // 尝试保留当前选中的字体
    for (int i = 0; i < ui->cmbFontFamily->count(); ++i) {
        if (ui->cmbFontFamily->itemText(i) == currentFamily) {
            ui->cmbFontFamily->setCurrentIndex(i);
            return;
        }
    }
    updateFontPreview();
}

void GeneralSettingsPage::onFontFamilyChanged(int)
{
    updateFontPreview();
}

void GeneralSettingsPage::onFontSizeChanged(int)
{
    updateFontPreview();
}

void GeneralSettingsPage::onPreviewTextChanged()
{
    updateFontPreview();
}
