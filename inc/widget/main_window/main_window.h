#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
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
    void onFileExit();
    
    void onEditUndo();
    void onEditRedo();
    void onEditCut();
    void onEditCopy();
    void onEditPaste();
    void onEditFind();
    void onEditReplace();
    
    void onViewZoomIn();
    void onViewZoomOut();
    void onViewResetZoom();
    void onViewLineNumbers();
    void onViewFolding();
    
    void onToolsPreferences();
    void onHelpAbout();
    
    void onTabCloseRequested(int index);
    void onCurrentTabChanged(int index);
    void onEditorModificationChanged(bool modified);
    void onEditorCursorPositionChanged(int line, int column);

private:
    void setupConnections();
    
    CodeEditor* createNewEditor(const QString &filePath = QString());
    CodeEditor* currentEditor() const;
    void updateWindowTitle();
    bool closeTab(int index);
    bool saveCurrentFile();
    
    Ui::MainWindow *ui;
    IpcServer *m_ipcServer;
    ProjectManager *m_projectManager;
    
    QMap<QString, int> m_fileToTabIndex;
};

#endif // MAIN_WINDOW_H