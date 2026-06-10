#include "widget/main_window/main_window.h"
#include "ui_main_window.h"
#include "widget/editor/code_editor.h"
#include "ipc/ipc_server.h"
#include "project/project_manager.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      m_ipcServer(nullptr),
      m_projectManager(nullptr)
{
    ui->setupUi(this);
    setupConnections();
    createNewEditor();
}

MainWindow::~MainWindow()
{
    delete m_ipcServer;
    delete m_projectManager;
    delete ui;
}

void MainWindow::setupConnections()
{
    connect(ui->actionFileNew, &QAction::triggered, this, &MainWindow::onFileNew);
    connect(ui->actionFileOpen, &QAction::triggered, this, &MainWindow::onFileOpen);
    connect(ui->actionFileSave, &QAction::triggered, this, &MainWindow::onFileSave);
    connect(ui->actionFileSaveAs, &QAction::triggered, this, &MainWindow::onFileSaveAs);
    connect(ui->actionFileExit, &QAction::triggered, this, &MainWindow::onFileExit);
    
    connect(ui->actionEditUndo, &QAction::triggered, this, &MainWindow::onEditUndo);
    connect(ui->actionEditRedo, &QAction::triggered, this, &MainWindow::onEditRedo);
    connect(ui->actionEditCut, &QAction::triggered, this, &MainWindow::onEditCut);
    connect(ui->actionEditCopy, &QAction::triggered, this, &MainWindow::onEditCopy);
    connect(ui->actionEditPaste, &QAction::triggered, this, &MainWindow::onEditPaste);
    connect(ui->actionEditFind, &QAction::triggered, this, &MainWindow::onEditFind);
    connect(ui->actionEditReplace, &QAction::triggered, this, &MainWindow::onEditReplace);
    
    connect(ui->actionViewZoomIn, &QAction::triggered, this, &MainWindow::onViewZoomIn);
    connect(ui->actionViewZoomOut, &QAction::triggered, this, &MainWindow::onViewZoomOut);
    connect(ui->actionViewResetZoom, &QAction::triggered, this, &MainWindow::onViewResetZoom);
    connect(ui->actionViewLineNumbers, &QAction::triggered, this, &MainWindow::onViewLineNumbers);
    connect(ui->actionViewFolding, &QAction::triggered, this, &MainWindow::onViewFolding);
    
    connect(ui->actionToolsPreferences, &QAction::triggered, this, &MainWindow::onToolsPreferences);
    connect(ui->actionHelpAbout, &QAction::triggered, this, &MainWindow::onHelpAbout);
    
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onCurrentTabChanged);
}

CodeEditor* MainWindow::createNewEditor(const QString &filePath)
{
    CodeEditor *editor = new CodeEditor();
    editor->setUndoDepth(100);
    
    if (!filePath.isEmpty()) {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            editor->setText(QString::fromUtf8(file.readAll()));
            editor->setFilePath(filePath);
            editor->setModified(false);
            file.close();
            
            QFileInfo info(filePath);
            editor->setLexerByExtension(info.suffix());
            m_fileToTabIndex[filePath] = ui->tabWidget->count();
            ui->tabWidget->addTab(editor, info.fileName());
        }
    } else {
        static int untitledCount = 1;
        QString title = QString("Untitled %1").arg(untitledCount++);
        ui->tabWidget->addTab(editor, title);
    }
    
    connect(editor, &CodeEditor::modificationChanged, this, &MainWindow::onEditorModificationChanged);
    connect(editor, &CodeEditor::cursorPositionChanged, this, &MainWindow::onEditorCursorPositionChanged);
    
    ui->tabWidget->setCurrentWidget(editor);
    return editor;
}

CodeEditor* MainWindow::currentEditor() const
{
    return qobject_cast<CodeEditor*>(ui->tabWidget->currentWidget());
}

void MainWindow::updateWindowTitle()
{
    CodeEditor *editor = currentEditor();
    if (editor) {
        QString title = "CodeEditorLite";
        if (!editor->filePath().isEmpty()) {
            QString fileName = QFileInfo(editor->filePath()).fileName();
            title = QString("%1 - %2").arg(fileName).arg(title);
        } else {
            title = QString("%1 - %2").arg(ui->tabWidget->tabText(ui->tabWidget->currentIndex())).arg(title);
        }
        if (editor->isModified()) {
            title += "*";
        }
        setWindowTitle(title);
    }
}

bool MainWindow::closeTab(int index)
{
    CodeEditor *editor = qobject_cast<CodeEditor*>(ui->tabWidget->widget(index));
    if (!editor) return true;
    
    if (editor->isModified()) {
        QString fileName = editor->filePath().isEmpty() 
            ? ui->tabWidget->tabText(index) 
            : QFileInfo(editor->filePath()).fileName();
        
        QMessageBox::StandardButton result = QMessageBox::question(
            this, 
            tr("Save Changes?"),
            tr("Save changes to '%1'?").arg(fileName),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel
        );
        
        if (result == QMessageBox::Save) {
            if (!saveCurrentFile()) return false;
        } else if (result == QMessageBox::Cancel) {
            return false;
        }
    }
    
    if (!editor->filePath().isEmpty()) {
        m_fileToTabIndex.remove(editor->filePath());
    }
    
    ui->tabWidget->removeTab(index);
    delete editor;
    
    if (ui->tabWidget->count() == 0) {
        createNewEditor();
    }
    
    return true;
}

bool MainWindow::saveCurrentFile()
{
    CodeEditor *editor = currentEditor();
    if (!editor) return false;
    
    QString filePath = editor->filePath();
    
    if (filePath.isEmpty()) {
        filePath = QFileDialog::getSaveFileName(
            this, 
            tr("Save File"),
            QString(),
            tr("All Files (*.*)")
        );
        
        if (filePath.isEmpty()) return false;
        
        editor->setFilePath(filePath);
        QFileInfo info(filePath);
        editor->setLexerByExtension(info.suffix());
        m_fileToTabIndex[filePath] = ui->tabWidget->currentIndex();
        ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), info.fileName());
    }
    
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.write(editor->text().toUtf8());
        editor->setModified(false);
        file.close();
        ui->statusBar->showMessage(tr("Saved: %1").arg(filePath));
        updateWindowTitle();
        return true;
    } else {
        QMessageBox::warning(this, tr("Error"), tr("Cannot save file"));
        return false;
    }
}

void MainWindow::initIpcServer(int port)
{
    m_ipcServer = new IpcServer(this);
    if (port > 0) {
        m_ipcServer->start(port);
    }
}

void MainWindow::onFileNew()
{
    createNewEditor();
    updateWindowTitle();
    ui->statusBar->showMessage(tr("New file created"));
}

void MainWindow::onFileOpen()
{
    QString fileName = QFileDialog::getOpenFileName(
        this, 
        tr("Open File"),
        QString(),
        tr("All Files (*.*)")
    );
    
    if (!fileName.isEmpty()) {
        if (m_fileToTabIndex.contains(fileName)) {
            ui->tabWidget->setCurrentIndex(m_fileToTabIndex[fileName]);
            ui->statusBar->showMessage(tr("Already open: %1").arg(fileName));
            return;
        }
        
        createNewEditor(fileName);
        updateWindowTitle();
        ui->statusBar->showMessage(tr("Opened: %1").arg(fileName));
    }
}

void MainWindow::onFileSave()
{
    if (saveCurrentFile()) {
        ui->statusBar->showMessage(tr("File saved"));
    }
}

void MainWindow::onFileSaveAs()
{
    CodeEditor *editor = currentEditor();
    if (!editor) return;
    
    QString filePath = QFileDialog::getSaveFileName(
        this, 
        tr("Save File As"),
        QString(),
        tr("All Files (*.*)")
    );
    
    if (!filePath.isEmpty()) {
        if (!editor->filePath().isEmpty()) {
            m_fileToTabIndex.remove(editor->filePath());
        }
        
        editor->setFilePath(filePath);
        QFileInfo info(filePath);
        editor->setLexerByExtension(info.suffix());
        m_fileToTabIndex[filePath] = ui->tabWidget->currentIndex();
        ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), info.fileName());
        
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            file.write(editor->text().toUtf8());
            editor->setModified(false);
            file.close();
            updateWindowTitle();
            ui->statusBar->showMessage(tr("Saved as: %1").arg(filePath));
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Cannot save file"));
        }
    }
}

void MainWindow::onFileExit()
{
    while (ui->tabWidget->count() > 0) {
        if (!closeTab(0)) return;
    }
    close();
}

void MainWindow::onEditUndo()
{
    CodeEditor *editor = currentEditor();
    if (editor) editor->undo();
}

void MainWindow::onEditRedo()
{
    CodeEditor *editor = currentEditor();
    if (editor) editor->redo();
}

void MainWindow::onEditCut()
{
    CodeEditor *editor = currentEditor();
    if (editor) editor->cut();
}

void MainWindow::onEditCopy()
{
    CodeEditor *editor = currentEditor();
    if (editor) editor->copy();
}

void MainWindow::onEditPaste()
{
    CodeEditor *editor = currentEditor();
    if (editor) editor->paste();
}

void MainWindow::onEditFind()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Find"), tr("Find what:"), QLineEdit::Normal, QString(), &ok);
    if (ok && !text.isEmpty()) {
        CodeEditor *editor = currentEditor();
        if (editor) editor->findText(text);
    }
}

void MainWindow::onEditReplace()
{
    bool ok;
    QString find = QInputDialog::getText(this, tr("Replace"), tr("Find what:"), QLineEdit::Normal, QString(), &ok);
    if (!ok || find.isEmpty()) return;
    
    QString replace = QInputDialog::getText(this, tr("Replace"), tr("Replace with:"), QLineEdit::Normal, QString(), &ok);
    if (!ok) return;
    
    CodeEditor *editor = currentEditor();
    if (editor) {
        QMessageBox::StandardButton result = QMessageBox::question(
            this, 
            tr("Replace"),
            tr("Replace all occurrences?"),
            QMessageBox::Yes | QMessageBox::No
        );
        editor->replaceText(find, replace, (result == QMessageBox::Yes));
    }
}

void MainWindow::onViewZoomIn()
{
    CodeEditor *editor = currentEditor();
    if (editor) editor->zoomIn();
}

void MainWindow::onViewZoomOut()
{
    CodeEditor *editor = currentEditor();
    if (editor) editor->zoomOut();
}

void MainWindow::onViewResetZoom()
{
    CodeEditor *editor = currentEditor();
    if (editor) {
        editor->zoomTo(0);
    }
}

void MainWindow::onViewLineNumbers()
{
    CodeEditor *editor = currentEditor();
    if (editor) {
        editor->setMarginWidth(0, ui->actionViewLineNumbers->isChecked() ? 50 : 0);
    }
}

void MainWindow::onViewFolding()
{
    CodeEditor *editor = currentEditor();
    if (editor) {
        editor->setFolding(ui->actionViewFolding->isChecked() ? QsciScintilla::BoxedTreeFoldStyle : QsciScintilla::NoFoldStyle);
    }
}

void MainWindow::onToolsPreferences()
{
    QMessageBox::information(this, tr("Preferences"), tr("Preferences dialog will be implemented in future version."));
}

void MainWindow::onHelpAbout()
{
    QMessageBox::about(this, tr("About CodeEditorLite"), 
        tr("CodeEditorLite v1.0.0\n\nA lightweight code editor with syntax highlighting, "
           "multiple tab support, and IPC communication capabilities."));
}

void MainWindow::onTabCloseRequested(int index)
{
    closeTab(index);
}

void MainWindow::onCurrentTabChanged(int index)
{
    Q_UNUSED(index);
    updateWindowTitle();
    CodeEditor *editor = currentEditor();
    if (editor) {
        onEditorCursorPositionChanged(editor->currentLine() + 1, editor->currentColumn() + 1);
    }
}

void MainWindow::onEditorModificationChanged(bool modified)
{
    Q_UNUSED(modified);
    updateWindowTitle();
}

void MainWindow::onEditorCursorPositionChanged(int line, int column)
{
    ui->statusBar->showMessage(tr("Line %1, Column %2").arg(line).arg(column));
}