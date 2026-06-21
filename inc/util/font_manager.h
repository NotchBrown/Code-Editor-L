#ifndef FONT_MANAGER_H
#define FONT_MANAGER_H

#include <QString>
#include <QStringList>
#include <QFont>
#include <QMap>

/**
 * @brief FontManager — 字体管理器，管理系统字体和安装字体
 * 
 * 功能：
 * - 扫描系统已安装字体
 * - 在 <appdir>/../resources/fonts/ 下管理安装的字体文件
 * - 用 XML 注册字体信息 (fonts.xml)
 * - 提供字体族、样式、大小列表供选择
 */
class FontManager
{
public:
    static FontManager& instance();

    /// 初始化，传入应用目录 (applicationDirPath)
    void init(const QString &appPath);

    /// 扫描系统字体 + fonts.xml 注册的字体，返回所有可用字体族（已去重）
    QStringList allFamilies() const;

    /// 返回某字体的可用样式列表 ("Regular", "Bold", "Italic" ...)
    QStringList stylesForFamily(const QString &family) const;

    /// 返回某样式可用字号列表（基于系统 + 注册表）
    QList<int> standardSizes() const;

    /// 检查字体族是否为安装的自定义字体
    bool isInstalledFont(const QString &family) const;

    /// 获取安装字体的文件路径
    QString installedFontPath(const QString &family) const;

    /// 安装字体文件（复制到资源目录并注册）
    bool installFont(const QString &filePath);

    /// 卸载字体
    bool uninstallFont(const QString &family);

    /// 获取字体资源目录
    QString fontsDir() const;

    /// 应用字体到整个应用
    void applyAppFont(const QFont &font);

    /// 获取默认等宽字体
    static QString defaultMonospaceFamily();

private:
    FontManager();
    ~FontManager() = default;
    FontManager(const FontManager&) = delete;
    FontManager& operator=(const FontManager&) = delete;

    void scanInstalledFonts();
    void loadFontRegistry();
    void saveFontRegistry();

    struct FontEntry {
        QString family;
        QString filePath;  // 相对于 fontsDir 的路径
        QString style;     // "Regular", "Bold" 等
        QString copyright; // 版权信息
    };

    QString m_fontsDir;                    // <appdir>/../resources/fonts/
    QString m_registryPath;                // fonts.xml 的完整路径
    QMap<QString, FontEntry> m_installedFonts; // family → FontEntry
    QStringList m_allFamilies;             // 缓存: 系统 + 安装字体族
    bool m_initialized = false;
};

#endif // FONT_MANAGER_H
