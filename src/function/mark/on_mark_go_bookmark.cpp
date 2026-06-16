#include "main.h"
#include "widget/main_window/main_window.h"
#include "widget/bookmark/go_bookmark.h"

void MainWindow::onMarkGoBookmark()
{
    GoBookmark *dlg = new GoBookmark(this);
    dlg->setWindowModality(Qt::NonModal);
    dlg->setAttribute(Qt::WA_DeleteOnClose, true);
    dlg->show();
}
