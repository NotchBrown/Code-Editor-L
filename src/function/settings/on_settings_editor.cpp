#include "main.h"
#include "widget/main_window/main_window.h"
#include "widget/settings/settings_dialog.h"

void MainWindow::onSettingsEditor()
{
    SettingsDialog dlg(this, SettingsDialog::TabEditor);
    dlg.exec();
}
