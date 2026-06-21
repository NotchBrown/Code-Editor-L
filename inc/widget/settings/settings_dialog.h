#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H

#include "main.h"

class GeneralSettingsPage;
class EditorSettingsPage;
class HotkeyDialog;

namespace Ui {
class SettingsDialog;
}

/**
 * @brief SettingsDialog — 统一设置对话框
 *
 * 包含 3 个标签页:
 * - General: 软件字体、自动保存、默认面板
 * - Editor: 编辑器设置（按语言区分）
 * - Hotkey: 快捷键设置（复用 HotkeyDialog 内容）
 *
 * 菜单中 General/Editor/Hotkey 均可打开此对话框并跳转到对应标签页。
 */
class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    enum Tab {
        TabGeneral = 0,
        TabEditor = 1,
        TabHotkey = 2
    };

    explicit SettingsDialog(QWidget *parent = nullptr, Tab defaultTab = TabGeneral);
    ~SettingsDialog();

private slots:
    void onApplyClicked();
    void onOkClicked();
    void onCancelClicked();

private:
    Ui::SettingsDialog *ui;
    GeneralSettingsPage *m_generalPage;
    EditorSettingsPage *m_editorPage;
    QWidget *m_hotkeyPage;
};

#endif // SETTINGS_DIALOG_H
