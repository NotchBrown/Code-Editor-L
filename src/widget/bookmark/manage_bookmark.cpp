#include "main.h"
#include "widget/bookmark/manage_bookmark.h"
#include "ui_manage_bookmark.h"
#include "widget/editor/code_editor.h"
#include <QHeaderView>
#include <QTabWidget>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

ManageBookmark::ManageBookmark(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::ManageBookmark)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    ui->bookmarkTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->bookmarkTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->bookmarkTable->verticalHeader()->hide();

    setupConnections();
    populateFileCombo();
}

ManageBookmark::~ManageBookmark()
{
    delete ui;
}

// ---------------------------------------------------------------------------
// Signal connections
// ---------------------------------------------------------------------------

void ManageBookmark::setupConnections()
{
    connect(ui->fileCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ManageBookmark::onFileComboChanged);
    connect(ui->deleteBtn, &QPushButton::clicked, this, &ManageBookmark::onDeleteClicked);
    connect(ui->deleteAllBtn, &QPushButton::clicked, this, &ManageBookmark::onDeleteAllClicked);
    connect(ui->importBtn, &QPushButton::clicked, this, &ManageBookmark::onImportClicked);
    connect(ui->exportBtn, &QPushButton::clicked, this, &ManageBookmark::onExportClicked);
    connect(ui->closeBtn, &QPushButton::clicked, this, &ManageBookmark::onCloseClicked);
    connect(ui->bookmarkTable, &QTableWidget::cellClicked,
            this, &ManageBookmark::onTableClicked);
    connect(ui->bookmarkTable, &QTableWidget::cellDoubleClicked,
            this, &ManageBookmark::onTableDoubleClicked);
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

QString ManageBookmark::getDataFilePath() const
{
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataPath);
    if (!dir.exists()) dir.mkpath(dataPath);
    return dataPath + "/bookmarks.xml";
}

CodeEditor* ManageBookmark::editorForFile(const QString &filePath) const
{
    QWidget *mainWindow = parentWidget();
    if (!mainWindow) return nullptr;

    QTabWidget *tabWidget = mainWindow->findChild<QTabWidget*>("mainTabWidget");
    if (!tabWidget) return nullptr;

    for (int i = 0; i < tabWidget->count(); ++i) {
        CodeEditor *editor = qobject_cast<CodeEditor*>(tabWidget->widget(i));
        if (editor && editor->filePath() == filePath) {
            return editor;
        }
    }
    return nullptr;
}

// ---------------------------------------------------------------------------
// Populate file combo
// ---------------------------------------------------------------------------

void ManageBookmark::populateFileCombo()
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
// Populate bookmark table
// ---------------------------------------------------------------------------

void ManageBookmark::populateBookmarkTable(CodeEditor *editor)
{
    ui->bookmarkTable->setRowCount(0);
    if (!editor) return;

    QList<int> lines = editor->allBookmarkLines();
    for (int line : lines) {
        int row = ui->bookmarkTable->rowCount();
        ui->bookmarkTable->insertRow(row);

        QTableWidgetItem *lineItem = new QTableWidgetItem(QString::number(line + 1));
        lineItem->setTextAlignment(Qt::AlignCenter);
        lineItem->setData(Qt::UserRole, line);
        ui->bookmarkTable->setItem(row, 0, lineItem);

        QString content = editor->text(line).trimmed();
        if (content.length() > 120)
            content = content.left(120) + "...";
        QTableWidgetItem *contentItem = new QTableWidgetItem(content);
        contentItem->setToolTip(editor->text(line));
        ui->bookmarkTable->setItem(row, 1, contentItem);
    }
}

void ManageBookmark::removeBookmarkRow(int row)
{
    ui->bookmarkTable->removeRow(row);
}

// ---------------------------------------------------------------------------
// Slots
// ---------------------------------------------------------------------------

void ManageBookmark::onFileComboChanged(int index)
{
    if (index < 0) return;
    quintptr ptr = ui->fileCombo->itemData(index).value<quintptr>();
    CodeEditor *editor = reinterpret_cast<CodeEditor*>(ptr);
    populateBookmarkTable(editor);
}

void ManageBookmark::onDeleteClicked()
{
    int row = ui->bookmarkTable->currentRow();
    if (row < 0) return;

    QTableWidgetItem *lineItem = ui->bookmarkTable->item(row, 0);
    if (!lineItem) return;

    int line = lineItem->data(Qt::UserRole).toInt();

    int fileIndex = ui->fileCombo->currentIndex();
    if (fileIndex < 0) return;

    quintptr ptr = ui->fileCombo->itemData(fileIndex).value<quintptr>();
    CodeEditor *editor = reinterpret_cast<CodeEditor*>(ptr);
    if (editor) {
        editor->markerDelete(line, 1);
    }
    removeBookmarkRow(row);
}

void ManageBookmark::onDeleteAllClicked()
{
    int fileIndex = ui->fileCombo->currentIndex();
    if (fileIndex < 0) return;

    quintptr ptr = ui->fileCombo->itemData(fileIndex).value<quintptr>();
    CodeEditor *editor = reinterpret_cast<CodeEditor*>(ptr);
    if (editor) {
        editor->clearAllBookmarks();
    }
    ui->bookmarkTable->setRowCount(0);
}

void ManageBookmark::onImportClicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Import Bookmarks"),
                                                     QString(), tr("Bookmark Files (*.xml);;All Files (*)"));
    if (filePath.isEmpty()) return;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Import Error"), tr("Cannot open file: %1").arg(filePath));
        return;
    }

    QList<BookmarkEntry> entries;
    QXmlStreamReader reader(&file);

    while (!reader.atEnd() && !reader.hasError()) {
        QXmlStreamReader::TokenType token = reader.readNext();
        if (token == QXmlStreamReader::StartDocument) continue;
        if (token == QXmlStreamReader::StartElement) {
            if (reader.name() == "bookmarks") continue;
            if (reader.name() == "bookmark") {
                BookmarkEntry entry;
                entry.filePath   = reader.attributes().value("file").toString();
                entry.lineNumber = reader.attributes().value("line").toInt();
                entry.lineText   = reader.attributes().value("text").toString();
                entries.append(entry);
                reader.readElementText(); // consume
            }
        }
    }
    file.close();

    if (reader.hasError()) {
        QMessageBox::warning(this, tr("Import Error"),
                             tr("Failed to parse bookmark file."));
        return;
    }

    if (entries.isEmpty()) {
        QMessageBox::information(this, tr("Import"), tr("No bookmarks found in file."));
        return;
    }

    // Apply bookmarks to open editors
    int applied = 0;
    int skipped = 0;
    for (const BookmarkEntry &entry : entries) {
        CodeEditor *editor = editorForFile(entry.filePath);
        if (!editor) {
            skipped++;
            continue;
        }
        // Check if line exists
        if (entry.lineNumber >= 0 && entry.lineNumber < editor->lines()) {
            editor->markerAdd(entry.lineNumber, 1);
            applied++;
        }
    }

    QString msg;
    if (skipped > 0) {
        msg = tr("Imported %1 bookmark(s). %2 bookmark(s) skipped because the file was not open.")
                  .arg(applied).arg(skipped);
        QMessageBox::information(this, tr("Import Result"), msg);
    } else {
        msg = tr("Successfully imported %1 bookmark(s).").arg(applied);
        QMessageBox::information(this, tr("Import Result"), msg);
    }

    // Refresh
    onFileComboChanged(ui->fileCombo->currentIndex());
}

void ManageBookmark::onExportClicked()
{
    // Collect all bookmarks from all open editors
    QList<BookmarkEntry> allEntries;

    QWidget *mainWindow = parentWidget();
    if (!mainWindow) return;

    QTabWidget *tabWidget = mainWindow->findChild<QTabWidget*>("mainTabWidget");
    if (!tabWidget) return;

    for (int i = 0; i < tabWidget->count(); ++i) {
        CodeEditor *editor = qobject_cast<CodeEditor*>(tabWidget->widget(i));
        if (!editor) continue;

        QString filePath = editor->filePath();
        if (filePath.isEmpty()) continue;

        QList<int> lines = editor->allBookmarkLines();
        for (int line : lines) {
            BookmarkEntry entry;
            entry.filePath   = filePath;
            entry.lineNumber = line;
            entry.lineText   = editor->text(line).trimmed();
            allEntries.append(entry);
        }
    }

    if (allEntries.isEmpty()) {
        QMessageBox::information(this, tr("Export"), tr("No bookmarks to export."));
        return;
    }

    QString filePath = QFileDialog::getSaveFileName(this, tr("Export Bookmarks"),
                                                     QString(), tr("Bookmark Files (*.xml);;All Files (*)"));
    if (filePath.isEmpty()) return;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Export Error"), tr("Cannot write file: %1").arg(filePath));
        return;
    }

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement("bookmarks");

    for (const BookmarkEntry &entry : allEntries) {
        writer.writeStartElement("bookmark");
        writer.writeAttribute("file", entry.filePath);
        writer.writeAttribute("line", QString::number(entry.lineNumber));
        writer.writeAttribute("text", entry.lineText);
        writer.writeEndElement();
    }

    writer.writeEndElement();
    writer.writeEndDocument();
    file.close();

    QMessageBox::information(this, tr("Export"),
                             tr("Successfully exported %1 bookmark(s).").arg(allEntries.size()));
}

void ManageBookmark::onCloseClicked()
{
    accept();
}

void ManageBookmark::onTableClicked(int row, int column)
{
    Q_UNUSED(column);
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

void ManageBookmark::onTableDoubleClicked(int row, int column)
{
    Q_UNUSED(column);
    onTableClicked(row, 0);
}
