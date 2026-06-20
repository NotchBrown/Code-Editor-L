#include "main.h"
#include "widget/main_window/main_window.h"
#include "widget/addons/addons_dialog.h"
#include "component/addon_manager.h"

void MainWindow::onSettingsAddons()
{
    // Ensure addons are scanned (if not already)
    AddonManager::instance()->scanAddons();

    AddonsDialog dialog(this);
    dialog.exec();
}
