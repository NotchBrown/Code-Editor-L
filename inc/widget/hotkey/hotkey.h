#ifndef HOTKEY_DIALOG_H
#define HOTKEY_DIALOG_H

#include "main.h"
#include <QKeySequence>

class QTableWidgetItem;

namespace Ui {
class HotkeyDialog;
}

class HotkeyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HotkeyDialog(QWidget *parent = nullptr);
    ~HotkeyDialog();

private slots:
    // Row-level operations
    void onSetClicked();
    void onClearClicked();
    void onDefaultClicked();

    // Global operations
    void onDefaultAllClicked();
    void onApplyClicked();
    void onAcceptClicked();
    void onRejectClicked();

    // Table selection tracking
    void onSelectionChanged();

private:
    void setupUI();
    void setupConnections();
    void populateActions();
    void addActionRow(const QString &actionName, const QString &displayName,
                      const QKeySequence &shortcut, const QString &description);
    void setRowShortcut(int row, const QKeySequence &ks);
    void applyToActions();

    // Helper: clean up action text (remove & accelerators)
    static QString cleanActionText(const QString &text);

    // Helper: build description from action name
    static QString actionDescription(const QString &actionName);

    Ui::HotkeyDialog *ui;

    // Store original shortcuts so we can cancel
    QMap<QString, QKeySequence> m_originalShortcuts;

    // Column indices
    enum Column {
        ColAction     = 0,
        ColShortcut   = 1,
        ColDescription = 2
    };

    // Data roles
    static constexpr int ActionNameRole = Qt::UserRole;
};

#endif // HOTKEY_DIALOG_H
