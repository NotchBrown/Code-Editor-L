#include "util/font_manager.h"
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QFontDatabase>
#include <QDebug>
#include <QApplication>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QSet>

FontManager& FontManager::instance()
{
    static FontManager inst;
    return inst;
}

FontManager::FontManager()
{
}

void FontManager::init(const QString &appPath)
{
    if (m_initialized) return;

    QDir appDir(appPath);
    appDir.cdUp();
    QDir resDir(appPath + "/../resource");
    m_fontsDir = resDir.absolutePath() + "/font";
    m_registryPath = m_fontsDir + "/fonts.xml";

    // 确保目录存在
    QDir().mkpath(m_fontsDir);

    // 扫描已安装字体
    scanInstalledFonts();

    // 构建系统+安装字体的完整族列表
    QStringList systemFamilies = QFontDatabase().families();
    m_allFamilies = systemFamilies;
    for (auto it = m_installedFonts.constBegin(); it != m_installedFonts.constEnd(); ++it) {
        if (!m_allFamilies.contains(it.key()))
            m_allFamilies.append(it.key());
    }
    m_allFamilies.sort();

    m_initialized = true;
    qDebug() << "[FontManager] initialized, fonts dir:" << m_fontsDir
             << "total families:" << m_allFamilies.size();
}

QStringList FontManager::allFamilies() const
{
    return m_allFamilies;
}

QStringList FontManager::stylesForFamily(const QString &family) const
{
    // 系统字体优先
    QFontDatabase db;
    if (db.families().contains(family)) {
        return db.styles(family);
    }
    // 安装字体
    if (m_installedFonts.contains(family)) {
        return {m_installedFonts[family].style};
    }
    return {"Regular"};
}

QList<int> FontManager::standardSizes() const
{
    return QFontDatabase::standardSizes();
}

bool FontManager::isInstalledFont(const QString &family) const
{
    return m_installedFonts.contains(family);
}

QString FontManager::installedFontPath(const QString &family) const
{
    if (!m_installedFonts.contains(family)) return {};
    return m_fontsDir + "/" + m_installedFonts[family].filePath;
}

bool FontManager::installFont(const QString &filePath)
{
    QFileInfo fi(filePath);
    if (!fi.exists()) return false;

    // 复制到 fonts 目录
    QString destPath = m_fontsDir + "/" + fi.fileName();
    if (!QFile::copy(filePath, destPath)) {
        // 可能已存在
        if (!QFile::exists(destPath)) {
            qWarning() << "[FontManager] Failed to copy font:" << filePath;
            return false;
        }
    }

    // 加载到系统字体数据库
    int id = QFontDatabase::addApplicationFont(destPath);
    if (id < 0) {
        qWarning() << "[FontManager] Failed to load font:" << destPath;
        QFile::remove(destPath);
        return false;
    }

    QStringList families = QFontDatabase::applicationFontFamilies(id);
    if (families.isEmpty()) return false;

    // 注册
    FontEntry entry;
    entry.family = families.first();
    entry.filePath = fi.fileName();
    entry.style = "Regular";

    // 尝试提取版权信息
    QFile fontFile(destPath);
    if (fontFile.open(QIODevice::ReadOnly)) {
        QByteArray head = fontFile.read(4096);
        fontFile.close();
        // 在 TTF/OTF 的 name table 中查找版权条目 (nameID=0)
        // 简单提取: 查找常见版权标记
        QString headStr = QString::fromUtf8(head);
        int cIdx = headStr.indexOf("Copyright", Qt::CaseInsensitive);
        if (cIdx < 0) cIdx = headStr.indexOf("(c)", Qt::CaseInsensitive);
        if (cIdx < 0) cIdx = headStr.indexOf("©");
        if (cIdx >= 0) {
            int endIdx = headStr.indexOf('\0', cIdx);
            if (endIdx < 0) endIdx = qMin(cIdx + 200, headStr.size());
            entry.copyright = headStr.mid(cIdx, endIdx - cIdx).trimmed();
        }
    }
    m_installedFonts[entry.family] = entry;

    // 更新 allFamilies
    if (!m_allFamilies.contains(entry.family)) {
        m_allFamilies.append(entry.family);
        m_allFamilies.sort();
    }

    saveFontRegistry();
    qDebug() << "[FontManager] Installed font:" << entry.family;
    return true;
}

bool FontManager::uninstallFont(const QString &family)
{
    if (!m_installedFonts.contains(family)) return false;

    QString filePath = m_fontsDir + "/" + m_installedFonts[family].filePath;
    QFile::remove(filePath);

    m_installedFonts.remove(family);
    m_allFamilies.removeAll(family);

    saveFontRegistry();
    return true;
}

QString FontManager::fontsDir() const
{
    return m_fontsDir;
}

void FontManager::applyAppFont(const QFont &font)
{
    QApplication::setFont(font);
}

QString FontManager::defaultMonospaceFamily()
{
#ifdef Q_OS_WIN
    return "Consolas";
#else
    return "Monospace";
#endif
}

void FontManager::scanInstalledFonts()
{
    loadFontRegistry();

    // 递归扫描 fonts 目录中所有 .ttf/.otf/.ttc 文件
    QDirIterator it(m_fontsDir, QStringList() << "*.ttf" << "*.otf" << "*.ttc",
                    QDir::Files, QDirIterator::Subdirectories);
    QSet<QString> foundFiles;

    while (it.hasNext()) {
        QString absPath = it.next();
        QFileInfo fi(absPath);
        foundFiles.insert(fi.fileName());

        // 检查是否已在注册表中
        bool registered = false;
        for (auto it2 = m_installedFonts.constBegin(); it2 != m_installedFonts.constEnd(); ++it2) {
            if (it2.value().filePath == fi.fileName() ||
                it2.value().filePath == absPath) {
                registered = true;
                break;
            }
        }
        if (registered) continue;

        // 未注册但存在文件 → 尝试加载
        int id = QFontDatabase::addApplicationFont(absPath);
        if (id >= 0) {
            QStringList families = QFontDatabase::applicationFontFamilies(id);
            if (!families.isEmpty()) {
                FontEntry entry;
                entry.family = families.first();
                entry.filePath = fi.fileName();
                entry.style = "Regular";
                // 提取版权
                QFile f(absPath);
                if (f.open(QIODevice::ReadOnly)) {
                    QByteArray head = f.read(4096);
                    QString headStr = QString::fromUtf8(head);
                    int ci = headStr.indexOf("Copyright", Qt::CaseInsensitive);
                    if (ci < 0) ci = headStr.indexOf("(c)", Qt::CaseInsensitive);
                    if (ci < 0) ci = headStr.indexOf(QChar(0xA9)); // ©
                    if (ci >= 0) {
                        int ei = headStr.indexOf('\0', ci);
                        if (ei < 0) ei = qMin(ci + 200, headStr.size());
                        entry.copyright = headStr.mid(ci, ei - ci).trimmed();
                    }
                    f.close();
                }
                m_installedFonts[entry.family] = entry;
                qDebug() << "[FontManager] Found font:" << entry.family;
            }
        }
    }

    if (!m_installedFonts.isEmpty())
        saveFontRegistry();
}

void FontManager::loadFontRegistry()
{
    QFile file(m_registryPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    m_installedFonts.clear();

    QXmlStreamReader xml(&file);
    while (!xml.atEnd() && !xml.hasError()) {
        xml.readNext();
        if (xml.isStartElement() && xml.name().toString() == "font") {
            FontEntry entry;
            entry.family = xml.attributes().value("family").toString();
            entry.filePath = xml.attributes().value("file").toString();
            entry.style = xml.attributes().value("style").toString();
            entry.copyright = xml.attributes().value("copyright").toString();
            if (!entry.family.isEmpty() && !entry.filePath.isEmpty()) {
                m_installedFonts[entry.family] = entry;
            }
        }
    }
    file.close();
}

void FontManager::saveFontRegistry()
{
    QFile file(m_registryPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    xml.writeStartDocument("1.0");
    xml.writeStartElement("fonts");

    for (auto it = m_installedFonts.constBegin(); it != m_installedFonts.constEnd(); ++it) {
        xml.writeStartElement("font");
        xml.writeAttribute("family", it.value().family);
        xml.writeAttribute("file", it.value().filePath);
        xml.writeAttribute("style", it.value().style);
        if (!it.value().copyright.isEmpty())
            xml.writeAttribute("copyright", it.value().copyright);
        xml.writeEndElement();
    }

    xml.writeEndElement();
    xml.writeEndDocument();
    file.close();
}
