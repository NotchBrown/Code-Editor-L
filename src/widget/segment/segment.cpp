#include "main.h"
#include "widget/segment/segment.h"
#include "widget/editor/code_editor.h"
#include "ui_segment.h"
#include <QHeaderView>

Segment::Segment(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Segment)
{
    ui->setupUi(this);

    ui->infoTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->infoTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
}

Segment::~Segment()
{
    delete ui;
}

void Segment::clearInfo()
{
    ui->infoTable->setRowCount(0);
}

void Segment::addInfoRow(const QString &label, const QString &value)
{
    int row = ui->infoTable->rowCount();
    ui->infoTable->insertRow(row);

    QTableWidgetItem *labelItem = new QTableWidgetItem(label);
    labelItem->setFont(QFont(ui->infoTable->font().family(), -1, QFont::Bold));
    ui->infoTable->setItem(row, 0, labelItem);

    QTableWidgetItem *valueItem = new QTableWidgetItem(value);
    valueItem->setToolTip(value);
    ui->infoTable->setItem(row, 1, valueItem);
}

void Segment::updateAtPosition(CodeEditor *editor, int line, int column)
{
    clearInfo();
    if (!editor) return;

    QString lexerName = editor->currentLexerName();
    QString source = editor->text();

    // Basic info always shown
    addInfoRow("Line", QString::number(line + 1));
    addInfoRow("Column", QString::number(column + 1));
    addInfoRow("Lexer", lexerName.isEmpty() ? "none" : lexerName);

    // Get file path
    QString filePath = editor->filePath();
    if (!filePath.isEmpty()) {
        QFileInfo fi(filePath);
        addInfoRow("File", fi.fileName());
    }

    // Tree-sitter segment analysis (only for supported languages)
    TreeSitterManager *ts = TreeSitterManager::instance();
    if (ts->supportsLanguage(lexerName)) {
        // Calculate byte offset from line/column
        int byteOffset = 0;
        int totalLines = source.count('\n') + 1;
        int currentLine = 0;
        for (int i = 0; i < source.length() && currentLine < line; ++i) {
            if (source[i] == '\n') currentLine++;
            byteOffset = i + 1;
        }
        byteOffset += column;

        SegmentInfo seg = ts->getSegmentAt(source, lexerName, byteOffset);
        if (!seg.typeName.isEmpty()) {
            addInfoRow("Context", seg.typeName);

            // Line range
            addInfoRow("Range Lines",
                       QString("%1 - %2").arg(seg.startLine + 1).arg(seg.endLine + 1));

            // Summary (first line of the segment)
            if (!seg.summary.isEmpty()) {
                addInfoRow("Summary", seg.summary);
            }

            // Properties from tree-sitter analysis
            for (const auto &prop : seg.properties) {
                addInfoRow(prop.first, prop.second);
            }
        }

        // Check for parse errors near cursor
        QList<SegmentInfo> errors = ts->getErrorNodes(source, lexerName);
        for (const SegmentInfo &err : errors) {
            if (err.startLine <= line && line <= err.endLine) {
                addInfoRow("Parse Error", err.summary);
                for (const auto &prop : err.properties) {
                    addInfoRow(prop.first, prop.second);
                }
                break;
            }
        }
    } else {
        // Fallback: basic info using QScintilla fold info
        int foldLevel = editor->SendScintilla(2423, line);  // SCI_GETFOLDLEVEL
        if (foldLevel & 0x400) {  // SC_FOLDLEVELHEADERFLAG
            addInfoRow("Fold", "Collapsible block header");
        }
        // Show indentation level
        int indent = editor->indentation(line);
        addInfoRow("Indent Level", QString::number(indent / editor->tabWidth()));
    }
}