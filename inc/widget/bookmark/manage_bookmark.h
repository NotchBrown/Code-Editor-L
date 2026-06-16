#ifndef MANAGE_BOOKMARK_H
#define MANAGE_BOOKMARK_H

#include "main.h"

class CodeEditor;
class QTableWidgetItem;

namespace Ui {
class ManageBookmark;
}

class ManageBookmark : public QDialog
{
    Q_OBJECT

public:
    explicit ManageBookmark(QWidget *parent = nullptr);
    ~ManageBookmark();

    // Bookmark entry data for import/export
    struct BookmarkEntry {
        QString filePath;
        int     lineNumber;
        QString lineText;
    };

private slots:
    void onFileComboChanged(int index);
    void onDeleteClicked();
    void onDeleteAllClicked();
    void onImportClicked();
    void onExportClicked();
    void onCloseClicked();
    void onTableClicked(int row, int column);
    void onTableDoubleClicked(int row, int column);

private:
    void setupUI();
    void setupConnections();
    void populateFileCombo();
    void populateBookmarkTable(CodeEditor *editor);
    void removeBookmarkRow(int row);
    CodeEditor* editorForFile(const QString &filePath) const;
    QString getDataFilePath() const;

    Ui::ManageBookmark *ui;
};

#endif // MANAGE_BOOKMARK_H
