#include "main.h"
#include "widget/bookmark/go_bookmark.h"
#include "ui_go_bookmark.h"
#include "widget/editor/code_editor.h"
#include <QHeaderView>
#include <QTabWidget>
#include <QFileInfo>

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

GoBookmark::GoBookmark(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::GoBookmark)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    ui->bookmarkTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->bookmarkTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->bookmarkTable->verticalHeader()->hide();

    setupConnections();
    populateFileCombo();
}

GoBookmark::~GoBookmark()
{
    delete ui;
}

// ---------------------------------------------------------------------------
// Signal connections
// ---------------------------------------------------------------------------

void GoBookmark::setupConnections()
{
    connect(ui->fileCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &GoBookmark::onFileComboChanged);
    connect(ui->goBtn, &QPushButton::clicked, this, &GoBookmark::onGoClicked);
    connect(ui->cancelBtn, &QPushButton::clicked, this, &GoBookmark::onCancelClicked);
    connect(ui->bookmarkTable, &QTableWidget::cellClicked,
            this, &GoBookmark::onTableClicked);
    connect(ui->bookmarkTable, &QTableWidget::cellDoubleClicked,
            this, &GoBookmark::onTableDoubleClicked);
}

// ---------------------------------------------------------------------------
// Populate file combo from parent's tab widget
// ---------------------------------------------------------------------------

void GoBookmark::populateFileCombo()
{
    ui->fileCombo->clear();

    QWidget *mainWindow = parentWidget();
    if (!mainWindow) return;

    QTabWidget *tabWidget = mainWindow->findChild<QTabWidget*>("mainTabWidget");
    if (!tabWidget) return;

    for (int i = 0; i < tabWidget->count(); ++i) {
        CodeEditor *editor = qobject_cast<CodeEditor*>(tabWidget->widget(i));
        if (!editor) continue;

        QString filePath = editor->filePath();
        QString label;
        if (filePath.isEmpty()) {
            label = tr("Untitled");
        } else {
            label = QFileInfo(filePath).fileName() + "  (" + filePath + ")";
        }
        ui->fileCombo->addItem(label, QVariant::fromValue(reinterpret_cast<quintptr>(editor)));
    }

    if (ui->fileCombo->count() > 0) {
        ui->fileCombo->setCurrentIndex(0);
    }
}

// ---------------------------------------------------------------------------
// Populate bookmark table for a given editor
// ---------------------------------------------------------------------------

void GoBookmark::populateBookmarkTable(CodeEditor *editor)
{
    ui->bookmarkTable->setRowCount(0);
    if (!editor) return;

    QList<int> lines = editor->allBookmarkLines();
    for (int line : lines) {
        int row = ui->bookmarkTable->rowCount();
        ui->bookmarkTable->insertRow(row);

        // Line number (1-based)
        QTableWidgetItem *lineItem = new QTableWidgetItem(QString::number(line + 1));
        lineItem->setTextAlignment(Qt::AlignCenter);
        lineItem->setData(Qt::UserRole, line);
        ui->bookmarkTable->setItem(row, 0, lineItem);

        // Line content (trimmed)
        QString content = editor->text(line).trimmed();
        if (content.length() > 120)
            content = content.left(120) + "...";
        QTableWidgetItem *contentItem = new QTableWidgetItem(content);
        contentItem->setToolTip(editor->text(line));
        ui->bookmarkTable->setItem(row, 1, contentItem);
    }
}

// ---------------------------------------------------------------------------
// Slots
// ---------------------------------------------------------------------------

void GoBookmark::onFileComboChanged(int index)
{
    if (index < 0) return;
    quintptr ptr = ui->fileCombo->itemData(index).value<quintptr>();
    CodeEditor *editor = reinterpret_cast<CodeEditor*>(ptr);
    populateBookmarkTable(editor);
}

void GoBookmark::onGoClicked()
{
    int row = ui->bookmarkTable->currentRow();
    if (row >= 0) {
        navigateToBookmark(row);
    }
}

void GoBookmark::onCancelClicked()
{
    reject();
}

void GoBookmark::onTableClicked(int row, int column)
{
    Q_UNUSED(column);
    navigateToBookmark(row);
}

void GoBookmark::onTableDoubleClicked(int row, int column)
{
    Q_UNUSED(column);
    navigateToBookmark(row);
}

void GoBookmark::navigateToBookmark(int row)
{
    if (row < 0 || row >= ui->bookmarkTable->rowCount()) return;

    QTableWidgetItem *lineItem = ui->bookmarkTable->item(row, 0);
    if (!lineItem) return;

    int line = lineItem->data(Qt::UserRole).toInt();

    int fileIndex = ui->fileCombo->currentIndex();
    if (fileIndex < 0) return;

    quintptr ptr = ui->fileCombo->itemData(fileIndex).value<quintptr>();
    CodeEditor *editor = reinterpret_cast<CodeEditor*>(ptr);
    if (!editor) return;

    editor->setCursorPosition(line, 0);
    editor->ensureLineVisible(line);
    editor->setFocus();
}
