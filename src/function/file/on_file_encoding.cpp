#include "main.h"
#include "widget/main_window/main_window.h"
#include "widget/editor/code_editor.h"
#include <QMessageBox>
#include <QTextCodec>

void MainWindow::onFileEncoding(const QString &encoding)
{
    CodeEditor *editor = currentEditor();
    if (!editor) {
        QMessageBox::warning(this, "Encoding", "No active editor.");
        return;
    }
    
    QString currentEnc = editor->currentEncoding();
    if (currentEnc == encoding) {
        return;
    }
    
    if (editor->isModified()) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Encoding",
            "The document has been modified. Do you want to save changes before changing encoding?",
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        
        if (reply == QMessageBox::Cancel) {
            return;
        } else if (reply == QMessageBox::Save) {
            if (!editor->saveFile()) {
                QMessageBox::warning(this, "Encoding", "Failed to save file.");
                return;
            }
        }
    }
    
    if (editor->reloadWithEncoding(encoding)) {
        QMessageBox::information(this, "Encoding", 
            QString("Encoding changed to %1").arg(encoding));
    } else {
        QMessageBox::warning(this, "Encoding", "Failed to change encoding.");
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