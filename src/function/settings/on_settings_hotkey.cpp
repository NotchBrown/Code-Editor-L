#include "main.h"
#include "widget/main_window/main_window.h"
#include "widget/hotkey/hotkey.h"
#include "ui_main_window.h"

void MainWindow::onSettingsHotkey()
{
    HotkeyDialog dlg(this);
    dlg.exec();
}