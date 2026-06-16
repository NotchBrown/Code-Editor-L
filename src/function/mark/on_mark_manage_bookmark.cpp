#include "main.h"
#include "widget/main_window/main_window.h"
#include "widget/bookmark/manage_bookmark.h"

void MainWindow::onMarkManageBookmark()
{
    ManageBookmark dlg(this);
    dlg.exec();
}
