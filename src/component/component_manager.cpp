#include "component/component_manager.h"
#include <QCoreApplication>
#include <QDir>
#include <QDebug>
#include <QLibrary>
#include <QPluginLoader>

ComponentManager* ComponentManager::m_instance = nullptr;

ComponentManager::ComponentManager() {}
ComponentManager::~ComponentManager()
{
    for (auto *comp : m_components) {
        if (m_ownership.value(comp, false)) {
            delete comp;
        }
    }
    m_components.clear();
    m_languageIndex.clear();
}

ComponentManager* ComponentManager::instance()
{
    if (!m_instance) {
        m_instance = new ComponentManager();
    }
    return m_instance;
}

void ComponentManager::registerComponent(ILanguageComponent *component, bool owns)
{
    if (!component) return;

    m_components.append(component);
    m_ownership[component] = owns;

    QStringList langs = component->supportedLanguages();
    for (const QString &lang : langs) {
        // Only register if no component already registered for this language
        // (first registered wins, or higher capability wins)
        if (!m_languageIndex.contains(lang)) {
            m_languageIndex[lang] = component;
            qDebug() << "[ComponentMgr] Registered" << component->componentName()
                     << "for" << lang;
        }
    }
}

void ComponentManager::scanComponents(const QString &componentDir)
{
    QString dir = componentDir;
    if (dir.isEmpty()) {
        dir = QCoreApplication::applicationDirPath() + "/components";
    }

    QDir pluginDir(dir);
    if (!pluginDir.exists()) {
        qDebug() << "[ComponentMgr] Component directory not found:" << dir;
        return;
    }

    QStringList filters;
#ifdef Q_OS_WIN
    filters << "*.dll";
#else
    filters << "*.so";
#endif

    for (const QString &fileName : pluginDir.entryList(filters, QDir::Files)) {
        QString fullPath = pluginDir.absoluteFilePath(fileName);
        ILanguageComponent *comp = loadPlugin(fullPath);
        if (comp) {
            registerComponent(comp, true);
        }
    }

    qDebug() << "[ComponentMgr] Loaded" << m_components.size() << "components from" << dir;
}

ILanguageComponent* ComponentManager::loadPlugin(const QString &dllPath)
{
    QPluginLoader loader(dllPath);
    if (!loader.load()) {
        qWarning() << "[ComponentMgr] Failed to load" << dllPath << loader.errorString();
        return nullptr;
    }

    QObject *instance = loader.instance();
    if (!instance) {
        qWarning() << "[ComponentMgr] No instance from" << dllPath;
        loader.unload();
        return nullptr;
    }

    // Try qobject_cast to a known interface
    // For now, use factory function export
    QLibrary lib(dllPath);
    if (!lib.load()) {
        qWarning() << "[ComponentMgr] Cannot load" << dllPath << lib.errorString();
        loader.unload();
        return nullptr;
    }

    ComponentFactoryFunc factory = reinterpret_cast<ComponentFactoryFunc>(
        lib.resolve("componentFactory"));
    if (!factory) {
        qWarning() << "[ComponentMgr] No componentFactory in" << dllPath;
        lib.unload();
        loader.unload();
        return nullptr;
    }

    ILanguageComponent *comp = factory();
    if (!comp) {
        qWarning() << "[ComponentMgr] factory returned null from" << dllPath;
        lib.unload();
        loader.unload();
        return nullptr;
    }

    // Version check
    if (comp->apiVersion() != "1.0") {
        qWarning() << "[ComponentMgr] Version mismatch:" << comp->componentName()
                   << "api=" << comp->apiVersion() << "(expected 1.0)";
        delete comp;
        lib.unload();
        loader.unload();
        return nullptr;
    }

    qDebug() << "[ComponentMgr] Loaded component:" << comp->componentName()
             << "v" << comp->componentVersion();
    return comp;
}

ILanguageComponent* ComponentManager::componentForLanguage(const QString &lexerName) const
{
    return m_languageIndex.value(lexerName, nullptr);
}

bool ComponentManager::hasCapability(const QString &lexerName, Capability cap) const
{
    ILanguageComponent *comp = componentForLanguage(lexerName);
    if (!comp) return false;
    return (comp->capabilities() & cap) != 0;
}

QList<ILanguageComponent*> ComponentManager::loadedComponents() const
{
    return m_components;
}

QStringList ComponentManager::supportedLanguages() const
{
    return m_languageIndex.keys();
}
