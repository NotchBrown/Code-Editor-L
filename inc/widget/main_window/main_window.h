#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "main.h"

class CodeEditor;
class IpcServer;
class ProjectManager;
class Navigator;
class Segment;
class ErrorsAndWarnings;
class IPCMessage;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void initIpcServer(int port = 0);

private slots:
    // Start menu
    void onStartNewInstance();
    void onStartExit();
    
    // File menu
    void onFileNew();
    void onFileOpen();
    void onFileSave();
    void onFileSaveAs();
    void onFileProperties();
    
    // Edit menu
    void onEditUndo();
    void onEditRedo();
    void onEditCut();
    void onEditCopy();
    void onEditPaste();
    void onEditDelete();
    void onEditGoLine();
    void onEditComment();
    void onEditUncomment();
    void onEditCommentSegment();
    void onEditUncommentSegment();
    void onEditFind();
    void onEditFindNext();
    void onEditFindPrev();
    void onEditReplace();
    
    // Mark menu
    void onMarkGoError();
    void onMarkDeleteError();
    void onMarkCleanError();
    void onMarkGoBookmark();
    void onMarkCreateBookmark();
    void onMarkManageBookmark();
    void onMarkCreateBreak();
    void onMarkRemoveBreak();
    
    // Settings menu
    void onSettingsGeneral();
    void onSettingsEditor();
    void onSettingsIPC();
    void onSettingsToolbar();
    
    // Window menu
    void onWindowCloseTab();
    void onWindowCloseAllSaved();
    void onWindowCloseAll();
    void onWindowCloseOther();
    void onWindowNavigator();
    void onWindowSegment();
    void onWindowErrorsWarnings();
    void onWindowIPCMessage();
    
    // View menu
    void onViewFullscreen();
    void onViewStatusbar();
    void onViewToolbar();
    
    // Help menu
    void onHelpAbout();
    void onHelpHelp();
    
    // Tab and editor events
    void onTabCloseRequested(int index);
    void onCurrentTabChanged(int index);
    void onEditorModificationChanged(CodeEditor *editor, bool modified);
    void onEditorCursorPositionChanged(CodeEditor *editor, int line, int column);

signals:
    void editorModificationChanged(CodeEditor *editor, bool modified);
    void editor_cursorPositionChanged(CodeEditor *editor, int line, int column);

private:
    void setupConnections();
    void createDockWidgets();
    
    CodeEditor* createNewEditor(const QString &filePath = QString());
    CodeEditor* currentEditor() const;
    void updateWindowTitle();
    bool closeTab(int index);
    bool saveCurrentFile();
    bool saveFileAs();
    void populateFileTypeMenu();
    void updateOpenTabsMenu();
    
    Ui::MainWindow *ui;
    IpcServer *m_ipcServer;
    ProjectManager *m_projectManager;
    
    // DockWidget instances
    Navigator *m_navigator;
    Segment *m_segment;
    ErrorsAndWarnings *m_errorsWarnings;
    IPCMessage *m_ipcMessage;
    
    QMap<QString, QPair<CodeEditor*, int>> m_fileToTabs;
};

#endif // MAIN_WINDOW_H
