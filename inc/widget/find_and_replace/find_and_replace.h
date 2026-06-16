#ifndef FIND_AND_REPLACE_H
#define FIND_AND_REPLACE_H

#include "main.h"

class CodeEditor;
class QTableWidgetItem;

namespace Ui {
class FindAndReplace;
}

class FindAndReplace : public QWidget
{
    Q_OBJECT

public:
    explicit FindAndReplace(QWidget *parent = nullptr);
    ~FindAndReplace();

    void setEditor(CodeEditor *editor);
    CodeEditor *editor() const;
    void setAllEditors(const QList<CodeEditor*> &editors);

    // Activation modes
    void activateFindMode();
    void activateReplaceMode();
    void focusOnSearch();
    void setSearchText(const QString &text);

signals:
    void searchProgress(int current, int total, const QString &editorName);
    void searchFinished(int totalResults);

public slots:
    void onFind();
    void onFindNext();
    void onFindPrev();
    void onFindSelection();
    void onFindInCurrent();
    void onFindAll();
    void onReplace();
    void onReplaceAll();
    void clearResults();

private slots:
    void onSearchTextChanged(const QString &text);
    void onResultTableClicked(int row, int column);
    void onResultTableItemClicked(QTableWidgetItem *item);
    void onResultTableDoubleClicked(int row, int column);
    void onResultTableItemDoubleClicked(QTableWidgetItem *item);

private:
    void connectSignals();
    void addResultContext(int selStart, int selEnd, int line, int col, CodeEditor *editor);
    void navigateToResult(int row);
    void performSearch(const QString &text, bool caseSensitive, bool wrap, bool forward);
    void performSearchAll(const QString &text, bool caseSensitive);
    void performSearchAllInEditor(const QString &text, bool caseSensitive, CodeEditor *editor);
    void performReplace(const QString &find, const QString &replace);
    void performReplaceAll(const QString &find, const QString &replace);

    Ui::FindAndReplace *ui;
    CodeEditor *m_editor;
    QList<CodeEditor*> m_allEditors;
    bool m_replaceEnabled;
    bool m_searchAborted;
};

#endif // FIND_AND_REPLACE_H
