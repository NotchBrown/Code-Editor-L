#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H

#include <QString>
#include <QStringList>
#include <QFont>
#include <QMap>
#include <QSettings>
#include <QDir>

/**
 * @brief SettingsManager — 统一设置管理（QSettings + XML）
 *
 * 管理所有用户设置，持久化到 QSettings (INI 格式)。
 * 编辑器设置支持按语言区分。
 */
class SettingsManager
{
public:
    static SettingsManager& instance();

    void init(const QString &appPath);

    // ==================== General ====================
    // 软件字体（非编辑器）
    QFont appFont() const;
    void setAppFont(const QFont &font);
    int appFontSize() const;
    void setAppFontSize(int size);

    // 自动保存
    bool autoSaveEnabled() const;
    void setAutoSaveEnabled(bool enabled);
    int autoSaveInterval() const;       // 秒
    void setAutoSaveInterval(int secs);

    // 默认打开面板
    QStringList defaultPanels() const;  // 面板名称列表
    void setDefaultPanels(const QStringList &panels);

    // ==================== Editor (per-language) ====================
    // 编辑器字体
    QFont editorFont(const QString &lang = "default") const;
    void setEditorFont(const QString &lang, const QFont &font);
    int editorFontSize(const QString &lang = "default") const;
    void setEditorFontSize(const QString &lang, int size);

    // 静态自动补全（基于 QScintilla 关键字）
    bool staticCompletionEnabled(const QString &lang = "default") const;
    void setStaticCompletionEnabled(const QString &lang, bool enabled);

    // 高级自动补全（基于 AddOns tree-sitter）
    bool advancedCompletionEnabled(const QString &lang = "default") const;
    void setAdvancedCompletionEnabled(const QString &lang, bool enabled);

    // 软换行
    bool softWrapEnabled(const QString &lang = "default") const;
    void setSoftWrapEnabled(const QString &lang, bool enabled);

    // 竖线提示（边列）
    bool edgeLineEnabled(const QString &lang = "default") const;
    void setEdgeLineEnabled(const QString &lang, bool enabled);
    int edgeLineColumn(const QString &lang = "default") const;
    void setEdgeLineColumn(const QString &lang, int column);

    // 奇偶行不同底色
    bool alternateLineColorEnabled(const QString &lang = "default") const;
    void setAlternateLineColorEnabled(const QString &lang, bool enabled);

    // ==================== 所有已知语言列表 ====================
    QStringList knownLanguages() const;

    // ==================== 保存/加载 ====================
    void save();
    void load();

private:
    SettingsManager();
    ~SettingsManager() = default;
    SettingsManager(const SettingsManager&) = delete;
    SettingsManager& operator=(const SettingsManager&) = delete;

    QString settingsFilePath() const;

    QSettings *m_settings = nullptr;
    QString m_settingsDir;
};

#endif // SETTINGS_MANAGER_H
