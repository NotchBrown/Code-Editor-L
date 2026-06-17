#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include "main.h"
#include "util/tree_sitter_manager.h"
#include <QTreeWidgetItem>

class CodeEditor;

namespace Ui {
class Navigator;
}

class Navigator : public QWidget
{
    Q_OBJECT

public:
    explicit Navigator(QWidget *parent = nullptr);
    ~Navigator();

    // Update the outline from the current editor content
    void updateOutline(CodeEditor *editor);

private slots:
    void onItemClicked(QTreeWidgetItem *item, int column);

private:
    void clearOutline();
    void addSymbolGroup(const QString &groupName, SymbolInfo::Type type);
    void addSymbolItem(QTreeWidgetItem *group, const SymbolInfo &sym);

    Ui::Navigator *ui;

    // Cache group items for quick access
    QMap<SymbolInfo::Type, QTreeWidgetItem*> m_groups;
};

#endif // NAVIGATOR_H