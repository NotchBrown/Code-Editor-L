#ifndef ADDON_MANAGER_H
#define ADDON_MANAGER_H

#include <QObject>
#include <QList>
#include <QMap>
#include "component/addon_info.h"
#include "component/language_component.h"

class QLibrary;

// ---------------------------------------------------------------------------
// AddonManager - manages addon lifecycle: discovery, install, enable/disable
// ---------------------------------------------------------------------------

class AddonManager : public QObject
{
    Q_OBJECT

public:
    static AddonManager* instance();

    // ---- Initialization ----
    // Set the base directory (default: <appDir>/../addons)
    void setAddonsDir(const QString &path);
    QString addonsDir() const;

    // Scan installed addons, load enabled ones
    void scanAddons();

    // ---- Query ----
    AddonDescriptor descriptor(const QString &addonName) const;
    AddonState       state(const QString &addonName) const;
    ILanguageComponent* component(const QString &addonName) const;
    ILanguageComponent* componentForLanguage(const QString &lexerName);
    bool hasCapability(const QString &lexerName, Capability cap);

    QList<AddonDescriptor> allAddons() const;
    QList<AddonState>      allStates() const;

    // ---- Enable / Disable ----
    bool setEnabled(const QString &addonName, bool enabled);
    bool isEnabled(const QString &addonName) const;

    // ---- Import / Export ----
    // Import from a .zip file: validate, extract, register in installed.xml
    // Returns error string on failure, empty on success
    QString importAddon(const QString &zipFilePath);

    // Export an addon as a .zip file
    // Returns the output zip path on success
    QString exportAddon(const QString &addonName, const QString &outputDir);

    // ---- Remove ----
    bool removeAddon(const QString &addonName);

    // ---- Reload ----
    void reload();

signals:
    void addonLoaded(const QString &name, bool success, const QString &error);
    void addonStateChanged(const QString &name);

private:
    AddonManager();
    ~AddonManager() override;

    // ---- installed.xml management ----
    QString installedXmlPath() const;
    void    saveInstalledXml();
    void    loadInstalledXml();

    // ---- Addon loading ----
    bool loadAddon(const AddonDescriptor &desc);
    void unloadAddon(const QString &addonName);
    AddonDescriptor parseDescriptor(const QString &xmlPath) const;
    QString findLibraryFile(const AddonDescriptor &desc) const;
    bool    verifyAddon(const QString &addonDir) const;

    static AddonManager *m_instance;

    QString m_addonsDir;  // absolute path to addons/ directory

    // Descriptors parsed from addon.xml files (all discovered)
    QMap<QString, AddonDescriptor> m_descriptors;

    // Runtime state from installed.xml
    QMap<QString, AddonState> m_states;

    // Loaded components (addon name -> component)
    QMap<QString, ILanguageComponent*> m_components;

    // Loaded library handles
    QMap<QString, QLibrary*> m_libraries;

    // Language index (lexer name -> component)
    QMap<QString, ILanguageComponent*> m_languageIndex;
};

#endif // ADDON_MANAGER_H
