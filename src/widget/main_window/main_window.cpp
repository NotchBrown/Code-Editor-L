#include "main.h"
#include "widget/main_window/main_window.h"
#include "ui_main_window.h"
#include "widget/editor/code_editor.h"
#include "widget/navigator/navigator.h"
#include "widget/segment/segment.h"
#include "widget/errors_and_warnings/errors_and_warnings.h"
#include "widget/ipc_message/ipc_message.h"
#include "ipc/ipc_server.h"
#include "project/project_manager.h"
#include "util/recent_files_manager.h"
#include <QVBoxLayout>
#include <QDockWidget>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_ipcServer(nullptr),
    m_projectManager(nullptr)
{
    ui->setupUi(this);
    
    // 初始化项目
    m_projectManager = new ProjectManager(this);
    
    // 设置 QTabWidget 为文档模式
    QTabWidget *tabWidget = new QTabWidget(this);
    tabWidget->setDocumentMode(true);
    tabWidget->setTabsClosable(true);
    tabWidget->setMovable(true);
    tabWidget->setStyleSheet("QTabWidget::pane { border: 0; } QTabBar::tab { margin: 2px; padding: 4px 8px; }");
    
    // 替换 placeholder 为 tabWidget - check for null pointer
    if (ui->editorPlaceholder && ui->editorPlaceholder->layout()) {
        ui->editorPlaceholder->layout()->addWidget(tabWidget);
    } else {
        // 如果没有 layout，创建一个
        QVBoxLayout *layout = new QVBoxLayout(ui->editorPlaceholder);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(tabWidget);
        ui->editorPlaceholder->setLayout(layout);
    }
    
    // 存储 tabWidget 以便后续访问
    tabWidget->setObjectName("mainTabWidget");
    
    // Create DockWidgets
    createDockWidgets();
    
    setupConnections();
    populateFileTypeMenu();
    populateRecentFilesMenu();
    
    // Connect recent files changed signal
    connect(RecentFilesManager::instance(), &RecentFilesManager::recentFilesChanged,
            this, &MainWindow::populateRecentFilesMenu);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createDockWidgets()
{
    // Navigator dock - Left
    QDockWidget *dockNavigator = new QDockWidget(tr("Navigator"), this);
    dockNavigator->setObjectName("dockNavigator");
    m_navigator = new Navigator();
    dockNavigator->setWidget(m_navigator);
    addDockWidget(Qt::LeftDockWidgetArea, dockNavigator);
    
    // Segment dock - Right
    QDockWidget *dockSegment = new QDockWidget(tr("Segment"), this);
    dockSegment->setObjectName("dockSegment");
    m_segment = new Segment();
    dockSegment->setWidget(m_segment);
    addDockWidget(Qt::RightDockWidgetArea, dockSegment);
    
    // Errors and Warnings dock - Bottom
    QDockWidget *dockErrorsWarnings = new QDockWidget(tr("Errors and Warnings"), this);
    dockErrorsWarnings->setObjectName("dockErrorsWarnings");
    m_errorsWarnings = new ErrorsAndWarnings();
    dockErrorsWarnings->setWidget(m_errorsWarnings);
    addDockWidget(Qt::BottomDockWidgetArea, dockErrorsWarnings);
    
    // IPC Message dock - Bottom (tabbed with Errors and Warnings)
    QDockWidget *dockIPCMessage = new QDockWidget(tr("IPC Message"), this);
    dockIPCMessage->setObjectName("dockIPCMessage");
    m_ipcMessage = new IPCMessage();
    dockIPCMessage->setWidget(m_ipcMessage);
    tabifyDockWidget(dockErrorsWarnings, dockIPCMessage);
    
    // Hide docks by default
    dockNavigator->hide();
    dockSegment->hide();
    dockErrorsWarnings->hide();
    dockIPCMessage->hide();
}

void MainWindow::setupConnections()
{
    // Get the tab widget
    QTabWidget *tabWidget = findChild<QTabWidget*>("mainTabWidget");
    
    // Connect tab close requests - check for null pointer
    if (tabWidget) {
        connect(tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::onTabCloseRequested);
        connect(tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onCurrentTabChanged);
    }
    
    // Menu actions
    // Start menu
    if (ui->actionStartNewInstance)
        connect(ui->actionStartNewInstance, &QAction::triggered, this, &MainWindow::onStartNewInstance);
    if (ui->actionStartExit)
        connect(ui->actionStartExit, &QAction::triggered, this, &MainWindow::onStartExit);
    
    // File menu
    if (ui->actionFileNew)
        connect(ui->actionFileNew, &QAction::triggered, this, &MainWindow::onFileNew);
    if (ui->actionFileOpen)
        connect(ui->actionFileOpen, &QAction::triggered, this, &MainWindow::onFileOpen);
    if (ui->actionFileSave)
        connect(ui->actionFileSave, &QAction::triggered, this, &MainWindow::onFileSave);
    if (ui->actionFileSaveAs)
        connect(ui->actionFileSaveAs, &QAction::triggered, this, &MainWindow::onFileSaveAs);
    if (ui->actionFileProperties)
        connect(ui->actionFileProperties, &QAction::triggered, this, &MainWindow::onFileProperties);
    if (ui->actionFilePrint)
        connect(ui->actionFilePrint, &QAction::triggered, this, &MainWindow::onFilePrint);
    if (ui->actionFilePrintPreview)
        connect(ui->actionFilePrintPreview, &QAction::triggered, this, &MainWindow::onFilePrintPreview);
    
    // Edit menu
    if (ui->actionEditUndo)
        connect(ui->actionEditUndo, &QAction::triggered, this, &MainWindow::onEditUndo);
    if (ui->actionEditRedo)
        connect(ui->actionEditRedo, &QAction::triggered, this, &MainWindow::onEditRedo);
    if (ui->actionEditCut)
        connect(ui->actionEditCut, &QAction::triggered, this, &MainWindow::onEditCut);
    if (ui->actionEditCopy)
        connect(ui->actionEditCopy, &QAction::triggered, this, &MainWindow::onEditCopy);
    if (ui->actionEditPaste)
        connect(ui->actionEditPaste, &QAction::triggered, this, &MainWindow::onEditPaste);
    if (ui->actionEditDelete)
        connect(ui->actionEditDelete, &QAction::triggered, this, &MainWindow::onEditDelete);
    if (ui->actionEditGoLine)
        connect(ui->actionEditGoLine, &QAction::triggered, this, &MainWindow::onEditGoLine);
    if (ui->actionEditComment)
        connect(ui->actionEditComment, &QAction::triggered, this, &MainWindow::onEditComment);
    if (ui->actionEditUncomment)
        connect(ui->actionEditUncomment, &QAction::triggered, this, &MainWindow::onEditUncomment);
    if (ui->actionEditCommentSegment)
        connect(ui->actionEditCommentSegment, &QAction::triggered, this, &MainWindow::onEditCommentSegment);
    if (ui->actionEditUncommentSegment)
        connect(ui->actionEditUncommentSegment, &QAction::triggered, this, &MainWindow::onEditUncommentSegment);
    if (ui->actionEditFind)
        connect(ui->actionEditFind, &QAction::triggered, this, &MainWindow::onEditFind);
    if (ui->actionEditReplace)
        connect(ui->actionEditReplace, &QAction::triggered, this, &MainWindow::onEditReplace);
    
    // Mark menu
    if (ui->actionMarkGoError)
        connect(ui->actionMarkGoError, &QAction::triggered, this, &MainWindow::onMarkGoError);
    if (ui->actionMarkDeleteError)
        connect(ui->actionMarkDeleteError, &QAction::triggered, this, &MainWindow::onMarkDeleteError);
    if (ui->actionMarkCleanError)
        connect(ui->actionMarkCleanError, &QAction::triggered, this, &MainWindow::onMarkCleanError);
    if (ui->actionMarkGoBookmark)
        connect(ui->actionMarkGoBookmark, &QAction::triggered, this, &MainWindow::onMarkGoBookmark);
    if (ui->actionMarkCreateBookmark)
        connect(ui->actionMarkCreateBookmark, &QAction::triggered, this, &MainWindow::onMarkCreateBookmark);
    if (ui->actionMarkManageBookmark)
        connect(ui->actionMarkManageBookmark, &QAction::triggered, this, &MainWindow::onMarkManageBookmark);
    if (ui->actionMarkCreateBreak)
        connect(ui->actionMarkCreateBreak, &QAction::triggered, this, &MainWindow::onMarkCreateBreak);
    if (ui->actionMarkRemoveBreak)
        connect(ui->actionMarkRemoveBreak, &QAction::triggered, this, &MainWindow::onMarkRemoveBreak);
    
    // Settings menu
    if (ui->actionSettingsGeneral)
        connect(ui->actionSettingsGeneral, &QAction::triggered, this, &MainWindow::onSettingsGeneral);
    if (ui->actionSettingsEditor)
        connect(ui->actionSettingsEditor, &QAction::triggered, this, &MainWindow::onSettingsEditor);
    if (ui->actionSettingsIPC)
        connect(ui->actionSettingsIPC, &QAction::triggered, this, &MainWindow::onSettingsIPC);
    if (ui->actionSettingsToolbar)
        connect(ui->actionSettingsToolbar, &QAction::triggered, this, &MainWindow::onSettingsToolbar);
    if (ui->actionSettingsHotkey)
        connect(ui->actionSettingsHotkey, &QAction::triggered, this, &MainWindow::onSettingsHotkey);
    
    // Window menu
    if (ui->actionWindowCloseTab)
        connect(ui->actionWindowCloseTab, &QAction::triggered, this, &MainWindow::onWindowCloseTab);
    if (ui->actionWindowCloseAllSaved)
        connect(ui->actionWindowCloseAllSaved, &QAction::triggered, this, &MainWindow::onWindowCloseAllSaved);
    if (ui->actionWindowCloseAll)
        connect(ui->actionWindowCloseAll, &QAction::triggered, this, &MainWindow::onWindowCloseAll);
    if (ui->actionWindowCloseOther)
        connect(ui->actionWindowCloseOther, &QAction::triggered, this, &MainWindow::onWindowCloseOther);
    
    // Dock widgets
    if (ui->actionWindowNavigator)
        connect(ui->actionWindowNavigator, &QAction::triggered, this, &MainWindow::onWindowNavigator);
    if (ui->actionWindowSegment)
        connect(ui->actionWindowSegment, &QAction::triggered, this, &MainWindow::onWindowSegment);
    if (ui->actionWindowErrorsWarnings)
        connect(ui->actionWindowErrorsWarnings, &QAction::triggered, this, &MainWindow::onWindowErrorsWarnings);
    if (ui->actionWindowIPCMessage)
        connect(ui->actionWindowIPCMessage, &QAction::triggered, this, &MainWindow::onWindowIPCMessage);
    
    // Help menu
    if (ui->actionHelpAbout)
        connect(ui->actionHelpAbout, &QAction::triggered, this, &MainWindow::onHelpAbout);
    if (ui->actionHelpHelp)
        connect(ui->actionHelpHelp, &QAction::triggered, this, &MainWindow::onHelpHelp);
    
    // Editor signals
    connect(this, &MainWindow::editorModificationChanged, this, &MainWindow::onEditorModificationChanged);
    connect(this, &MainWindow::editor_cursorPositionChanged, this, &MainWindow::onEditorCursorPositionChanged);
}

void MainWindow::initIpcServer(int port)
{
    m_ipcServer = new IpcServer(this);
    if (m_ipcServer->start(port)) {
        qDebug() << "IPC server started successfully on port" << port;
    }
}

CodeEditor* MainWindow::createNewEditor(const QString &filePath)
{
    QTabWidget *tabWidget = findChild<QTabWidget*>("mainTabWidget");
    
    CodeEditor *editor = new CodeEditor(this);
    
    // Connect editor signals
    connect(editor, &CodeEditor::modificationChanged, this, [this, editor](bool m) {
        emit editorModificationChanged(editor, m);
    });
    connect(editor, &CodeEditor::cursorPositionChanged, this, [this, editor](int line, int col) {
        emit editor_cursorPositionChanged(editor, line, col);
    });
    
    int index = -1;
    if (filePath.isEmpty()) {
        // New untitled file
        index = tabWidget->addTab(editor, tr("Untitled"));
    } else {
        // Open existing file
        if (editor->loadFile(filePath)) {
            index = tabWidget->addTab(editor, QFileInfo(filePath).fileName());
            m_fileToTabs[filePath] = QPair<CodeEditor*, int>(editor, index);
        } else {
            delete editor;
            return nullptr;
        }
    }
    
    tabWidget->setCurrentIndex(index);
    updateWindowTitle();
    
    return editor;
}

CodeEditor* MainWindow::currentEditor() const
{
    QTabWidget *tabWidget = findChild<QTabWidget*>("mainTabWidget");
    return qobject_cast<CodeEditor*>(tabWidget->currentWidget());
}

void MainWindow::updateWindowTitle()
{
    CodeEditor *editor = currentEditor();
    QString title = "CodeEditorLite";
    
    if (editor) {
        if (editor->isModified()) {
            title.prepend("* ");
        }
        
        QString filePath = editor->filePath();
        if (!filePath.isEmpty()) {
            title += " - " + QFileInfo(filePath).fileName();
        }
    }
    
    setWindowTitle(title);
}

bool MainWindow::closeTab(int index)
{
    QTabWidget *tabWidget = findChild<QTabWidget*>("mainTabWidget");
    QWidget *widget = tabWidget->widget(index);
    CodeEditor *editor = qobject_cast<CodeEditor*>(widget);
    
    if (!editor) {
        return false;
    }
    
    if (editor->isModified()) {
        QString fileName = QFileInfo(editor->filePath()).fileName();
        if (fileName.isEmpty()) {
            fileName = "Untitled";
        }
        
        QMessageBox::StandardButton ret = QMessageBox::warning(
            this,
            tr("Unsaved Changes"),
            tr("Do you want to save changes to %1?").arg(fileName),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel
        );
        
        if (ret == QMessageBox::Cancel) {
            return false;
        } else if (ret == QMessageBox::Save) {
            if (!saveCurrentFile()) {
                return false;
            }
        }
    }
    
    // Remove from file-to-tabs mapping
    QString filePath = editor->filePath();
    if (!filePath.isEmpty() && m_fileToTabs.contains(filePath)) {
        m_fileToTabs.remove(filePath);
    }
    
    tabWidget->removeTab(index);
    delete editor;
    
    updateWindowTitle();
    return true;
}

bool MainWindow::saveCurrentFile()
{
    CodeEditor *editor = currentEditor();
    if (!editor) {
        return false;
    }
    
    if (editor->filePath().isEmpty()) {
        return saveFileAs();
    } else {
        return editor->saveFile();
    }
}

bool MainWindow::saveFileAs()
{
    CodeEditor *editor = currentEditor();
    if (!editor) {
        return false;
    }
    
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save File"), "", 
        tr("All Files (*);;C/C++ Files (*.cpp *.h *.hpp *.cxx);;Python Files (*.py);;HTML Files (*.html *.htm);;JavaScript Files (*.js);;JSON Files (*.json);;XML Files (*.xml);;SQL Files (*.sql);;Lua Files (*.lua);;Shell Files (*.sh *.bash)"));
    
    if (!filePath.isEmpty()) {
        if (editor->saveFile(filePath)) {
            QTabWidget *tabWidget = findChild<QTabWidget*>("mainTabWidget");
            int index = tabWidget->currentIndex();
            tabWidget->setTabText(index, QFileInfo(filePath).fileName());
            
            m_fileToTabs[filePath] = QPair<CodeEditor*, int>(editor, index);
            updateWindowTitle();
            return true;
        }
    }
    
    return false;
}

QMap<QString, QString> MainWindow::getSupportedLexers() const
{
    // Return a map of display name -> lexer name
    // Sorted alphabetically by display name
    QMap<QString, QString> lexers;
    
    lexers["AVS"] = "avs";
    lexers["Bash"] = "bash";
    lexers["Batch"] = "batch";
    lexers["C#"] = "csharp";
    lexers["C/C++"] = "cpp";
    lexers["CMake"] = "cmake";
    lexers["CoffeeScript"] = "coffeescript";
    lexers["CSS"] = "css";
    lexers["D"] = "d";
    lexers["Diff"] = "diff";
    lexers["EDIFACT"] = "edifact";
    lexers["Fortran"] = "fortran";
    lexers["Fortran77"] = "fortran77";
    lexers["HTML"] = "html";
    lexers["IDL"] = "idl";
    lexers["Intel Hex"] = "intelhex";
    lexers["Java"] = "java";
    lexers["JavaScript"] = "javascript";
    lexers["JSON"] = "json";
    lexers["Lua"] = "lua";
    lexers["Makefile"] = "makefile";
    lexers["Markdown"] = "markdown";
    lexers["Matlab"] = "matlab";
    lexers["None"] = "none";
    lexers["Octave"] = "octave";
    lexers["Pascal"] = "pascal";
    lexers["Perl"] = "perl";
    lexers["PO"] = "po";
    lexers["PostScript"] = "postscript";
    lexers["POV"] = "pov";
    lexers["Properties"] = "properties";
    lexers["Python"] = "python";
    lexers["Ruby"] = "ruby";
    lexers["Spice"] = "spice";
    lexers["SQL"] = "sql";
    lexers["TCL"] = "tcl";
    lexers["TeX"] = "tex";
    lexers["VHDL"] = "vhdl";
    lexers["Verilog"] = "verilog";
    lexers["XML"] = "xml";
    lexers["YAML"] = "yaml";
    
    return lexers;
}

void MainWindow::populateFileTypeMenu()
{
    // Clear existing items
    ui->menuFileType->clear();
    m_lexerActions.clear();
    
    // Get all supported lexers
    QMap<QString, QString> lexers = getSupportedLexers();
    
    // Group by first letter: A-E, F-J, K-O, P-T, U-Z
    QStringList groups;
    groups << "A-E" << "F-J" << "K-O" << "P-T" << "U-Z";
    
    // Create submenus for each group
    QMap<QString, QMenu*> groupMenus;
    for (const QString &group : groups) {
        QMenu *menu = new QMenu(group, this);
        groupMenus[group] = menu;
    }
    
    // Add lexer actions to appropriate group
    for (auto it = lexers.constBegin(); it != lexers.constEnd(); ++it) {
        QString displayName = it.key();
        QString lexerName = it.value();
        
        // Determine which group this lexer belongs to
        QChar firstChar = displayName.at(0).toUpper();
        QString groupName;
        
        if (firstChar >= 'A' && firstChar <= 'E') {
            groupName = "A-E";
        } else if (firstChar >= 'F' && firstChar <= 'J') {
            groupName = "F-J";
        } else if (firstChar >= 'K' && firstChar <= 'O') {
            groupName = "K-O";
        } else if (firstChar >= 'P' && firstChar <= 'T') {
            groupName = "P-T";
        } else if (firstChar >= 'U' && firstChar <= 'Z') {
            groupName = "U-Z";
        } else {
            groupName = "A-E"; // Default to first group
        }
        
        // Create action
        QAction *action = new QAction(displayName, this);
        action->setCheckable(true);
        action->setChecked(false);
        
        // Connect to slot
        connect(action, &QAction::triggered, this, [this, lexerName]() {
            onFileTypeChanged(lexerName);
        });
        
        // Store action for later reference
        m_lexerActions[lexerName] = action;
        
        // Add to appropriate submenu
        groupMenus[groupName]->addAction(action);
    }
    
    // Add submenus to main Type menu
    for (const QString &group : groups) {
        ui->menuFileType->addMenu(groupMenus[group]);
    }
    
    // Update checked state
    updateFileTypeMenuChecked();
}

void MainWindow::updateFileTypeMenuChecked()
{
    // Get current editor
    CodeEditor *editor = currentEditor();
    QString currentLexer = "none";
    
    if (editor) {
        currentLexer = editor->currentLexerName();
    }
    
    // Update all actions
    for (auto it = m_lexerActions.constBegin(); it != m_lexerActions.constEnd(); ++it) {
        QString lexerName = it.key();
        QAction *action = it.value();
        
        action->setChecked(lexerName == currentLexer);
    }
}

void MainWindow::onFileTypeChanged(const QString &lexerName)
{
    // Get current editor
    CodeEditor *editor = currentEditor();
    if (!editor) {
        return;
    }
    
    // Set the lexer
    editor->setTempLexer(lexerName);
    
    // Update checked state
    updateFileTypeMenuChecked();
}

void MainWindow::updateOpenTabsMenu()
{
    ui->menuWindowTabs->clear();
    
    QTabWidget *tabWidget = findChild<QTabWidget*>("mainTabWidget");
    for (int i = 0; i < tabWidget->count(); ++i) {
        QAction *action = new QAction(tabWidget->tabText(i), this);
        action->setCheckable(true);
        action->setChecked(i == tabWidget->currentIndex());
        connect(action, &QAction::triggered, this, [this, i]() {
            QTabWidget *tabWidget = findChild<QTabWidget*>("mainTabWidget");
            tabWidget->setCurrentIndex(i);
        });
        ui->menuWindowTabs->addAction(action);
    }
}

// Slot implementations are in src/function/main_window_functions.cpp

void MainWindow::populateRecentFilesMenu()
{
    ui->menuFileRecent->clear();
    
    QStringList recentFiles = RecentFilesManager::instance()->recentFiles();
    
    if (recentFiles.isEmpty()) {
        QAction *action = new QAction("No recent files", this);
        action->setEnabled(false);
        ui->menuFileRecent->addAction(action);
        return;
    }
    
    int index = 1;
    for (const QString &filePath : recentFiles) {
        QString displayText = QString("%1. %2").arg(index).arg(QFileInfo(filePath).fileName());
        QAction *action = new QAction(displayText, this);
        
        connect(action, &QAction::triggered, this, [this, filePath]() {
            // Handle opening the recent file
            CodeEditor *editor = createNewEditor(filePath);
            if (editor) {
                // Add to recent files (to update timestamp)
                RecentFilesManager::instance()->addFile(filePath);
            }
        });
        
        ui->menuFileRecent->addAction(action);
        index++;
    }
    
    // Add separator and clear option
    ui->menuFileRecent->addSeparator();
    
    QAction *clearAction = new QAction("Clear Recent", this);
    connect(clearAction, &QAction::triggered, this, []() {
        RecentFilesManager::instance()->clear();
    });
    ui->menuFileRecent->addAction(clearAction);
}
