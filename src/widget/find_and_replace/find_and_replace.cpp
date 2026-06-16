#include "main.h"
#include "widget/find_and_replace/find_and_replace.h"
#include "widget/editor/code_editor.h"
#include "ui_find_and_replace.h"
#include <QHeaderView>
#include <QLabel>
#include <QApplication>
#include <QFileInfo>

// Scintilla message constants
const int SCI_GETSELECTIONSTART      = 2143;
const int SCI_GETSELECTIONEND        = 2145;
const int SCI_GETLENGTH              = 2006;
const int SCI_LINEFROMPOSITION       = 2166;
const int SCI_POSITIONFROMLINE       = 2167;
const int SCI_GETLINEENDPOSITION     = 2136;
const int SCI_SETTARGETSTART         = 2190;
const int SCI_SETTARGETEND           = 2192;
const int SCI_SEARCHINTARGET         = 2197;
const int SCI_SETSEARCHFLAGS         = 2198;
const int SCI_GETTARGETTEXT          = 2687;
const int SCI_GETTARGETEND           = 2193;
const int SCI_GETCHARAT              = 2007;
const int SCI_REPLACETARGET          = 2194;
const int SCI_SETSEL                 = 2160;
const int SCI_ENSUREVISIBLEENFORCEPOLICY = 2234;

// SCFIND flags
const int SCFIND_MATCHCASE   = 0x4;

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

FindAndReplace::FindAndReplace(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::FindAndReplace),
      m_editor(nullptr),
      m_replaceEnabled(true),
      m_searchAborted(false)
{
    ui->setupUi(this);

    // Configure results table headers
    ui->resultsTable->horizontalHeader()->setStretchLastSection(false);
    ui->resultsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->resultsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->resultsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->resultsTable->verticalHeader()->hide();

    connectSignals();
}

FindAndReplace::~FindAndReplace()
{
    delete ui;
}

// ---------------------------------------------------------------------------
// Public interface
// ---------------------------------------------------------------------------

void FindAndReplace::setEditor(CodeEditor *editor)
{
    m_editor = editor;
}

CodeEditor *FindAndReplace::editor() const
{
    return m_editor;
}

void FindAndReplace::setAllEditors(const QList<CodeEditor*> &editors)
{
    m_allEditors = editors;
}

void FindAndReplace::activateFindMode()
{
    m_replaceEnabled = false;
    ui->replaceInput->setEnabled(false);
    ui->replaceGroup->setEnabled(false);
}

void FindAndReplace::activateReplaceMode()
{
    m_replaceEnabled = true;
    ui->replaceInput->setEnabled(true);
    ui->replaceGroup->setEnabled(true);
}

void FindAndReplace::focusOnSearch()
{
    ui->searchInput->setFocus();
    ui->searchInput->selectAll();
}

void FindAndReplace::setSearchText(const QString &text)
{
    ui->searchInput->setText(text);
}

void FindAndReplace::connectSignals()
{
    connect(ui->searchInput, &QLineEdit::textChanged,
            this, &FindAndReplace::onSearchTextChanged);
    connect(ui->searchInput, &QLineEdit::returnPressed,
            this, &FindAndReplace::onFind);
    connect(ui->replaceInput, &QLineEdit::returnPressed,
            this, &FindAndReplace::onReplace);

    connect(ui->findSelectionBtn, &QPushButton::clicked,
            this, &FindAndReplace::onFindSelection);
    connect(ui->findCurrentBtn, &QPushButton::clicked,
            this, &FindAndReplace::onFindInCurrent);
    connect(ui->findAllBtn, &QPushButton::clicked,
            this, &FindAndReplace::onFindAll);
    connect(ui->replaceBtn, &QPushButton::clicked,
            this, &FindAndReplace::onReplace);
    connect(ui->replaceAllBtn, &QPushButton::clicked,
            this, &FindAndReplace::onReplaceAll);

    connect(ui->resultsTable, &QTableWidget::cellClicked,
            this, &FindAndReplace::onResultTableClicked);
    // itemClicked also fires when clicking on cells with setCellWidget
    connect(ui->resultsTable, &QTableWidget::itemClicked,
            this, &FindAndReplace::onResultTableItemClicked);
    connect(ui->resultsTable, &QTableWidget::cellDoubleClicked,
            this, &FindAndReplace::onResultTableDoubleClicked);
    connect(ui->resultsTable, &QTableWidget::itemDoubleClicked,
            this, &FindAndReplace::onResultTableItemDoubleClicked);
}

void FindAndReplace::onSearchTextChanged(const QString &text)
{
    Q_UNUSED(text);
    m_searchAborted = true;  // abort any running search
    clearResults();
}

// ---------------------------------------------------------------------------
// Slot: result table clicked -> navigate
// ---------------------------------------------------------------------------

void FindAndReplace::onResultTableClicked(int row, int column)
{
    Q_UNUSED(column);
    navigateToResult(row);
}

void FindAndReplace::onResultTableItemClicked(QTableWidgetItem *item)
{
    if (item) navigateToResult(item->row());
}

void FindAndReplace::onResultTableDoubleClicked(int row, int column)
{
    Q_UNUSED(column);
    navigateToResult(row);
}

void FindAndReplace::onResultTableItemDoubleClicked(QTableWidgetItem *item)
{
    if (item) navigateToResult(item->row());
}

// ---------------------------------------------------------------------------
// Public Slots – entry points
// ---------------------------------------------------------------------------

void FindAndReplace::onFind()
{
    QString text = ui->searchInput->text();
    if (text.isEmpty() || !m_editor) return;

    bool cs   = ui->caseCheck->isChecked();
    bool wrap = ui->wrapCheck->isChecked();

    performSearch(text, cs, wrap, true);
}

void FindAndReplace::onFindNext()
{
    if (!m_editor || ui->searchInput->text().isEmpty()) return;

    QString text = ui->searchInput->text();
    bool cs   = ui->caseCheck->isChecked();
    bool wrap = ui->wrapCheck->isChecked();

    // QScintilla requires findFirst before findNext; we always start fresh
    // from current cursor position
    bool found = m_editor->findFirst(text, false, cs, false, wrap, true,
                                     -1, -1, false, false, false);
    if (found) {
        int selStart = m_editor->SendScintilla(SCI_GETSELECTIONSTART);
        int selEnd   = m_editor->SendScintilla(SCI_GETSELECTIONEND);
        int line = m_editor->SendScintilla(SCI_LINEFROMPOSITION, selStart);
        int col  = selStart - m_editor->SendScintilla(SCI_POSITIONFROMLINE, line);
        addResultContext(selStart, selEnd, line, col, m_editor);
    }
}

void FindAndReplace::onFindPrev()
{
    if (!m_editor || ui->searchInput->text().isEmpty()) return;

    QString text = ui->searchInput->text();
    bool cs   = ui->caseCheck->isChecked();
    bool wrap = ui->wrapCheck->isChecked();

    bool found = m_editor->findFirst(text, false, cs, false, wrap, false,
                                     -1, -1, false, false, false);
    if (found) {
        int selStart = m_editor->SendScintilla(SCI_GETSELECTIONSTART);
        int selEnd   = m_editor->SendScintilla(SCI_GETSELECTIONEND);
        int line = m_editor->SendScintilla(SCI_LINEFROMPOSITION, selStart);
        int col  = selStart - m_editor->SendScintilla(SCI_POSITIONFROMLINE, line);
        addResultContext(selStart, selEnd, line, col, m_editor);
    }
}

void FindAndReplace::onFindSelection()
{
    if (!m_editor) return;
    if (!m_editor->hasSelectedText()) return;
    if (ui->searchInput->text().isEmpty()) return;

    QString text = ui->searchInput->text();
    bool cs = ui->caseCheck->isChecked();
    clearResults();

    int userSelStart = m_editor->SendScintilla(SCI_GETSELECTIONSTART);
    int userSelEnd   = m_editor->SendScintilla(SCI_GETSELECTIONEND);
    int flags = cs ? SCFIND_MATCHCASE : 0;

    m_editor->cancelFind();
    int pos = userSelStart;
    while (pos < userSelEnd) {
        m_editor->SendScintilla(SCI_SETSEARCHFLAGS, flags);
        m_editor->SendScintilla(SCI_SETTARGETSTART, pos);
        m_editor->SendScintilla(SCI_SETTARGETEND, userSelEnd);

        QByteArray searchBytes = text.toUtf8();
        int foundPos = m_editor->SendScintilla(SCI_SEARCHINTARGET,
                                                searchBytes.length(),
                                                searchBytes.constData());
        if (foundPos == -1) break;

        int matchLen = m_editor->SendScintilla(SCI_GETTARGETEND) - foundPos;
        int line = m_editor->SendScintilla(SCI_LINEFROMPOSITION, foundPos);
        int col  = foundPos - m_editor->SendScintilla(SCI_POSITIONFROMLINE, line);
        addResultContext(foundPos, foundPos + matchLen, line, col, m_editor);

        pos = foundPos + matchLen;
        if (pos <= foundPos) pos = foundPos + 1;
    }
}

void FindAndReplace::onFindInCurrent()
{
    if (!m_editor || ui->searchInput->text().isEmpty()) return;
    m_searchAborted = true;  // abort any previous search
    QApplication::processEvents();
    m_searchAborted = false;
    clearResults();
    performSearchAllInEditor(ui->searchInput->text(), ui->caseCheck->isChecked(), m_editor);
}

void FindAndReplace::onFindAll()
{
    QString text = ui->searchInput->text();
    if (text.isEmpty()) return;
    m_searchAborted = true;
    QApplication::processEvents();
    m_searchAborted = false;
    bool cs = ui->caseCheck->isChecked();
    clearResults();

    // Search all tabs
    QList<CodeEditor*> editors = m_allEditors;
    if (!m_editor) return;
    if (!editors.contains(m_editor)) editors.prepend(m_editor);

    foreach (CodeEditor *editor, editors) {
        if (editor) performSearchAllInEditor(text, cs, editor);
    }
}

void FindAndReplace::onReplace()
{
    QString replaceText = ui->replaceInput->text();
    int row = ui->resultsTable->currentRow();
    if (row < 0) return; // nothing selected
    if (row >= ui->resultsTable->rowCount()) return;

    QTableWidgetItem *lineItem = ui->resultsTable->item(row, 1);
    if (!lineItem) return;

    int selStart = lineItem->data(Qt::UserRole).toInt();
    int selEnd   = lineItem->data(Qt::UserRole + 1).toInt();

    m_editor->SendScintilla(SCI_SETSEL, selStart, selEnd);
    m_editor->replace(replaceText);

    ui->resultsTable->removeRow(row);
}

void FindAndReplace::onReplaceAll()
{
    QString replaceText = ui->replaceInput->text();
    if (ui->resultsTable->rowCount() == 0) return;

    // Iterate bottom-to-top so positions stay valid
    for (int row = ui->resultsTable->rowCount() - 1; row >= 0; --row) {
        QTableWidgetItem *lineItem = ui->resultsTable->item(row, 1);
        if (!lineItem) continue;

        int selStart = lineItem->data(Qt::UserRole).toInt();
        int selEnd   = lineItem->data(Qt::UserRole + 1).toInt();

        m_editor->SendScintilla(SCI_SETSEL, selStart, selEnd);
        m_editor->replace(replaceText);
    }
    clearResults();
}

void FindAndReplace::clearResults()
{
    ui->resultsTable->setRowCount(0);
}

// ---------------------------------------------------------------------------
// Private: search / replace helpers
// ---------------------------------------------------------------------------

void FindAndReplace::performSearch(const QString &text, bool caseSensitive,
                                   bool wrap, bool forward)
{
    if (!m_editor) return;

    clearResults();

    bool found = m_editor->findFirst(text, false, caseSensitive, false,
                                     wrap, forward, -1, -1, false, false, false);
    if (found) {
        int selStart = m_editor->SendScintilla(SCI_GETSELECTIONSTART);
        int selEnd   = m_editor->SendScintilla(SCI_GETSELECTIONEND);
        int line = m_editor->SendScintilla(SCI_LINEFROMPOSITION, selStart);
        int col  = selStart - m_editor->SendScintilla(SCI_POSITIONFROMLINE, line);
        addResultContext(selStart, selEnd, line, col, m_editor);
    }
}

void FindAndReplace::performSearchAll(const QString &text, bool caseSensitive)
{
    if (!m_editor) return;
    performSearchAllInEditor(text, caseSensitive, m_editor);
}

void FindAndReplace::performSearchAllInEditor(const QString &text, bool caseSensitive, CodeEditor *editor)
{
    if (!editor) return;
    editor->cancelFind();

    QString editorName = editor->filePath().isEmpty()
        ? tr("Untitled")
        : QFileInfo(editor->filePath()).fileName();

    int docLen = editor->SendScintilla(SCI_GETLENGTH);
    int flags = caseSensitive ? SCFIND_MATCHCASE : 0;

    // First pass: count approximate matches for progress
    int approxTotal = 0;
    int countPos = 0;
    while (countPos < docLen) {
        editor->SendScintilla(SCI_SETSEARCHFLAGS, flags);
        editor->SendScintilla(SCI_SETTARGETSTART, countPos);
        editor->SendScintilla(SCI_SETTARGETEND, docLen);
        QByteArray searchBytes = text.toUtf8();
        int fp = editor->SendScintilla(SCI_SEARCHINTARGET,
                                        searchBytes.length(),
                                        searchBytes.constData());
        if (fp == -1) break;
        int ml = editor->SendScintilla(SCI_GETTARGETEND) - fp;
        countPos = fp + (ml > 0 ? ml : 1);
        approxTotal++;
        if (approxTotal > 100000) { // safety cap
            approxTotal = approxTotal;
            break;
        }
    }
    if (approxTotal == 0) approxTotal = 1;

    // Second pass: collect results with progress updates
    emit searchProgress(0, approxTotal, editorName);
    QApplication::processEvents();
    if (m_searchAborted) { m_searchAborted = false; return; }

    int pos = 0;
    int found = 0;
    int batchCounter = 0;

    while (pos < docLen && !m_searchAborted) {
        editor->SendScintilla(SCI_SETSEARCHFLAGS, flags);
        editor->SendScintilla(SCI_SETTARGETSTART, pos);
        editor->SendScintilla(SCI_SETTARGETEND, docLen);
        QByteArray searchBytes = text.toUtf8();
        int foundPos = editor->SendScintilla(SCI_SEARCHINTARGET,
                                              searchBytes.length(),
                                              searchBytes.constData());
        if (foundPos == -1) break;

        int matchLen = editor->SendScintilla(SCI_GETTARGETEND) - foundPos;
        QString matched = editor->text(foundPos, foundPos + matchLen);

        if (ui->widthCheck->isChecked()) {
            QString searchNf = text.normalized(QString::NormalizationForm_KC);
            QString matchNf  = matched.normalized(QString::NormalizationForm_KC);
            if (searchNf != matchNf) {
                pos = foundPos + 1;
                continue;
            }
        }

        int line = editor->SendScintilla(SCI_LINEFROMPOSITION, foundPos);
        int col  = foundPos - editor->SendScintilla(SCI_POSITIONFROMLINE, line);
        addResultContext(foundPos, foundPos + matchLen, line, col, editor);
        found++;

        pos = foundPos + matchLen;
        if (pos <= foundPos) pos = foundPos + 1;

        // Yield to event loop every 50 results to keep UI responsive
        batchCounter++;
        if (batchCounter >= 50) {
            batchCounter = 0;
            emit searchProgress(found, approxTotal, editorName);
            QApplication::processEvents();
        }
    }

    m_searchAborted = false;
    emit searchProgress(found, found, editorName);
    emit searchFinished(found);
    QApplication::processEvents();
}

void FindAndReplace::performReplace(const QString &find, const QString &replace)
{
    if (!m_editor) return;

    bool cs   = ui->caseCheck->isChecked();
    bool wrap = ui->wrapCheck->isChecked();

    bool found = m_editor->findFirst(find, false, cs, false, wrap, true,
                                     -1, -1, false, false, false);
    if (found) {
        m_editor->replace(replace);
    }
}

void FindAndReplace::performReplaceAll(const QString &find, const QString &replace)
{
    if (!m_editor) return;

    m_editor->cancelFind();
    bool cs = ui->caseCheck->isChecked();

    int docLen = m_editor->SendScintilla(SCI_GETLENGTH);
    int flags = cs ? SCFIND_MATCHCASE : 0;
    int replaced = 0;

    int pos = 0;
    while (pos < docLen) {
        m_editor->SendScintilla(SCI_SETSEARCHFLAGS, flags);
        m_editor->SendScintilla(SCI_SETTARGETSTART, pos);
        m_editor->SendScintilla(SCI_SETTARGETEND, docLen);

        QByteArray findBytes = find.toUtf8();
        int foundPos = m_editor->SendScintilla(SCI_SEARCHINTARGET,
                                                findBytes.length(),
                                                findBytes.constData());
        if (foundPos == -1) break;

        int matchLen = m_editor->SendScintilla(SCI_GETTARGETEND) - foundPos;

        if (ui->widthCheck->isChecked()) {
            QString matched = m_editor->text(foundPos, foundPos + matchLen);
            QString searchNf = find.normalized(QString::NormalizationForm_KC);
            QString matchNf  = matched.normalized(QString::NormalizationForm_KC);
            if (searchNf != matchNf) {
                pos = foundPos + 1;
                continue;
            }
        }

        QByteArray replaceBytes = replace.toUtf8();
        m_editor->SendScintilla(SCI_REPLACETARGET,
                                replaceBytes.length(),
                                replaceBytes.constData());

        int insertedLen = replace.length();
        docLen += (insertedLen - matchLen);
        pos = foundPos + insertedLen;
        replaced++;
    }
}

// ---------------------------------------------------------------------------
// Add result row with context display
// ---------------------------------------------------------------------------

void FindAndReplace::addResultContext(int selStart, int selEnd, int line, int col, CodeEditor *editor)
{
    if (!editor) return;

    int docLen = editor->SendScintilla(SCI_GETLENGTH);
    int contextChars = 15;

    int ctxStart = qMax(0, selStart - contextChars);
    int ctxEnd   = qMin(docLen, selEnd + contextChars);

    QString prefix = editor->text(ctxStart, selStart);
    QString match  = editor->text(selStart, selEnd);
    QString suffix = editor->text(selEnd, ctxEnd);

    prefix.replace('\n', "\\n").replace('\r', "\\r");
    suffix.replace('\n', "\\n").replace('\r', "\\r");
    match.replace('\n', "\\n").replace('\r', "\\r");

    QString contextHtml = prefix.toHtmlEscaped()
                          + "<b>" + match.toHtmlEscaped() + "</b>"
                          + suffix.toHtmlEscaped();

    int row = ui->resultsTable->rowCount();
    ui->resultsTable->insertRow(row);

    QLabel *contextLabel = new QLabel(contextHtml);
    contextLabel->setTextFormat(Qt::RichText);
    contextLabel->setWordWrap(false);
    contextLabel->setContentsMargins(4, 2, 4, 2);
    contextLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->resultsTable->setCellWidget(row, 0, contextLabel);

    QTableWidgetItem *lineItem = new QTableWidgetItem(QString::number(line + 1));
    lineItem->setTextAlignment(Qt::AlignCenter);
    ui->resultsTable->setItem(row, 1, lineItem);

    QTableWidgetItem *colItem = new QTableWidgetItem(QString::number(col + 1));
    colItem->setTextAlignment(Qt::AlignCenter);
    ui->resultsTable->setItem(row, 2, colItem);

    lineItem->setData(Qt::UserRole, selStart);
    lineItem->setData(Qt::UserRole + 1, selEnd);
    // Store a pointer to the editor so cross-tab navigation works
    lineItem->setData(Qt::UserRole + 2, reinterpret_cast<quintptr>(editor));
}

// ---------------------------------------------------------------------------
// Navigate to result row
// ---------------------------------------------------------------------------

void FindAndReplace::navigateToResult(int row)
{
    if (row < 0 || row >= ui->resultsTable->rowCount()) return;

    QTableWidgetItem *lineItem = ui->resultsTable->item(row, 1);
    if (!lineItem) return;

    int selStart = lineItem->data(Qt::UserRole).toInt();
    int selEnd   = lineItem->data(Qt::UserRole + 1).toInt();
    CodeEditor *editor = reinterpret_cast<CodeEditor*>(lineItem->data(Qt::UserRole + 2).value<quintptr>());
    if (!editor) editor = m_editor;
    if (!editor) return;

    editor->SendScintilla(SCI_SETSEL, selStart, selEnd);
    editor->SendScintilla(SCI_ENSUREVISIBLEENFORCEPOLICY,
                          editor->SendScintilla(SCI_LINEFROMPOSITION, selStart));
    editor->setFocus();
}
