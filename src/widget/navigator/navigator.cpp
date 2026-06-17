#include "main.h"
#include "widget/navigator/navigator.h"
#include "widget/editor/code_editor.h"
#include "ui_navigator.h"
#include <QHeaderView>
#include <QDebug>

Navigator::Navigator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Navigator)
{
    ui->setupUi(this);

    ui->treeWidget->setColumnWidth(0, 180);
    ui->treeWidget->header()->setStretchLastSection(false);
    ui->treeWidget->header()->setSectionResizeMode(0, QHeaderView::Stretch);

    connect(ui->treeWidget, &QTreeWidget::itemClicked,
            this, &Navigator::onItemClicked);
}

Navigator::~Navigator()
{
    delete ui;
}

void Navigator::clearOutline()
{
    ui->treeWidget->clear();
    m_groups.clear();
}

void Navigator::addSymbolGroup(const QString &groupName, SymbolInfo::Type type)
{
    QTreeWidgetItem *group = new QTreeWidgetItem(ui->treeWidget);
    group->setText(0, groupName);
    group->setText(1, QString());
    group->setData(0, Qt::UserRole, "group");
    group->setExpanded(true);
    m_groups[type] = group;
}

void Navigator::addSymbolItem(QTreeWidgetItem *group, const SymbolInfo &sym)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(group);
    item->setText(0, sym.name);
    item->setText(1, QString::number(sym.line + 1));  // 1-based display
    item->setData(0, Qt::UserRole, "symbol");
    item->setData(0, Qt::UserRole + 1, sym.line);
    item->setData(0, Qt::UserRole + 2, sym.startByte);
    item->setData(0, Qt::UserRole + 3, sym.endByte);
}

void Navigator::updateOutline(CodeEditor *editor)
{
    clearOutline();
    if (!editor) {
        qDebug() << "[Navigator] updateOutline: editor is null";
        return;
    }

    QString lexerName = editor->currentLexerName();
    QString source = editor->text();

    qDebug() << "[Navigator] updateOutline: lexer=" << lexerName
             << "sourceLen=" << source.length()
             << "supportsTS=" << TreeSitterManager::instance()->supportsLanguage(lexerName);

    TreeSitterManager *ts = TreeSitterManager::instance();

    // Use tree-sitter for supported languages, fallback to QScintilla
    QList<SymbolInfo> symbols;
    if (ts->supportsLanguage(lexerName)) {
        symbols = ts->parseSymbols(source, lexerName);
    } else {
        qDebug() << "[Navigator] fallback: using fold/indent heuristics for" << lexerName;
        // Fallback: use QScintilla via fold structure + basic line parsing
        // For unsupported languages, show fold regions
        int lineCount = editor->lines();
        for (int i = 0; i < lineCount; ++i) {
            QString lineText = editor->text(i).trimmed();
            if (lineText.isEmpty()) continue;

            // Simple heuristic: lines ending with { or : might be structure
            QChar last = lineText.isEmpty() ? QChar() : lineText.at(lineText.length() - 1);
            if (last == '{' || last == ':') {
                SymbolInfo info;
                info.type = SymbolInfo::Unknown;
                info.name = lineText.left(60);
                info.line = i;
                symbols.append(info);
            }
        }
    }

    qDebug() << "[Navigator] updateOutline: total symbols=" << symbols.size();

    // Build tree: group by type
    QMap<SymbolInfo::Type, QList<SymbolInfo>> grouped;
    for (const SymbolInfo &s : symbols) {
        grouped[s.type].append(s);
    }

    // Create groups in a defined order
    struct GroupDef { SymbolInfo::Type type; QString name; };
    GroupDef order[] = {
        {SymbolInfo::Class,     "Classes"},
        {SymbolInfo::Struct,    "Structs"},
        {SymbolInfo::Enum,      "Enums"},
        {SymbolInfo::Union,     "Unions"},
        {SymbolInfo::Interface, "Interfaces"},
        {SymbolInfo::Namespace, "Namespaces"},
        {SymbolInfo::Function,  "Functions"},
        {SymbolInfo::Method,    "Methods"},
        {SymbolInfo::Variable,  "Variables"},
        {SymbolInfo::Typedef,   "Typedefs"},
        {SymbolInfo::Macro,     "Macros"},
        {SymbolInfo::Module,    "Modules"},
        {SymbolInfo::Unknown,   "Other"}
    };

    int totalItems = 0;
    for (const auto &gd : order) {
        if (grouped.contains(gd.type) && !grouped[gd.type].isEmpty()) {
            addSymbolGroup(gd.name, gd.type);
            QTreeWidgetItem *grp = m_groups[gd.type];
            for (const SymbolInfo &s : grouped[gd.type]) {
                addSymbolItem(grp, s);
                totalItems++;
            }
        }
    }
    qDebug() << "[Navigator] updateOutline: added" << totalItems << "tree items";
}

void Navigator::onItemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    if (!item) return;

    QString role = item->data(0, Qt::UserRole).toString();
    if (role != "symbol") return;

    int line = item->data(0, Qt::UserRole + 1).toInt();

    // Find the parent MainWindow and navigate
    QWidget *p = parentWidget();
    while (p) {
        QMainWindow *mw = qobject_cast<QMainWindow*>(p);
        if (mw) {
            // Find the editor via MainWindow's tab widget
            QTabWidget *tabs = mw->findChild<QTabWidget*>("mainTabWidget");
            if (tabs) {
                CodeEditor *editor = qobject_cast<CodeEditor*>(tabs->currentWidget());
                if (editor) {
                    editor->setCursorPosition(line, 0);
                    editor->ensureLineVisible(line);
                    editor->setFocus();
                }
            }
            break;
        }
        p = p->parentWidget();
    }
}