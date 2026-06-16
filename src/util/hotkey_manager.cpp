#include "util/hotkey_manager.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QAction>
#include <QWidget>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

HotkeyManager* HotkeyManager::m_instance = nullptr;

HotkeyManager::HotkeyManager(QObject *parent)
    : QObject(parent)
{
}

HotkeyManager::~HotkeyManager()
{
    save();
}

HotkeyManager* HotkeyManager::instance()
{
    if (!m_instance) {
        m_instance = new HotkeyManager();
    }
    return m_instance;
}

QString HotkeyManager::getConfigFilePath() const
{
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataPath);
    if (!dir.exists()) {
        dir.mkpath(dataPath);
    }
    return dataPath + "/hotkeys.xml";
}

// ---------------------------------------------------------------------------
// Default shortcuts (standard Windows / IDE conventions)
// ---------------------------------------------------------------------------

QMap<QString, QKeySequence> HotkeyManager::defaultShortcuts() const
{
    QMap<QString, QKeySequence> defs;
    defs["actionFileNew"]         = QKeySequence("Ctrl+N");
    defs["actionFileOpen"]        = QKeySequence("Ctrl+O");
    defs["actionFileSave"]        = QKeySequence("Ctrl+S");
    defs["actionFileSaveAs"]      = QKeySequence("Ctrl+Shift+S");
    defs["actionFilePrint"]       = QKeySequence("Ctrl+P");

    defs["actionEditUndo"]        = QKeySequence("Ctrl+Z");
    defs["actionEditRedo"]        = QKeySequence("Ctrl+Y");
    defs["actionEditCut"]         = QKeySequence("Ctrl+X");
    defs["actionEditCopy"]        = QKeySequence("Ctrl+C");
    defs["actionEditPaste"]       = QKeySequence("Ctrl+V");
    defs["actionEditDelete"]      = QKeySequence("Del");
    defs["actionEditGoLine"]      = QKeySequence("Ctrl+G");
    defs["actionEditComment"]     = QKeySequence("Ctrl+D");
    defs["actionEditUncomment"]   = QKeySequence("Ctrl+Shift+D");
    defs["actionEditFind"]        = QKeySequence("Ctrl+F");
    defs["actionEditFindNext"]    = QKeySequence("F3");
    defs["actionEditFindPrev"]    = QKeySequence("Shift+F3");
    defs["actionEditReplace"]     = QKeySequence("Ctrl+H");

    defs["actionWindowCloseTab"]  = QKeySequence("Ctrl+W");
    defs["actionWindowFindAndReplace"] = QKeySequence("Ctrl+Shift+F");

    defs["actionHelpHelp"]        = QKeySequence("F1");

    return defs;
}

// ---------------------------------------------------------------------------
// Load / save
// ---------------------------------------------------------------------------

void HotkeyManager::load()
{
    m_shortcuts.clear();

    QString filePath = getConfigFilePath();
    QFile file(filePath);

    if (!file.exists()) {
        // File does not exist — create it with defaults
        m_shortcuts = defaultShortcuts();
        save();
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // Fall back to defaults
        m_shortcuts = defaultShortcuts();
        return;
    }

    QXmlStreamReader reader(&file);

    while (!reader.atEnd() && !reader.hasError()) {
        QXmlStreamReader::TokenType token = reader.readNext();

        if (token == QXmlStreamReader::StartDocument) {
            continue;
        }

        if (token == QXmlStreamReader::StartElement) {
            if (reader.name() == "shortcuts") {
                continue;
            }

            if (reader.name() == "shortcut") {
                QString actionName = reader.attributes().value("action").toString();
                QString keyText    = reader.attributes().value("key").toString();
                if (!actionName.isEmpty()) {
                    m_shortcuts[actionName] = QKeySequence(keyText);
                }
                reader.readElementText(); // consume
            }
        }
    }

    file.close();

    if (reader.hasError()) {
        m_shortcuts = defaultShortcuts();
    }

    // Merge with defaults — any action not in file gets default
    QMap<QString, QKeySequence> defs = defaultShortcuts();
    for (auto it = defs.constBegin(); it != defs.constEnd(); ++it) {
        if (!m_shortcuts.contains(it.key())) {
            m_shortcuts[it.key()] = it.value();
        }
    }
}

void HotkeyManager::save()
{
    QString filePath = getConfigFilePath();
    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement("shortcuts");

    for (auto it = m_shortcuts.constBegin(); it != m_shortcuts.constEnd(); ++it) {
        if (it.value().isEmpty()) continue; // skip cleared shortcuts
        writer.writeStartElement("shortcut");
        writer.writeAttribute("action", it.key());
        writer.writeAttribute("key", it.value().toString(QKeySequence::PortableText));
        writer.writeEndElement();
    }

    writer.writeEndElement();
    writer.writeEndDocument();
    file.close();
}

// ---------------------------------------------------------------------------
// Query / modify
// ---------------------------------------------------------------------------

QKeySequence HotkeyManager::shortcut(const QString &actionName) const
{
    return m_shortcuts.value(actionName);
}

void HotkeyManager::setShortcut(const QString &actionName, const QKeySequence &key)
{
    m_shortcuts[actionName] = key;
}

void HotkeyManager::removeShortcut(const QString &actionName)
{
    m_shortcuts.remove(actionName);
}

void HotkeyManager::clearAll()
{
    m_shortcuts.clear();
}

QStringList HotkeyManager::actionNames() const
{
    return m_shortcuts.keys();
}

bool HotkeyManager::isDefault(const QString &actionName) const
{
    QMap<QString, QKeySequence> defs = defaultShortcuts();
    return defs.contains(actionName) && defs[actionName] == m_shortcuts.value(actionName);
}

// ---------------------------------------------------------------------------
// Apply to QActions
// ---------------------------------------------------------------------------

void HotkeyManager::applyToAction(QAction *action) const
{
    if (!action) return;
    QString name = action->objectName();
    if (name.isEmpty()) return;

    if (m_shortcuts.contains(name)) {
        QKeySequence ks = m_shortcuts.value(name);
        action->setShortcut(ks);
        action->setShortcutContext(Qt::ApplicationShortcut);
    }
}

void HotkeyManager::applyAll(QWidget *rootWidget) const
{
    if (!rootWidget) return;
    QList<QAction*> actions = rootWidget->findChildren<QAction*>(QString(), Qt::FindChildrenRecursively);
    foreach (QAction *action, actions) {
        applyToAction(action);
    }
}
