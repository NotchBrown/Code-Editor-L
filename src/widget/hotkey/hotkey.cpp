#include "main.h"
#include "widget/hotkey/hotkey.h"
#include "ui_hotkey.h"
#include "util/hotkey_manager.h"
#include <QHeaderView>
#include <QKeySequenceEdit>
#include <QDialogButtonBox>
#include <QVBoxLayout>

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

HotkeyDialog::HotkeyDialog(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::HotkeyDialog)
{
    ui->setupUi(this);

    // Configure table
    ui->actionTable->horizontalHeader()->setSectionResizeMode(ColAction, QHeaderView::Stretch);
    ui->actionTable->horizontalHeader()->setSectionResizeMode(ColShortcut, QHeaderView::ResizeToContents);
    ui->actionTable->horizontalHeader()->setSectionResizeMode(ColDescription, QHeaderView::Stretch);
    ui->actionTable->verticalHeader()->hide();

    setupConnections();
    populateActions();
}

HotkeyDialog::~HotkeyDialog()
{
    delete ui;
}

// ---------------------------------------------------------------------------
// Signal connections
// ---------------------------------------------------------------------------

void HotkeyDialog::setupConnections()
{
    // Row-level operations
    connect(ui->setBtn, &QPushButton::clicked, this, &HotkeyDialog::onSetClicked);
    connect(ui->clearBtn, &QPushButton::clicked, this, &HotkeyDialog::onClearClicked);
    connect(ui->defaultBtn, &QPushButton::clicked, this, &HotkeyDialog::onDefaultClicked);

    // Global operations
    connect(ui->defaultAllBtn, &QPushButton::clicked, this, &HotkeyDialog::onDefaultAllClicked);
    connect(ui->applyBtn, &QPushButton::clicked, this, &HotkeyDialog::onApplyClicked);
    connect(ui->okBtn, &QPushButton::clicked, this, &HotkeyDialog::onAcceptClicked);
    connect(ui->cancelBtn, &QPushButton::clicked, this, &HotkeyDialog::onRejectClicked);

    // Selection tracking
    connect(ui->actionTable, &QTableWidget::itemSelectionChanged,
            this, &HotkeyDialog::onSelectionChanged);
}

// ---------------------------------------------------------------------------
// Populate the action table
// ---------------------------------------------------------------------------

void HotkeyDialog::populateActions()
{
    ui->actionTable->setSortingEnabled(false);
    ui->actionTable->setRowCount(0);

    HotkeyManager *hkm = HotkeyManager::instance();
    QMap<QString, QKeySequence> defaults = hkm->defaultShortcuts();

    // We get actions from the parent MainWindow
    QWidget *mainWindow = parentWidget();
    if (!mainWindow) {
        // Fallback: just list defaults
        for (auto it = defaults.constBegin(); it != defaults.constEnd(); ++it) {
            QString displayName = cleanActionText(it.key());
            // Remove "action" prefix for cleaner names
            if (displayName.startsWith("action", Qt::CaseInsensitive))
                displayName = displayName.mid(6);
            addActionRow(it.key(), displayName, hkm->shortcut(it.key()), actionDescription(it.key()));
        }
    } else {
        // Get all actions from the main window
        QList<QAction*> actions = mainWindow->findChildren<QAction*>(QString(), Qt::FindChildrenRecursively);

        // Build a set of known action names (from defaults) for ordering
        QStringList knownOrder = defaults.keys();

        // Add known actions first, in default order
        for (const QString &name : knownOrder) {
            QAction *action = mainWindow->findChild<QAction*>(name);
            if (!action) continue;

            QString displayName = action->text();
            if (displayName.isEmpty()) {
                displayName = cleanActionText(name);
                if (displayName.startsWith("action", Qt::CaseInsensitive))
                    displayName = displayName.mid(6);
            } else {
                displayName = cleanActionText(displayName);
            }

            QKeySequence currentKs = hkm->shortcut(name);
            addActionRow(name, displayName, currentKs, actionDescription(name));

            // Store original for cancel
            m_originalShortcuts[name] = currentKs;
        }

        // Add any remaining actions from MainWindow that are not in defaults
        for (QAction *action : actions) {
            QString name = action->objectName();
            if (name.isEmpty() || m_originalShortcuts.contains(name)) continue;

            QString displayName = cleanActionText(action->text());
            if (displayName.isEmpty()) {
                displayName = cleanActionText(name);
                if (displayName.startsWith("action", Qt::CaseInsensitive))
                    displayName = displayName.mid(6);
            }

            QKeySequence ks = hkm->shortcut(name);
            addActionRow(name, displayName, ks, actionDescription(name));
            m_originalShortcuts[name] = ks;
        }
    }

    ui->actionTable->setSortingEnabled(true);
    ui->actionTable->sortByColumn(ColAction, Qt::AscendingOrder);

    onSelectionChanged();
}

void HotkeyDialog::addActionRow(const QString &actionName, const QString &displayName,
                                const QKeySequence &shortcut, const QString &description)
{
    int row = ui->actionTable->rowCount();
    ui->actionTable->insertRow(row);

    QTableWidgetItem *nameItem = new QTableWidgetItem(displayName);
    nameItem->setData(ActionNameRole, actionName);
    nameItem->setToolTip(actionName);
    ui->actionTable->setItem(row, ColAction, nameItem);

    QTableWidgetItem *shortcutItem = new QTableWidgetItem(shortcut.toString(QKeySequence::NativeText));
    shortcutItem->setTextAlignment(Qt::AlignCenter);
    ui->actionTable->setItem(row, ColShortcut, shortcutItem);

    QTableWidgetItem *descItem = new QTableWidgetItem(description);
    descItem->setToolTip(description);
    ui->actionTable->setItem(row, ColDescription, descItem);
}

void HotkeyDialog::setRowShortcut(int row, const QKeySequence &ks)
{
    QTableWidgetItem *item = ui->actionTable->item(row, ColShortcut);
    if (item) {
        item->setText(ks.toString(QKeySequence::NativeText));
    }
}

// ---------------------------------------------------------------------------
// Selection tracking
// ---------------------------------------------------------------------------

void HotkeyDialog::onSelectionChanged()
{
    bool hasSelection = !ui->actionTable->selectedItems().isEmpty();
    ui->setBtn->setEnabled(hasSelection);
    ui->clearBtn->setEnabled(hasSelection);
    ui->defaultBtn->setEnabled(hasSelection);
}

// ---------------------------------------------------------------------------
// Row-level operations
// ---------------------------------------------------------------------------

void HotkeyDialog::onSetClicked()
{
    int row = ui->actionTable->currentRow();
    if (row < 0) return;

    // Show a small dialog with QKeySequenceEdit
    QDialog dlg(this);
    dlg.setWindowTitle(tr("Set Shortcut"));
    QVBoxLayout *layout = new QVBoxLayout(&dlg);
    QKeySequenceEdit *editor = new QKeySequenceEdit(&dlg);
    layout->addWidget(editor);

    QDialogButtonBox *box = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    layout->addWidget(box);
    connect(box, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(box, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() == QDialog::Accepted) {
        QKeySequence ks = editor->keySequence();
        if (!ks.isEmpty()) {
            setRowShortcut(row, ks);

            // Also update the HotkeyManager in-memory
            QTableWidgetItem *nameItem = ui->actionTable->item(row, ColAction);
            if (nameItem) {
                QString actionName = nameItem->data(ActionNameRole).toString();
                HotkeyManager::instance()->setShortcut(actionName, ks);
            }
        }
    }
}

void HotkeyDialog::onClearClicked()
{
    int row = ui->actionTable->currentRow();
    if (row < 0) return;

    setRowShortcut(row, QKeySequence());

    QTableWidgetItem *nameItem = ui->actionTable->item(row, ColAction);
    if (nameItem) {
        QString actionName = nameItem->data(ActionNameRole).toString();
        HotkeyManager::instance()->removeShortcut(actionName);
    }
}

void HotkeyDialog::onDefaultClicked()
{
    int row = ui->actionTable->currentRow();
    if (row < 0) return;

    QTableWidgetItem *nameItem = ui->actionTable->item(row, ColAction);
    if (!nameItem) return;

    QString actionName = nameItem->data(ActionNameRole).toString();
    QMap<QString, QKeySequence> defs = HotkeyManager::instance()->defaultShortcuts();

    QKeySequence defaultKs = defs.value(actionName);
    setRowShortcut(row, defaultKs);
    HotkeyManager::instance()->setShortcut(actionName, defaultKs);
}

// ---------------------------------------------------------------------------
// Global operations
// ---------------------------------------------------------------------------

void HotkeyDialog::onDefaultAllClicked()
{
    QMap<QString, QKeySequence> defs = HotkeyManager::instance()->defaultShortcuts();

    for (int row = 0; row < ui->actionTable->rowCount(); ++row) {
        QTableWidgetItem *nameItem = ui->actionTable->item(row, ColAction);
        if (!nameItem) continue;

        QString actionName = nameItem->data(ActionNameRole).toString();
        QKeySequence defaultKs = defs.value(actionName);
        setRowShortcut(row, defaultKs);
        HotkeyManager::instance()->setShortcut(actionName, defaultKs);
    }
}

void HotkeyDialog::onApplyClicked()
{
    applyToActions();
    HotkeyManager::instance()->save();
}

void HotkeyDialog::onAcceptClicked()
{
    applyToActions();
    HotkeyManager::instance()->save();
    accept();
}

void HotkeyDialog::onRejectClicked()
{
    // Restore original shortcuts in HotkeyManager
    HotkeyManager *hkm = HotkeyManager::instance();
    for (auto it = m_originalShortcuts.constBegin(); it != m_originalShortcuts.constEnd(); ++it) {
        hkm->setShortcut(it.key(), it.value());
    }
    // Reload from manager (which still has originals)
    hkm->load(); // re-read from file to be safe
    hkm->applyAll(parentWidget());
    reject();
}

// ---------------------------------------------------------------------------
// Apply shortcuts to actual QActions
// ---------------------------------------------------------------------------

void HotkeyDialog::applyToActions()
{
    QWidget *mainWindow = parentWidget();
    if (!mainWindow) return;

    HotkeyManager::instance()->applyAll(mainWindow);
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

QString HotkeyDialog::cleanActionText(const QString &text)
{
    QString result = text;
    // Remove single & (Qt accelerator marker), keep && as &
    result.replace("&&", "\x01"); // temporary placeholder
    result.remove('&');
    result.replace('\x01', "&");
    return result.trimmed();
}

QString HotkeyDialog::actionDescription(const QString &actionName)
{
    // Build readable descriptions from action object names
    if (actionName.startsWith("actionFileNew"))        return "Create a new file";
    if (actionName.startsWith("actionFileOpen"))       return "Open an existing file";
    if (actionName.startsWith("actionFileSave"))       return "Save current file";
    if (actionName.startsWith("actionFileSaveAs"))     return "Save current file with a new name";
    if (actionName.startsWith("actionFileProperties")) return "View file properties";
    if (actionName.startsWith("actionFileReadOnly"))   return "Toggle read-only mode";
    if (actionName.startsWith("actionFilePrint"))      return "Print current file";

    if (actionName.startsWith("actionEditUndo"))       return "Undo last action";
    if (actionName.startsWith("actionEditRedo"))       return "Redo last undone action";
    if (actionName.startsWith("actionEditCut"))        return "Cut selection to clipboard";
    if (actionName.startsWith("actionEditCopy"))       return "Copy selection to clipboard";
    if (actionName.startsWith("actionEditPaste"))      return "Paste from clipboard";
    if (actionName.startsWith("actionEditDelete"))     return "Delete selection";
    if (actionName.startsWith("actionEditGoLine"))     return "Go to a specific line number";
    if (actionName.startsWith("actionEditComment"))    return "Comment selected lines";
    if (actionName.startsWith("actionEditUncomment"))  return "Uncomment selected lines";
    if (actionName.startsWith("actionEditFind"))       return "Find text in current document";
    if (actionName.startsWith("actionEditFindNext"))   return "Find next occurrence";
    if (actionName.startsWith("actionEditFindPrev"))   return "Find previous occurrence";
    if (actionName.startsWith("actionEditReplace"))    return "Find and replace text";

    if (actionName.startsWith("actionMarkGoError"))       return "Navigate to next error";
    if (actionName.startsWith("actionMarkDeleteError"))   return "Delete current error mark";
    if (actionName.startsWith("actionMarkCleanError"))    return "Clear all error marks";
    if (actionName.startsWith("actionMarkGoBookmark"))    return "Navigate to bookmark";
    if (actionName.startsWith("actionMarkCreateBookmark")) return "Create a bookmark";
    if (actionName.startsWith("actionMarkManageBookmark")) return "Open bookmark manager";
    if (actionName.startsWith("actionMarkCreateBreak"))   return "Create a breakpoint";
    if (actionName.startsWith("actionMarkRemoveBreak"))  return "Remove a breakpoint";

    if (actionName.startsWith("actionWindowCloseTab"))      return "Close current tab";
    if (actionName.startsWith("actionWindowCloseAllSaved")) return "Close all saved tabs";
    if (actionName.startsWith("actionWindowCloseAll"))      return "Close all tabs";
    if (actionName.startsWith("actionWindowCloseOther"))    return "Close all tabs except current";
    if (actionName.startsWith("actionWindowNavigator"))     return "Toggle navigator panel";
    if (actionName.startsWith("actionWindowSegment"))       return "Toggle segment panel";
    if (actionName.startsWith("actionWindowFindAndReplace")) return "Toggle find and replace panel";
    if (actionName.startsWith("actionWindowErrorsWarnings")) return "Toggle errors and warnings panel";
    if (actionName.startsWith("actionWindowIPCMessage"))    return "Toggle IPC message panel";

    if (actionName.startsWith("actionViewFullscreen")) return "Toggle fullscreen mode";
    if (actionName.startsWith("actionViewStatusbar"))  return "Toggle status bar visibility";
    if (actionName.startsWith("actionViewToolbar"))    return "Toggle toolbar visibility";

    if (actionName.startsWith("actionSettingsGeneral")) return "Configure general settings";
    if (actionName.startsWith("actionSettingsEditor"))  return "Configure editor settings";
    if (actionName.startsWith("actionSettingsIPC"))     return "Configure IPC settings";
    if (actionName.startsWith("actionSettingsToolbar"))  return "Configure toolbar settings";
    if (actionName.startsWith("actionSettingsHotkey"))  return "Configure keyboard shortcuts";

    if (actionName.startsWith("actionHelpAbout")) return "About CodeEditorLite";
    if (actionName.startsWith("actionHelpHelp")) return "Open help";

    if (actionName.startsWith("actionStartNewInstance")) return "Open a new CodeEditorLite instance";
    if (actionName.startsWith("actionStartExit"))        return "Exit CodeEditorLite";

    if (actionName.startsWith("actionEncoding")) return "Change file encoding";

    return QString();
}
