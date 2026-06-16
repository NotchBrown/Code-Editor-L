#ifndef HOTKEY_MANAGER_H
#define HOTKEY_MANAGER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QKeySequence>

class QAction;

class HotkeyManager : public QObject
{
    Q_OBJECT

public:
    static HotkeyManager* instance();

    // Load / save shortcuts from config file
    void load();
    void save();

    // Default shortcuts map: action objectName -> key sequence
    QMap<QString, QKeySequence> defaultShortcuts() const;

    // Query and modify shortcuts
    QKeySequence shortcut(const QString &actionName) const;
    void setShortcut(const QString &actionName, const QKeySequence &key);
    void removeShortcut(const QString &actionName);
    void clearAll();

    // Apply stored shortcut to a QAction (call on startup and after changes)
    void applyToAction(QAction *action) const;
    void applyAll(QWidget *rootWidget) const;

    // Check if a shortcut is the default for an action
    bool isDefault(const QString &actionName) const;

    // Get all stored action names
    QStringList actionNames() const;

signals:
    void shortcutsChanged();

private:
    HotkeyManager(QObject *parent = nullptr);
    ~HotkeyManager();

    QString getConfigFilePath() const;

    QMap<QString, QKeySequence> m_shortcuts;
    static HotkeyManager* m_instance;
};

#endif // HOTKEY_MANAGER_H
