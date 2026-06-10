#include "main.h"
#include "widget/main_window/main_window.h"

void MainWindow::onViewFullscreen()
{
    if (isFullScreen()) {
        showNormal();
    } else {
        showFullScreen();
    }
}
