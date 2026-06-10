#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QToolBar>
#include <QStatusBar>
#include <QMenuBar>
#include <QAction>
#include <QMap>
#include <QString>

class CodeEditor;
class IpcServer;
class ProjectManager;

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
    void onFileNew();
    void onFileOpen();
    void onFileSave();
    void onFileSaveAs();
    void onFileClose();
    void onFileExit();
    
    void onEditUndo();
    void onEditRedo();
    void onEditCut();
    void onEditCopy();
    void onEditPaste();
    void onEditSelectAll();
    void onEditFind();
    void onEditReplace();
    
    void onViewToggleToolbar();
    void onViewToggleStatusBar();
    void onViewToggleLineNumbers();
    void onViewZoomIn();
    void onViewZoomOut();
    
    void onNavigateGoToLine();
    void onNavigateNextTab();
    void onNavigatePrevTab();
    
    void onHelpAbout();
    
    void onTabCloseRequested(int index);
    void onCurrentTabChanged(int index);
    void onEditorModificationChanged(bool modified);
    void onEditorCursorPositionChanged(int line, int column);

private:
    void setupUi();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void setupTabWidget();
    void setupActions();
    
    CodeEditor* createNewEditor(const QString &filePath = QString());
    CodeEditor* currentEditor() const;
    void updateWindowTitle();
    bool closeTab(int index);
    bool saveCurrentFile();
    
    Ui::MainWindow *ui;
    QTabWidget *m_tabWidget;
    QToolBar *m_toolBar;
    QStatusBar *m_statusBar;
    IpcServer *m_ipcServer;
    ProjectManager *m_projectManager;
    
    QAction *m_actionNew;
    QAction *m_actionOpen;
    QAction *m_actionSave;
    QAction *m_actionSaveAs;
    QAction *m_actionClose;
    QAction *m_actionExit;
    
    QAction *m_actionUndo;
    QAction *m_actionRedo;
    QAction *m_actionCut;
    QAction *m_actionCopy;
    QAction *m_actionPaste;
    QAction *m_actionSelectAll;
    QAction *m_actionFind;
    QAction *m_actionReplace;
    
    QAction *m_actionToggleToolbar;
    QAction *m_actionToggleStatusBar;
    QAction *m_actionToggleLineNumbers;
    QAction *m_actionZoomIn;
    QAction *m_actionZoomOut;
    
    QAction *m_actionGoToLine;
    QAction *m_actionNextTab;
    QAction *m_actionPrevTab;
    
    QAction *m_actionAbout;
    
    QMap<QString, int> m_fileToTabIndex;
};

#endif // MAIN_WINDOW_H