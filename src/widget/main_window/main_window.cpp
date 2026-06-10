#include "widget/main_window/main_window.h"
#include "ui_main_window.h"
#include "widget/editor/code_editor.h"
#include "ipc/ipc_server.h"
#include "project/project_manager.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileInfo>
#include <QTabBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      m_ipcServer(nullptr),
      m_projectManager(nullptr)
{
    setupUi();
    setupActions();
    setupMenuBar();
    setupToolBar();
    setupStatusBar();
    setupTabWidget();
    
    createNewEditor();
}

MainWindow::~MainWindow()
{
    delete m_ipcServer;
    delete m_projectManager;
    delete ui;
}

void MainWindow::setupUi()
{
    ui->setupUi(this);
    setWindowTitle("CodeEditorLite");
    resize(1024, 768);
}

void MainWindow::setupActions()
{
    m_actionNew = new QAction(tr("&New"), this);
    m_actionNew->setShortcut(QKeySequence::New);
    connect(m_actionNew, &QAction::triggered, this, &MainWindow::onFileNew);
    
    m_actionOpen = new QAction(tr("&Open..."), this);
    m_actionOpen->setShortcut(QKeySequence::Open);
    connect(m_actionOpen, &QAction::triggered, this, &MainWindow::onFileOpen);
    
    m_actionSave = new QAction(tr("&Save"), this);
    m_actionSave->setShortcut(QKeySequence::Save);
    connect(m_actionSave, &QAction::triggered, this, &MainWindow::onFileSave);
    
    m_actionSaveAs = new QAction(tr("Save &As..."), this);
    m_actionSaveAs->setShortcut(QKeySequence::SaveAs);
    connect(m_actionSaveAs, &QAction::triggered, this, &MainWindow::onFileSaveAs);
    
    m_actionClose = new QAction(tr("&Close"), this);
    m_actionClose->setShortcut(QKeySequence::Close);
    connect(m_actionClose, &QAction::triggered, this, &MainWindow::onFileClose);
    
    m_actionExit = new QAction(tr("E&xit"), this);
    m_actionExit->setShortcut(QKeySequence::Quit);
    connect(m_actionExit, &QAction::triggered, this, &MainWindow::onFileExit);
    
    m_actionUndo = new QAction(tr("&Undo"), this);
    m_actionUndo->setShortcut(QKeySequence::Undo);
    connect(m_actionUndo, &QAction::triggered, this, &MainWindow::onEditUndo);
    
    m_actionRedo = new QAction(tr("&Redo"), this);
    m_actionRedo->setShortcut(QKeySequence::Redo);
    connect(m_actionRedo, &QAction::triggered, this, &MainWindow::onEditRedo);
    
    m_actionCut = new QAction(tr("Cu&t"), this);
    m_actionCut->setShortcut(QKeySequence::Cut);
    connect(m_actionCut, &QAction::triggered, this, &MainWindow::onEditCut);
    
    m_actionCopy = new QAction(tr("&Copy"), this);
    m_actionCopy->setShortcut(QKeySequence::Copy);
    connect(m_actionCopy, &QAction::triggered, this, &MainWindow::onEditCopy);
    
    m_actionPaste = new QAction(tr("&Paste"), this);
    m_actionPaste->setShortcut(QKeySequence::Paste);
    connect(m_actionPaste, &QAction::triggered, this, &MainWindow::onEditPaste);
    
    m_actionSelectAll = new QAction(tr("Select &All"), this);
    m_actionSelectAll->setShortcut(QKeySequence::SelectAll);
    connect(m_actionSelectAll, &QAction::triggered, this, &MainWindow::onEditSelectAll);
    
    m_actionFind = new QAction(tr("&Find..."), this);
    m_actionFind->setShortcut(QKeySequence::Find);
    connect(m_actionFind, &QAction::triggered, this, &MainWindow::onEditFind);
    
    m_actionReplace = new QAction(tr("&Replace..."), this);
    m_actionReplace->setShortcut(QKeySequence::Replace);
    connect(m_actionReplace, &QAction::triggered, this, &MainWindow::onEditReplace);
    
    m_actionToggleToolbar = new QAction(tr("&Toolbar"), this);
    m_actionToggleToolbar->setCheckable(true);
    m_actionToggleToolbar->setChecked(true);
    connect(m_actionToggleToolbar, &QAction::triggered, this, &MainWindow::onViewToggleToolbar);
    
    m_actionToggleStatusBar = new QAction(tr("&Status Bar"), this);
    m_actionToggleStatusBar->setCheckable(true);
    m_actionToggleStatusBar->setChecked(true);
    connect(m_actionToggleStatusBar, &QAction::triggered, this, &MainWindow::onViewToggleStatusBar);
    
    m_actionToggleLineNumbers = new QAction(tr("&Line Numbers"), this);
    m_actionToggleLineNumbers->setCheckable(true);
    m_actionToggleLineNumbers->setChecked(true);
    connect(m_actionToggleLineNumbers, &QAction::triggered, this, &MainWindow::onViewToggleLineNumbers);
    
    m_actionZoomIn = new QAction(tr("Zoom &In"), this);
    m_actionZoomIn->setShortcut(QKeySequence::ZoomIn);
    connect(m_actionZoomIn, &QAction::triggered, this, &MainWindow::onViewZoomIn);
    
    m_actionZoomOut = new QAction(tr("Zoom &Out"), this);
    m_actionZoomOut->setShortcut(QKeySequence::ZoomOut);
    connect(m_actionZoomOut, &QAction::triggered, this, &MainWindow::onViewZoomOut);
    
    m_actionGoToLine = new QAction(tr("&Go to Line..."), this);
    m_actionGoToLine->setShortcut(QKeySequence(tr("Ctrl+G")));
    connect(m_actionGoToLine, &QAction::triggered, this, &MainWindow::onNavigateGoToLine);
    
    m_actionNextTab = new QAction(tr("&Next Tab"), this);
    m_actionNextTab->setShortcut(QKeySequence::NextChild);
    connect(m_actionNextTab, &QAction::triggered, this, &MainWindow::onNavigateNextTab);
    
    m_actionPrevTab = new QAction(tr("&Previous Tab"), this);
    m_actionPrevTab->setShortcut(QKeySequence::PreviousChild);
    connect(m_actionPrevTab, &QAction::triggered, this, &MainWindow::onNavigatePrevTab);
    
    m_actionAbout = new QAction(tr("&About"), this);
    connect(m_actionAbout, &QAction::triggered, this, &MainWindow::onHelpAbout);
}

void MainWindow::setupMenuBar()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(m_actionNew);
    fileMenu->addAction(m_actionOpen);
    fileMenu->addSeparator();
    fileMenu->addAction(m_actionSave);
    fileMenu->addAction(m_actionSaveAs);
    fileMenu->addSeparator();
    fileMenu->addAction(m_actionClose);
    fileMenu->addAction(m_actionExit);
    
    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(m_actionUndo);
    editMenu->addAction(m_actionRedo);
    editMenu->addSeparator();
    editMenu->addAction(m_actionCut);
    editMenu->addAction(m_actionCopy);
    editMenu->addAction(m_actionPaste);
    editMenu->addSeparator();
    editMenu->addAction(m_actionSelectAll);
    editMenu->addSeparator();
    editMenu->addAction(m_actionFind);
    editMenu->addAction(m_actionReplace);
    
    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(m_actionToggleToolbar);
    viewMenu->addAction(m_actionToggleStatusBar);
    viewMenu->addSeparator();
    viewMenu->addAction(m_actionToggleLineNumbers);
    viewMenu->addSeparator();
    viewMenu->addAction(m_actionZoomIn);
    viewMenu->addAction(m_actionZoomOut);
    
    QMenu *navigateMenu = menuBar()->addMenu(tr("&Navigate"));
    navigateMenu->addAction(m_actionGoToLine);
    navigateMenu->addSeparator();
    navigateMenu->addAction(m_actionNextTab);
    navigateMenu->addAction(m_actionPrevTab);
    
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(m_actionAbout);
}

void MainWindow::setupToolBar()
{
    m_toolBar = addToolBar(tr("Main Toolbar"));
    m_toolBar->addAction(m_actionNew);
    m_toolBar->addAction(m_actionOpen);
    m_toolBar->addAction(m_actionSave);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_actionUndo);
    m_toolBar->addAction(m_actionRedo);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_actionCut);
    m_toolBar->addAction(m_actionCopy);
    m_toolBar->addAction(m_actionPaste);
}

void MainWindow::setupStatusBar()
{
    m_statusBar = statusBar();
    m_statusBar->showMessage(tr("Ready"));
}

void MainWindow::setupTabWidget()
{
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setTabsClosable(true);
    m_tabWidget->setMovable(true);
    setCentralWidget(m_tabWidget);
    
    connect(m_tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::onTabCloseRequested);
    connect(m_tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onCurrentTabChanged);
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
            m_fileToTabIndex[filePath] = m_tabWidget->count();
            m_tabWidget->addTab(editor, info.fileName());
        }
    } else {
        static int untitledCount = 1;
        QString title = tr("Untitled %1").arg(untitledCount++);
        m_tabWidget->addTab(editor, title);
    }
    
    connect(editor, &CodeEditor::modificationChanged, this, &MainWindow::onEditorModificationChanged);
    connect(editor, &CodeEditor::cursorPositionChanged, this, &MainWindow::onEditorCursorPositionChanged);
    
    m_tabWidget->setCurrentWidget(editor);
    return editor;
}

CodeEditor* MainWindow::currentEditor() const
{
    return qobject_cast<CodeEditor*>(m_tabWidget->currentWidget());
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
            title = QString("%1 - %2").arg(m_tabWidget->tabText(m_tabWidget->currentIndex())).arg(title);
        }
        if (editor->isModified()) {
            title += "*";
        }
        setWindowTitle(title);
    }
}

bool MainWindow::closeTab(int index)
{
    CodeEditor *editor = qobject_cast<CodeEditor*>(m_tabWidget->widget(index));
    if (!editor) return true;
    
    if (editor->isModified()) {
        QString fileName = editor->filePath().isEmpty() 
            ? m_tabWidget->tabText(index) 
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
    
    m_tabWidget->removeTab(index);
    delete editor;
    
    if (m_tabWidget->count() == 0) {
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
        m_fileToTabIndex[filePath] = m_tabWidget->currentIndex();
        m_tabWidget->setTabText(m_tabWidget->currentIndex(), info.fileName());
    }
    
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.write(editor->text().toUtf8());
        editor->setModified(false);
        file.close();
        m_statusBar->showMessage(tr("Saved: %1").arg(filePath));
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
    m_statusBar->showMessage(tr("New file created"));
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
            m_tabWidget->setCurrentIndex(m_fileToTabIndex[fileName]);
            m_statusBar->showMessage(tr("Already open: %1").arg(fileName));
            return;
        }
        
        createNewEditor(fileName);
        updateWindowTitle();
        m_statusBar->showMessage(tr("Opened: %1").arg(fileName));
    }
}

void MainWindow::onFileSave()
{
    if (saveCurrentFile()) {
        m_statusBar->showMessage(tr("File saved"));
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
        m_fileToTabIndex[filePath] = m_tabWidget->currentIndex();
        m_tabWidget->setTabText(m_tabWidget->currentIndex(), info.fileName());
        
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            file.write(editor->text().toUtf8());
            editor->setModified(false);
            file.close();
            updateWindowTitle();
            m_statusBar->showMessage(tr("Saved as: %1").arg(filePath));
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Cannot save file"));
        }
    }
}

void MainWindow::onFileClose()
{
    closeTab(m_tabWidget->currentIndex());
}

void MainWindow::onFileExit()
{
    while (m_tabWidget->count() > 0) {
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

void MainWindow::onEditSelectAll()
{
    CodeEditor *editor = currentEditor();
    if (editor) editor->selectAll();
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

void MainWindow::onViewToggleToolbar()
{
    m_toolBar->setVisible(m_actionToggleToolbar->isChecked());
}

void MainWindow::onViewToggleStatusBar()
{
    m_statusBar->setVisible(m_actionToggleStatusBar->isChecked());
}

void MainWindow::onViewToggleLineNumbers()
{
    CodeEditor *editor = currentEditor();
    if (editor) {
        editor->setMarginWidth(0, m_actionToggleLineNumbers->isChecked() ? 50 : 0);
    }
}

void MainWindow::onViewZoomIn()
{
    CodeEditor *editor = currentEditor();
    if (editor) {
        editor->zoomIn();
    }
}

void MainWindow::onViewZoomOut()
{
    CodeEditor *editor = currentEditor();
    if (editor) {
        editor->zoomOut();
    }
}

void MainWindow::onNavigateGoToLine()
{
    bool ok;
    int line = QInputDialog::getInt(this, tr("Go to Line"), tr("Line number:"), 1, 1, INT_MAX, 1, &ok);
    if (ok) {
        CodeEditor *editor = currentEditor();
        if (editor) editor->goToLine(line);
    }
}

void MainWindow::onNavigateNextTab()
{
    int current = m_tabWidget->currentIndex();
    int next = (current + 1) % m_tabWidget->count();
    m_tabWidget->setCurrentIndex(next);
}

void MainWindow::onNavigatePrevTab()
{
    int current = m_tabWidget->currentIndex();
    int prev = (current - 1 + m_tabWidget->count()) % m_tabWidget->count();
    m_tabWidget->setCurrentIndex(prev);
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
    m_statusBar->showMessage(tr("Line %1, Column %2").arg(line).arg(column));
}