#include "main.h"
#include "widget/navigator/navigator.h"
#include "widget/editor/code_editor.h"
#include "ui_navigator.h"
#include "component/addon_manager.h"
#include <QHeaderView>
#include <QDebug>
#include <QRegularExpression>

// ---------------------------------------------------------------------------
// Language-specific fallback symbol extraction (when no addon component)
// ---------------------------------------------------------------------------

static void fallbackSymbolsForLanguage(const QString &lexerName,
                                        CodeEditor *editor,
                                        QList<SymbolInfo> &symbols)
{
    int lineCount = editor->lines();
    for (int i = 0; i < lineCount; ++i) {
        QString raw = editor->text(i);
        QString trimmed = raw.trimmed();
        if (trimmed.isEmpty()) continue;
        QChar last = trimmed.at(trimmed.length() - 1);

        if (lexerName == "ruby") {
            QRegularExpression rx(R"(^\s*(def|class|module)\s+(\S+))");
            auto m = rx.match(raw);
            if (m.hasMatch()) {
                SymbolInfo info;
                QString kw = m.captured(1);
                info.name = m.captured(2); info.line = i;
                info.type = (kw == "class") ? SymbolInfo::Class
                          : (kw == "module") ? SymbolInfo::Namespace
                          : SymbolInfo::Function;
                symbols.append(info); continue;
            }
        }
        if (lexerName == "python") {
            QRegularExpression rx(R"(^(\s*)(def|class)\s+(\S+))");
            auto m = rx.match(raw);
            if (m.hasMatch() && m.captured(1).length() < 4) {
                SymbolInfo info;
                info.name = m.captured(3); info.line = i;
                info.type = (m.captured(2) == "class") ? SymbolInfo::Class : SymbolInfo::Function;
                symbols.append(info); continue;
            }
        }
        if (lexerName == "fortran" || lexerName == "fortran77") {
            QRegularExpression rx(R"(^\s*(subroutine|function|module|program)\s+(\S+))",
                                  QRegularExpression::CaseInsensitiveOption);
            auto m = rx.match(raw);
            if (m.hasMatch()) {
                SymbolInfo info;
                info.name = m.captured(2); info.line = i;
                QString kw = m.captured(1).toLower();
                info.type = (kw == "module" || kw == "program") ? SymbolInfo::Namespace : SymbolInfo::Function;
                symbols.append(info); continue;
            }
        }
        if (lexerName == "verilog") {
            QRegularExpression rx(R"(^\s*(module|function|task)\s+(\S+))",
                                  QRegularExpression::CaseInsensitiveOption);
            auto m = rx.match(raw);
            if (m.hasMatch()) {
                SymbolInfo info;
                info.name = m.captured(2); info.line = i;
                info.type = (m.captured(1).toLower() == "module") ? SymbolInfo::Class : SymbolInfo::Function;
                symbols.append(info); continue;
            }
        }
        if (lexerName == "makefile") {
            QRegularExpression rx(R"(^([a-zA-Z_][a-zA-Z0-9_.-]*)\s*:)", QRegularExpression::MultilineOption);
            auto m = rx.match(raw);
            if (m.hasMatch() && raw.contains(':')) {
                SymbolInfo info;
                info.name = m.captured(1); info.line = i;
                info.type = SymbolInfo::Function;
                symbols.append(info); continue;
            }
        }
        if (lexerName == "html") {
            QRegularExpression rx(R"(<(\w+)[^>]*(id|class)\s*=\s*["']([^"']+)["'][^>]*>)");
            auto m = rx.match(trimmed);
            if (m.hasMatch()) {
                SymbolInfo info;
                info.name = m.captured(1) + "#" + m.captured(3);
                info.line = i; info.type = SymbolInfo::Unknown;
                symbols.append(info); continue;
            }
        }
        // C-like fallback: { } blocks
        if (last == '{') {
            QString beforeBrace = trimmed.left(trimmed.length() - 1).trimmed();
            QRegularExpression rx2(R"(^\s*(class|struct|enum|union|interface|namespace)\s+(\w+))");
            auto m2 = rx2.match(trimmed);
            if (m2.hasMatch()) {
                SymbolInfo info;
                info.name = m2.captured(2); info.line = i;
                QString kw = m2.captured(1);
                if (kw == "class") info.type = SymbolInfo::Class;
                else if (kw == "struct") info.type = SymbolInfo::Struct;
                else if (kw == "enum") info.type = SymbolInfo::Enum;
                else if (kw == "union") info.type = SymbolInfo::Union;
                else if (kw == "interface") info.type = SymbolInfo::Interface;
                else if (kw == "namespace") info.type = SymbolInfo::Namespace;
                else info.type = SymbolInfo::Unknown;
                symbols.append(info); continue;
            }
            QRegularExpression rx3(R"((\w+)\s*\([^)]*\)\s*)");
            auto m3 = rx3.match(beforeBrace);
            if (m3.hasMatch()) {
                SymbolInfo info;
                info.name = m3.captured(1); info.line = i;
                info.type = SymbolInfo::Function;
                symbols.append(info); continue;
            }
        }
    }
}

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
             << "sourceLen=" << source.length();

    // Try addon component first, fall back to QScintilla heuristics
    QList<SymbolInfo> symbols;
    ILanguageComponent *comp = AddonManager::instance()->componentForLanguage(lexerName);

    if (comp && (comp->capabilities() & CapSymbolOutline)) {
        symbols = comp->parseSymbols(source, lexerName);
        qDebug() << "[Navigator] used component:" << comp->componentName()
                 << "found" << symbols.size() << "symbols";
    } else {
        qDebug() << "[Navigator] fallback: QScintilla heuristics for" << lexerName;
        // QScintilla-based symbol extraction
        fallbackSymbolsForLanguage(lexerName, editor, symbols);
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