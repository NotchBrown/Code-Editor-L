#include "widget/settings/settings_dialog.h"
#include "ui_settings_dialog.h"
#include "widget/settings/general_settings_page.h"
#include "widget/settings/editor_settings_page.h"
#include "widget/hotkey/hotkey.h"
#include <QVBoxLayout>
#include <QDebug>

SettingsDialog::SettingsDialog(QWidget *parent, Tab defaultTab)
    : QDialog(parent)
    , ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    setWindowTitle(tr("Settings"));
    setMinimumSize(750, 600);

    // ── 1. General 标签页 ──
    m_generalPage = new GeneralSettingsPage(this);
    auto *layout1 = new QVBoxLayout(ui->tabGeneral);
    layout1->setContentsMargins(8, 8, 8, 8);
    layout1->addWidget(m_generalPage);

    // ── 2. Editor 标签页 ──
    m_editorPage = new EditorSettingsPage(this);
    auto *layout2 = new QVBoxLayout(ui->tabEditor);
    layout2->setContentsMargins(8, 8, 8, 8);
    layout2->addWidget(m_editorPage);

    // ── 3. Hotkey 标签页 ──
    // 注意：parentWidget() 是 MainWindow，传给 HotkeyDialog 用于查找 QAction
    m_hotkeyPage = new HotkeyDialog(this, parentWidget());
    m_hotkeyPage->setWindowFlags(Qt::Widget);

    // 隐藏 HotkeyDialog 底部的全局按钮（SettingsDialog 有自己的 Apply/OK/Cancel）
    QPushButton *applyBtn = m_hotkeyPage->findChild<QPushButton*>("applyBtn");
    QPushButton *okBtn = m_hotkeyPage->findChild<QPushButton*>("okBtn");
    QPushButton *cancelBtn = m_hotkeyPage->findChild<QPushButton*>("cancelBtn");
    QPushButton *defaultAllBtn = m_hotkeyPage->findChild<QPushButton*>("defaultAllBtn");
    if (applyBtn) applyBtn->hide();
    if (okBtn) okBtn->hide();
    if (cancelBtn) cancelBtn->hide();
    if (defaultAllBtn) defaultAllBtn->hide();

    auto *layout3 = new QVBoxLayout(ui->tabHotkey);
    layout3->setContentsMargins(8, 8, 8, 8);
    layout3->addWidget(m_hotkeyPage);

    // ── 切换到指定默认标签页 ──
    ui->tabWidget->setCurrentIndex(static_cast<int>(defaultTab));

    // ── 加载设置 ──
    m_generalPage->loadSettings();
    m_editorPage->loadSettings();

    // ── 连接按钮 ──
    connect(ui->btnApply, &QPushButton::clicked, this, &SettingsDialog::onApplyClicked);
    connect(ui->btnOk, &QPushButton::clicked, this, &SettingsDialog::onOkClicked);
    connect(ui->btnCancel, &QPushButton::clicked, this, &SettingsDialog::onCancelClicked);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::onApplyClicked()
{
    m_generalPage->saveSettings();
    m_editorPage->saveSettings();
    // 触发 HotkeyDialog 的 apply
    QMetaObject::invokeMethod(m_hotkeyPage, "onApplyClicked");
}

void SettingsDialog::onOkClicked()
{
    onApplyClicked();
    accept();
}

void SettingsDialog::onCancelClicked()
{
    // 触发 HotkeyDialog 的恢复原始快捷键
    QMetaObject::invokeMethod(m_hotkeyPage, "onRejectClicked");
    reject();
}
