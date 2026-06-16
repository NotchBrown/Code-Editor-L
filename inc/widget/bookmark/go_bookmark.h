#ifndef GO_BOOKMARK_H
#define GO_BOOKMARK_H

#include "main.h"

class CodeEditor;
class QTableWidgetItem;

namespace Ui {
class GoBookmark;
}

class GoBookmark : public QDialog
{
    Q_OBJECT

public:
    explicit GoBookmark(QWidget *parent = nullptr);
    ~GoBookmark();

private slots:
    void onFileComboChanged(int index);
    void onGoClicked();
    void onCancelClicked();
    void onTableClicked(int row, int column);
    void onTableDoubleClicked(int row, int column);

private:
    void setupUI();
    void setupConnections();
    void populateFileCombo();
    void populateBookmarkTable(CodeEditor *editor);
    void navigateToBookmark(int row);

    Ui::GoBookmark *ui;
};

#endif // GO_BOOKMARK_H
