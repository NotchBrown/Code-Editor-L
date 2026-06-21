#include "widget/settings/editor_settings_page.h"
#include "ui_editor_settings_page.h"
#include "util/font_manager.h"
#include "util/settings_manager.h"
#include <QFont>
#include <QFontDatabase>
#include <QDebug>

EditorSettingsPage::EditorSettingsPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::EditorSettingsPage)
{
    ui->setupUi(this);

    connect(ui->cmbLanguage, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &EditorSettingsPage::onLanguageChanged);
    connect(ui->cmbFontFamily, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &EditorSettingsPage::onFontFamilyChanged);
    connect(ui->cmbFontSize, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &EditorSettingsPage::onFontSizeChanged);
    connect(ui->cmbFontCategory, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &EditorSettingsPage::onFontCategoryChanged);

    // 所有复选框和 spinbox 变更时标记已修改
    connect(ui->chkStaticCompletion, &QCheckBox::toggled, this, &EditorSettingsPage::onSettingChanged);
    connect(ui->chkAdvancedCompletion, &QCheckBox::toggled, this, &EditorSettingsPage::onSettingChanged);
    connect(ui->chkSoftWrap, &QCheckBox::toggled, this, &EditorSettingsPage::onSettingChanged);
    connect(ui->chkEdgeLine, &QCheckBox::toggled, this, &EditorSettingsPage::onSettingChanged);
    connect(ui->chkAlternateLine, &QCheckBox::toggled, this, &EditorSettingsPage::onSettingChanged);
    connect(ui->spinEdgeColumn, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &EditorSettingsPage::onSettingChanged);
}

EditorSettingsPage::~EditorSettingsPage()
{
    delete ui;
}

void EditorSettingsPage::populateLanguageList()
{
    ui->cmbLanguage->clear();
    QStringList langs = SettingsManager::instance().knownLanguages();
    for (const QString &l : langs) {
        QString label = (l == "default") ? QString("(Default)") : l;
        ui->cmbLanguage->addItem(label, l);
    }
}

void EditorSettingsPage::populateFontFamilies()
{
    ui->cmbFontFamily->clear();
    QString category = ui->cmbFontCategory->currentData().toString();
    QStringList families;

    if (category == "system") {
        families = QFontDatabase().families();
    } else {
        const auto &fm = FontManager::instance();
        QStringList all = fm.allFamilies();
        for (const QString &f : all) {
            if (fm.isInstalledFont(f))
                families.append(f);
        }
    }

    families.sort();
    ui->cmbFontFamily->addItems(families);
}

void EditorSettingsPage::populateFontCategories()
{
    ui->cmbFontCategory->clear();
    ui->cmbFontCategory->addItem(tr("System Fonts"), "system");
    ui->cmbFontCategory->addItem(tr("Installed Fonts"), "installed");
}

void EditorSettingsPage::populateFontSizes()
{
    ui->cmbFontSize->clear();
    QList<int> sizes = FontManager::instance().standardSizes();
    for (int s : sizes)
        ui->cmbFontSize->addItem(QString::number(s));
}

void EditorSettingsPage::loadSettings()
{
    m_loading = true;

    populateLanguageList();
    populateFontCategories();
    populateFontFamilies();
    populateFontSizes();

    m_loading = false;

    // 加载默认语言的设置
    if (ui->cmbLanguage->count() > 0)
        ui->cmbLanguage->setCurrentIndex(0);
    loadCurrentLanguageSettings();
}

void EditorSettingsPage::saveSettings()
{
    // 保存所有语言的设置（当前正在编辑的语言已实时保存）
    // 将当前页面的值保存到当前语言
    if (!m_currentLang.isEmpty()) {
        auto &s = SettingsManager::instance();

        QString family = ui->cmbFontFamily->currentText();
        int size = ui->cmbFontSize->currentText().toInt();
        if (size <= 0) size = 10;

        s.setEditorFont(m_currentLang, QFont(family, size));
        s.setEditorFontSize(m_currentLang, size);
        s.setStaticCompletionEnabled(m_currentLang, ui->chkStaticCompletion->isChecked());
        s.setAdvancedCompletionEnabled(m_currentLang, ui->chkAdvancedCompletion->isChecked());
        s.setSoftWrapEnabled(m_currentLang, ui->chkSoftWrap->isChecked());
        s.setEdgeLineEnabled(m_currentLang, ui->chkEdgeLine->isChecked());
        s.setEdgeLineColumn(m_currentLang, ui->spinEdgeColumn->value());
        s.setAlternateLineColorEnabled(m_currentLang, ui->chkAlternateLine->isChecked());
    }

    SettingsManager::instance().save();
}

void EditorSettingsPage::onLanguageChanged(int index)
{
    if (m_loading || index < 0) return;

    // 保存当前语言的设置
    if (!m_currentLang.isEmpty()) {
        auto &s = SettingsManager::instance();
        int size = ui->cmbFontSize->currentText().toInt();
        if (size <= 0) size = 10;
        s.setEditorFont(m_currentLang, QFont(ui->cmbFontFamily->currentText(), size));
        s.setEditorFontSize(m_currentLang, size);
        s.setStaticCompletionEnabled(m_currentLang, ui->chkStaticCompletion->isChecked());
        s.setAdvancedCompletionEnabled(m_currentLang, ui->chkAdvancedCompletion->isChecked());
        s.setSoftWrapEnabled(m_currentLang, ui->chkSoftWrap->isChecked());
        s.setEdgeLineEnabled(m_currentLang, ui->chkEdgeLine->isChecked());
        s.setEdgeLineColumn(m_currentLang, ui->spinEdgeColumn->value());
        s.setAlternateLineColorEnabled(m_currentLang, ui->chkAlternateLine->isChecked());
    }

    // 切换到新语言
    m_currentLang = ui->cmbLanguage->itemData(index).toString();
    loadCurrentLanguageSettings();
}

void EditorSettingsPage::loadCurrentLanguageSettings()
{
    m_loading = true;
    const auto &s = SettingsManager::instance();

    QFont font = s.editorFont(m_currentLang);
    int size = s.editorFontSize(m_currentLang);

    // 选中字体族
    int idx = ui->cmbFontFamily->findText(font.family());
    if (idx >= 0) ui->cmbFontFamily->setCurrentIndex(idx);

    // 选中字号
    idx = ui->cmbFontSize->findText(QString::number(size));
    if (idx >= 0)
        ui->cmbFontSize->setCurrentIndex(idx);
    else
        ui->cmbFontSize->setEditText(QString::number(size));

    ui->chkStaticCompletion->setChecked(s.staticCompletionEnabled(m_currentLang));
    ui->chkAdvancedCompletion->setChecked(s.advancedCompletionEnabled(m_currentLang));
    ui->chkSoftWrap->setChecked(s.softWrapEnabled(m_currentLang));
    ui->chkEdgeLine->setChecked(s.edgeLineEnabled(m_currentLang));
    ui->spinEdgeColumn->setValue(s.edgeLineColumn(m_currentLang));
    ui->chkAlternateLine->setChecked(s.alternateLineColorEnabled(m_currentLang));

    updatePreview();
    m_loading = false;
}

void EditorSettingsPage::updatePreview()
{
    QString family = ui->cmbFontFamily->currentText();
    int size = ui->cmbFontSize->currentText().toInt();
    if (size <= 0) size = 10;
    ui->txtPreview->setFont(QFont(family, size));
}

void EditorSettingsPage::onFontCategoryChanged(int)
{
    if (m_loading) return;
    // 重新填充字体列表，尝试保留当前字体族
    QString currentFamily = ui->cmbFontFamily->currentText();
    populateFontFamilies();
    int idx = ui->cmbFontFamily->findText(currentFamily);
    if (idx >= 0)
        ui->cmbFontFamily->setCurrentIndex(idx);
    updatePreview();
}

void EditorSettingsPage::onFontFamilyChanged(int)
{
    if (!m_loading) updatePreview();
}

void EditorSettingsPage::onFontSizeChanged(int)
{
    if (!m_loading) updatePreview();
}

void EditorSettingsPage::onSettingChanged()
{
    // 任何设置变更时实时保存到当前语言
    if (m_loading || m_currentLang.isEmpty()) return;

    auto &s = SettingsManager::instance();
    int size = ui->cmbFontSize->currentText().toInt();
    if (size <= 0) size = 10;
    s.setEditorFont(m_currentLang, QFont(ui->cmbFontFamily->currentText(), size));
    s.setEditorFontSize(m_currentLang, size);
    s.setStaticCompletionEnabled(m_currentLang, ui->chkStaticCompletion->isChecked());
    s.setAdvancedCompletionEnabled(m_currentLang, ui->chkAdvancedCompletion->isChecked());
    s.setSoftWrapEnabled(m_currentLang, ui->chkSoftWrap->isChecked());
    s.setEdgeLineEnabled(m_currentLang, ui->chkEdgeLine->isChecked());
    s.setEdgeLineColumn(m_currentLang, ui->spinEdgeColumn->value());
    s.setAlternateLineColorEnabled(m_currentLang, ui->chkAlternateLine->isChecked());
}
