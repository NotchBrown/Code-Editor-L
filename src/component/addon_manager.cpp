#include "component/addon_manager.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QLibrary>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QProcess>
#include <QDateTime>

AddonManager* AddonManager::m_instance = nullptr;

// =========================================================================
// Singleton
// =========================================================================

AddonManager::AddonManager()
{
    QDir appDir(QCoreApplication::applicationDirPath());
    appDir.cdUp();
    m_addonsDir = appDir.absoluteFilePath("addons");
}

AddonManager::~AddonManager()
{
    for (auto *comp : m_components) delete comp;
    m_components.clear();
    m_libraries.clear();
}

AddonManager* AddonManager::instance()
{
    if (!m_instance) m_instance = new AddonManager();
    return m_instance;
}

void AddonManager::setAddonsDir(const QString &path) { m_addonsDir = path; }
QString AddonManager::addonsDir() const { return m_addonsDir; }

// =========================================================================
// installed.xml
// =========================================================================

QString AddonManager::installedXmlPath() const
{
    return m_addonsDir + "/installed.xml";
}

void AddonManager::loadInstalledXml()
{
    m_states.clear();
    QFile file(installedXmlPath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QXmlStreamReader xml(&file);
    AddonState current;
    while (!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType t = xml.readNext();
        if (t != QXmlStreamReader::StartElement) continue;
        QStringRef tag = xml.name();
        if (tag == "addon") {
            current.name = xml.attributes().value("name").toString();
            current.enabled = (xml.attributes().value("enabled").toString() != "false");
            current.installPath = xml.attributes().value("path").toString();
        } else if (tag == "version") {
            current.version = xml.readElementText().trimmed();
        } else if (tag == "installDate") {
            current.installDate = QDateTime::fromString(
                xml.readElementText().trimmed(), Qt::ISODate);
        }
    }
    if (!current.name.isEmpty()) m_states[current.name] = current;
}

void AddonManager::saveInstalledXml()
{
    QDir().mkpath(m_addonsDir);
    QFile file(installedXmlPath());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;

    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
    xml.writeStartElement("installed");
    for (auto it = m_states.constBegin(); it != m_states.constEnd(); ++it) {
        const AddonState &s = it.value();
        xml.writeStartElement("addon");
        xml.writeAttribute("name", s.name);
        xml.writeAttribute("enabled", s.enabled ? "true" : "false");
        xml.writeAttribute("path", s.installPath);
        xml.writeTextElement("version", s.version);
        xml.writeTextElement("installDate", s.installDate.toString(Qt::ISODate));
        xml.writeEndElement();
    }
    xml.writeEndElement();
    xml.writeEndDocument();
}

// =========================================================================
// Scan
// =========================================================================

void AddonManager::scanAddons()
{
    loadInstalledXml();
    QDir dir(m_addonsDir);
    if (!dir.exists()) return;

    for (const QFileInfo &info : dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        if (info.fileName() == "installed.xml") continue;
        QString xmlPath = info.absoluteFilePath() + "/addon.xml";
        if (!QFile::exists(xmlPath)) continue;

        AddonDescriptor desc = parseDescriptor(xmlPath);
        if (!desc.isValid()) continue;
        desc.directory = info.absoluteFilePath();
        m_descriptors[desc.name] = desc;

        if (!m_states.contains(desc.name)) {
            AddonState st;
            st.name = desc.name;
            st.version = desc.version;
            st.enabled = true;
            st.installPath = "addons/" + desc.name;
            st.installDate = QDateTime::currentDateTime();
            m_states[desc.name] = st;
        }
    }
    saveInstalledXml();

    for (auto it = m_states.constBegin(); it != m_states.constEnd(); ++it) {
        if (it.value().enabled && m_descriptors.contains(it.key()))
            loadAddon(m_descriptors[it.key()]);
    }
}

// =========================================================================
// Enable / Disable
// =========================================================================

bool AddonManager::setEnabled(const QString &addonName, bool enabled)
{
    if (!m_states.contains(addonName)) return false;
    m_states[addonName].enabled = enabled;
    saveInstalledXml();
    if (enabled && m_descriptors.contains(addonName))
        loadAddon(m_descriptors[addonName]);
    else if (!enabled)
        unloadAddon(addonName);
    emit addonStateChanged(addonName);
    return true;
}

bool AddonManager::isEnabled(const QString &addonName) const
{
    return m_states.value(addonName).enabled;
}

// =========================================================================
// Import / Export
// =========================================================================

QString AddonManager::importAddon(const QString &zipFilePath)
{
    QFileInfo fi(zipFilePath);
    if (!fi.exists()) return "File not found";
    if (fi.suffix().toLower() != "zip") return "Not a .zip file";

    QString tempDir = m_addonsDir + "/.import_tmp";
    QDir().mkpath(tempDir);

#ifdef Q_OS_WIN
    QString extractCmd = QString(
        "powershell -Command \"Expand-Archive -Path '%1' -DestinationPath '%2' -Force\"")
        .arg(zipFilePath).arg(tempDir);
#else
    QString extractCmd = QString("unzip -o '%1' -d '%2'").arg(zipFilePath).arg(tempDir);
#endif

    QProcess proc;
    proc.start(extractCmd);
    proc.waitForFinished(30000);
    if (proc.exitCode() != 0) { QDir(tempDir).removeRecursively(); return "Extract failed"; }

    // Find addon.xml
    QString addonXmlPath;
    QStringList xmlFiles = QDir(tempDir).entryList({"addon.xml"}, QDir::Files);
    if (xmlFiles.isEmpty()) {
        for (const QFileInfo &sub : QDir(tempDir).entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)) {
            QString cand = sub.absoluteFilePath() + "/addon.xml";
            if (QFile::exists(cand)) { addonXmlPath = cand; break; }
        }
    } else {
        addonXmlPath = tempDir + "/addon.xml";
    }

    if (addonXmlPath.isEmpty()) { QDir(tempDir).removeRecursively(); return "No addon.xml"; }

    AddonDescriptor desc = parseDescriptor(addonXmlPath);
    if (!desc.isValid()) { QDir(tempDir).removeRecursively(); return "Invalid addon.xml"; }
    if (m_descriptors.contains(desc.name)) { QDir(tempDir).removeRecursively(); return "Already installed"; }

    QString targetDir = m_addonsDir + "/" + desc.name;
    QFileInfo xmlInfo(addonXmlPath);
    QString sourceDir = xmlInfo.absolutePath();

#ifdef Q_OS_WIN
    QString copyCmd = QString(
        "powershell -Command \"Copy-Item -Path '%1\\*' -Destination '%2' -Recurse -Force\"")
        .arg(sourceDir).arg(targetDir);
#else
    QString copyCmd = QString("cp -r '%1/*' '%2/'").arg(sourceDir).arg(targetDir);
#endif

    QProcess cp;
    cp.start(copyCmd);
    cp.waitForFinished(30000);
    QDir(tempDir).removeRecursively();

    if (!QFile::exists(targetDir + "/addon.xml"))
        return "Copy failed";

    AddonState st;
    st.name = desc.name; st.version = desc.version; st.enabled = true;
    st.installPath = "addons/" + desc.name;
    st.installDate = QDateTime::currentDateTime();
    m_states[desc.name] = st;
    saveInstalledXml();

    desc.directory = targetDir;
    m_descriptors[desc.name] = desc;
    if (st.enabled) loadAddon(desc);

    qDebug() << "[AddonMgr] Imported:" << desc.name;
    return QString();
}

QString AddonManager::exportAddon(const QString &addonName, const QString &outputDir)
{
    if (!m_descriptors.contains(addonName)) return "Addon not found";
    const AddonDescriptor &desc = m_descriptors[addonName];
    if (desc.directory.isEmpty()) return "Directory unknown";

    QDir().mkpath(outputDir);
    QString zipPath = outputDir + "/" + addonName + ".zip";

#ifdef Q_OS_WIN
    QString cmd = QString(
        "powershell -Command \"Compress-Archive -Path '%1\\*' -DestinationPath '%2' -Force\"")
        .arg(desc.directory).arg(zipPath);
#else
    QString cmd = QString("zip -r '%1' '%2/' -x '*.git*'").arg(zipPath).arg(desc.directory);
#endif

    QProcess proc;
    proc.start(cmd);
    proc.waitForFinished(30000);
    if (proc.exitCode() != 0 || !QFile::exists(zipPath)) return "Archive failed";

    return zipPath;
}

bool AddonManager::removeAddon(const QString &addonName)
{
    if (!m_descriptors.contains(addonName)) return false;
    unloadAddon(addonName);
    if (!m_descriptors[addonName].directory.isEmpty())
        QDir(m_descriptors[addonName].directory).removeRecursively();
    m_descriptors.remove(addonName);
    m_states.remove(addonName);
    saveInstalledXml();
    return true;
}

// =========================================================================
// Query
// =========================================================================

AddonDescriptor AddonManager::descriptor(const QString &addonName) const { return m_descriptors.value(addonName); }
AddonState AddonManager::state(const QString &addonName) const { return m_states.value(addonName); }
ILanguageComponent* AddonManager::component(const QString &addonName) const { return m_components.value(addonName); }
ILanguageComponent* AddonManager::componentForLanguage(const QString &lexerName)
{
    if (m_languageIndex.isEmpty() && m_descriptors.isEmpty()) scanAddons();
    return m_languageIndex.value(lexerName);
}

bool AddonManager::hasCapability(const QString &lexerName, Capability cap)
{
    ILanguageComponent *comp = componentForLanguage(lexerName);
    return comp && (comp->capabilities() & cap);
}

QList<AddonDescriptor> AddonManager::allAddons() const { return m_descriptors.values(); }
QList<AddonState> AddonManager::allStates() const { return m_states.values(); }

void AddonManager::reload()
{
    for (const QString &n : m_components.keys()) unloadAddon(n);
    m_descriptors.clear();
    scanAddons();
}

// =========================================================================
// Private
// =========================================================================

void AddonManager::unloadAddon(const QString &addonName)
{
    if (!m_components.contains(addonName)) return;
    ILanguageComponent *comp = m_components.take(addonName);
    for (auto it = m_languageIndex.begin(); it != m_languageIndex.end();)
        if (it.value() == comp) it = m_languageIndex.erase(it); else ++it;
    delete comp;
    if (m_libraries.contains(addonName)) delete m_libraries.take(addonName);
}

bool AddonManager::loadAddon(const AddonDescriptor &desc)
{
    if (m_components.contains(desc.name)) return true;

    QString libPath = findLibraryFile(desc);
    if (libPath.isEmpty() || !QFile::exists(libPath)) {
        emit addonLoaded(desc.name, false, "Library not found");
        return false;
    }

    QLibrary *lib = new QLibrary(libPath);
    if (!lib->load()) {
        emit addonLoaded(desc.name, false, lib->errorString());
        delete lib; return false;
    }

    auto factory = reinterpret_cast<ILanguageComponent*(*)()>(
        lib->resolve("componentFactory"));
    if (!factory) {
        lib->unload(); delete lib;
        emit addonLoaded(desc.name, false, "No componentFactory()");
        return false;
    }

    ILanguageComponent *comp = factory();
    if (!comp) { lib->unload(); delete lib; emit addonLoaded(desc.name, false, "factory()=null"); return false; }

    if (comp->apiVersion() != "1.0") {
        delete comp; lib->unload(); delete lib;
        emit addonLoaded(desc.name, false, "API version mismatch");
        return false;
    }

    m_components[desc.name] = comp;
    m_libraries[desc.name] = lib;
    for (const QString &lang : comp->supportedLanguages())
        if (!m_languageIndex.contains(lang)) m_languageIndex[lang] = comp;

    emit addonLoaded(desc.name, true, QString());
    return true;
}

AddonDescriptor AddonManager::parseDescriptor(const QString &xmlPath) const
{
    AddonDescriptor desc;
    QFile file(xmlPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return desc;

    QXmlStreamReader xml(&file);
    while (!xml.atEnd() && !xml.hasError()) {
        if (xml.readNext() != QXmlStreamReader::StartElement) continue;
        QStringRef tag = xml.name();
        if (tag == "name")        desc.name = xml.readElementText().trimmed();
        else if (tag == "vendor") desc.vendor = xml.readElementText().trimmed();
        else if (tag == "displayName") desc.displayName = xml.readElementText().trimmed();
        else if (tag == "version") desc.version = xml.readElementText().trimmed();
        else if (tag == "description") desc.description = xml.readElementText().trimmed();
        else if (tag == "capability") desc.capabilities.append(xml.readElementText().trimmed());
        else if (tag == "file") {
            AddonFileInfo fi;
            fi.platform = xml.attributes().value("platform").toString();
            fi.fileName = xml.readElementText().trimmed();
            if (!fi.fileName.isEmpty()) desc.files.append(fi);
        }
    }
    return desc;
}

QString AddonManager::findLibraryFile(const AddonDescriptor &desc) const
{
#ifdef Q_OS_WIN
    QString plat = "win", suf = ".dll";
#elif defined(Q_OS_MACOS)
    QString plat = "mac", suf = ".dylib";
#else
    QString plat = "linux", suf = ".so";
#endif
    for (const AddonFileInfo &fi : desc.files) {
        if (fi.platform == plat || fi.platform == "all" || fi.fileName.endsWith(suf))
            return desc.directory + "/" + fi.fileName;
    }
    return QString();
}
