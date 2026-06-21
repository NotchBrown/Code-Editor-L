#include "util/settings_manager.h"
#include <QApplication>
#include <QDebug>
#include <QFont>

SettingsManager& SettingsManager::instance()
{
    static SettingsManager inst;
    return inst;
}

SettingsManager::SettingsManager()
{
}

void SettingsManager::init(const QString &appPath)
{
    // 设置目录: bin/../config/（与 bin/ 同级）
    QDir dir(appPath + "/../config");
    m_settingsDir = dir.absolutePath();
    QDir().mkpath(m_settingsDir);

    m_settings = new QSettings(settingsFilePath(), QSettings::IniFormat);
    load();
}

QString SettingsManager::settingsFilePath() const
{
    return m_settingsDir + "/settings.ini";
}

// ==================== General ====================

QFont SettingsManager::appFont() const
{
    QFont f;
    f.fromString(m_settings->value("General/AppFont", "Microsoft YaHei").toString());
    return f;
}

void SettingsManager::setAppFont(const QFont &font)
{
    m_settings->setValue("General/AppFont", font.toString());
}

int SettingsManager::appFontSize() const
{
    return m_settings->value("General/AppFontSize", 10).toInt();
}

void SettingsManager::setAppFontSize(int size)
{
    m_settings->setValue("General/AppFontSize", size);
}

bool SettingsManager::autoSaveEnabled() const
{
    return m_settings->value("General/AutoSaveEnabled", false).toBool();
}

void SettingsManager::setAutoSaveEnabled(bool enabled)
{
    m_settings->setValue("General/AutoSaveEnabled", enabled);
}

int SettingsManager::autoSaveInterval() const
{
    return m_settings->value("General/AutoSaveInterval", 300).toInt(); // 默认 5 分钟
}

void SettingsManager::setAutoSaveInterval(int secs)
{
    m_settings->setValue("General/AutoSaveInterval", secs);
}

QStringList SettingsManager::defaultPanels() const
{
    return m_settings->value("General/DefaultPanels",
        QStringList() << "navigator" << "segment").toStringList();
}

void SettingsManager::setDefaultPanels(const QStringList &panels)
{
    m_settings->setValue("General/DefaultPanels", panels);
}

// ==================== Editor ====================

static QString langKey(const QString &lang, const QString &key)
{
    return "Editor/" + lang + "/" + key;
}

QFont SettingsManager::editorFont(const QString &lang) const
{
    QFont f;
    f.fromString(m_settings->value(langKey(lang, "Font"), "Consolas").toString());
    return f;
}

void SettingsManager::setEditorFont(const QString &lang, const QFont &font)
{
    m_settings->setValue(langKey(lang, "Font"), font.toString());
}

int SettingsManager::editorFontSize(const QString &lang) const
{
    return m_settings->value(langKey(lang, "FontSize"), 10).toInt();
}

void SettingsManager::setEditorFontSize(const QString &lang, int size)
{
    m_settings->setValue(langKey(lang, "FontSize"), size);
}

bool SettingsManager::staticCompletionEnabled(const QString &lang) const
{
    return m_settings->value(langKey(lang, "StaticCompletion"), true).toBool();
}

void SettingsManager::setStaticCompletionEnabled(const QString &lang, bool enabled)
{
    m_settings->setValue(langKey(lang, "StaticCompletion"), enabled);
}

bool SettingsManager::advancedCompletionEnabled(const QString &lang) const
{
    return m_settings->value(langKey(lang, "AdvancedCompletion"), false).toBool();
}

void SettingsManager::setAdvancedCompletionEnabled(const QString &lang, bool enabled)
{
    m_settings->setValue(langKey(lang, "AdvancedCompletion"), enabled);
}

bool SettingsManager::softWrapEnabled(const QString &lang) const
{
    return m_settings->value(langKey(lang, "SoftWrap"), false).toBool();
}

void SettingsManager::setSoftWrapEnabled(const QString &lang, bool enabled)
{
    m_settings->setValue(langKey(lang, "SoftWrap"), enabled);
}

bool SettingsManager::edgeLineEnabled(const QString &lang) const
{
    return m_settings->value(langKey(lang, "EdgeLine"), true).toBool();
}

void SettingsManager::setEdgeLineEnabled(const QString &lang, bool enabled)
{
    m_settings->setValue(langKey(lang, "EdgeLine"), enabled);
}

int SettingsManager::edgeLineColumn(const QString &lang) const
{
    return m_settings->value(langKey(lang, "EdgeColumn"), 80).toInt();
}

void SettingsManager::setEdgeLineColumn(const QString &lang, int column)
{
    m_settings->setValue(langKey(lang, "EdgeColumn"), column);
}

bool SettingsManager::alternateLineColorEnabled(const QString &lang) const
{
    return m_settings->value(langKey(lang, "AlternateLine"), true).toBool();
}

void SettingsManager::setAlternateLineColorEnabled(const QString &lang, bool enabled)
{
    m_settings->setValue(langKey(lang, "AlternateLine"), enabled);
}

QStringList SettingsManager::knownLanguages() const
{
    // 从 settings 中读取已知语言列表，或返回默认列表
    QStringList defaults;
    defaults << "default" << "cpp" << "python" << "javascript" << "html" << "css"
             << "java" << "csharp" << "ruby" << "bash" << "php" << "go" << "rust"
             << "scala" << "haskell" << "typescript" << "tsx" << "json" << "verilog"
             << "sql" << "lua" << "perl" << "xml" << "yaml" << "markdown"
             << "fortran" << "pascal" << "vhdl" << "cmake" << "makefile" << "batch"
             << "tcl" << "d";
    QStringList langs = m_settings->value("Editor/KnownLanguages",
        QVariant(defaults)).toStringList();
    return langs;
}

void SettingsManager::save()
{
    m_settings->sync();
}

void SettingsManager::load()
{
    // QSettings 自动加载，无需额外操作
    m_settings->sync();
}
