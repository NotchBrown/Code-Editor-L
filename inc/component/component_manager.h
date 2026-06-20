#ifndef COMPONENT_MANAGER_H
#define COMPONENT_MANAGER_H

#include "component/language_component.h"
#include <QObject>
#include <QList>
#include <QMap>
#include <QString>

class QLibrary;

// ---------------------------------------------------------------------------
// ComponentManager - singleton that discovers and manages language components
// ---------------------------------------------------------------------------

class ComponentManager : public QObject
{
    Q_OBJECT

public:
    static ComponentManager* instance();

    // Scan components/ directory and load all valid components
    void scanComponents(const QString &componentDir = QString());

    // Get the best component for a language
    ILanguageComponent* componentForLanguage(const QString &lexerName) const;

    // Check if a language has a specific capability
    bool hasCapability(const QString &lexerName, Capability cap) const;

    // List all loaded components
    QList<ILanguageComponent*> loadedComponents() const;

    // List languages that have at least basic component support
    QStringList supportedLanguages() const;

private:
    ComponentManager();
    ~ComponentManager() override;

    // Register a component (called by scanComponents or for built-in)
    void registerComponent(ILanguageComponent *component, bool owns = true);

    // Load a single plugin DLL
    ILanguageComponent* loadPlugin(const QString &dllPath);

    static ComponentManager *m_instance;

    // Index: lexer name -> best component
    QMap<QString, ILanguageComponent*> m_languageIndex;

    // All loaded components
    QList<ILanguageComponent*> m_components;

    // Whether we own the component (delete on shutdown)
    QMap<ILanguageComponent*, bool> m_ownership;
};

#endif // COMPONENT_MANAGER_H
