#include "main.h"
#include "widget/main_window/main_window.h"
#include "widget/main_window/status_bar.h"
#include "widget/editor/code_editor.h"
#include <QMessageBox>
#include <QTextCodec>

void MainWindow::onFileEncoding(const QString &encoding)
{
    CodeEditor *editor = currentEditor();
    if (!editor) {
        return;
    }
    
    QString currentEnc = editor->currentEncoding();
    if (currentEnc == encoding) {
        return;
    }
    
    // Change encoding without prompting
    if (editor->reloadWithEncoding(encoding)) {
        updateEncodingMenuChecked();
        // Update status bar encoding
        if (m_statusBar) {
            m_statusBar->setEncoding(encoding);
        }
    }
}

void MainWindow::onEncodingUTF8()
{
    onFileEncoding("UTF-8");
}

void MainWindow::onEncodingUTF8BOM()
{
    onFileEncoding("UTF-8 BOM");
}

void MainWindow::onEncodingGBK()
{
    onFileEncoding("GBK");
}

void MainWindow::onEncodingGB2312()
{
    onFileEncoding("GB2312");
}

void MainWindow::onEncodingGB18030()
{
    onFileEncoding("GB18030");
}

void MainWindow::onEncodingBig5()
{
    onFileEncoding("Big5");
}

void MainWindow::onEncodingASCII()
{
    onFileEncoding("ASCII");
}

void MainWindow::onEncodingISO88591()
{
    onFileEncoding("ISO-8859-1");
}

void MainWindow::onEncodingUTF16()
{
    onFileEncoding("UTF-16");
}

void MainWindow::onEncodingUTF16LE()
{
    onFileEncoding("UTF-16LE");
}
