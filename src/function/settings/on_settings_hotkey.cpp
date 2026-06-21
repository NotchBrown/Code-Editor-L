#include "main.h"
#include "widget/main_window/main_window.h"
#include "widget/settings/settings_dialog.h"

void MainWindow::onSettingsHotkey()
{
    SettingsDialog dlg(this, SettingsDialog::TabHotkey);
    dlg.exec();
}