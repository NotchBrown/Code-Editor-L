#include "main.h"
#include "widget/editor/code_editor.h"
#include <Qsci/qscilexercpp.h>
#include <Qsci/qscilexerpython.h>
#include <Qsci/qscilexerhtml.h>
#include <Qsci/qscilexerjavascript.h>
#include <Qsci/qscilexerxml.h>
#include <Qsci/qscilexersql.h>
#include <Qsci/qscilexerlua.h>
#include <Qsci/qscilexerbash.h>
#include <Qsci/qscilexerjson.h>
#include <QFont>

const int SCI_SETUNDODEPTH = 262;
const int SCI_DELETERANGE = 227;
const int SCI_GETLINESTART = 216;
const int SCI_GETCURRENTPOS = 200;
const int SCI_GETCURRENTLINE = 201;
const int SCI_GETLINEENDPOSITION = 234;
const int SCI_GETLINECOUNT = 204;

CodeEditor::CodeEditor(QWidget *parent)
    : QsciScintilla(parent),
      m_filePath(""),
      m_lexer(nullptr)
{
    m_extensionToLexer[".cpp"] = "cpp";
    m_extensionToLexer[".h"] = "cpp";
    m_extensionToLexer[".cxx"] = "cpp";
    m_extensionToLexer[".hpp"] = "cpp";
    m_extensionToLexer[".c"] = "cpp";
    m_extensionToLexer[".py"] = "python";
    m_extensionToLexer[".html"] = "html";
    m_extensionToLexer[".htm"] = "html";
    m_extensionToLexer[".js"] = "javascript";
    m_extensionToLexer[".json"] = "json";
    m_extensionToLexer[".xml"] = "xml";
    m_extensionToLexer[".sql"] = "sql";
    m_extensionToLexer[".lua"] = "lua";
    m_extensionToLexer[".sh"] = "bash";
    m_extensionToLexer[".bash"] = "bash";
    
    setupEditor();
    setupConnections();
}

CodeEditor::~CodeEditor()
{
    if (m_lexer) {
        delete m_lexer;
    }
}

void CodeEditor::setupEditor()
{
    QFont font("Consolas", 10);
    setFont(font);
    setMarginsFont(font);
    
    setUtf8(true);
    setEolMode(QsciScintilla::EolWindows);
    setIndentationsUseTabs(false);
    setTabWidth(4);
    setIndentationGuides(true);
    setTabIndents(true);
    setAutoIndent(true);
    
    SendScintilla(SCI_SETUNDODEPTH, 1000);
    
    setCaretLineVisible(true);
    setCaretLineBackgroundColor(QColor(230, 240, 255));
    
    setBraceMatching(QsciScintilla::SloppyBraceMatch);
    
    setFolding(QsciScintilla::BoxedTreeFoldStyle);
    
    setMarginType(0, QsciScintilla::NumberMargin);
    setMarginWidth(0, 50);
    setMarginLineNumbers(0, true);
    setMarginsBackgroundColor(QColor(240, 240, 240));
    setMarginsForegroundColor(QColor(100, 100, 100));
    
    setMarginType(1, QsciScintilla::SymbolMargin);
    setMarginWidth(1, 16);
    setMarginSensitivity(1, true);
    
    setLexerByName("cpp");
}

void CodeEditor::setupConnections()
{
    connect(this, &QsciScintilla::textChanged, this, &CodeEditor::onTextChanged);
    connect(this, &QsciScintilla::cursorPositionChanged, this, &CodeEditor::onCursorPositionChanged);
}

QString CodeEditor::filePath() const
{
    return m_filePath;
}

void CodeEditor::setFilePath(const QString &path)
{
    m_filePath = path;
    emit filePathChanged(path);
    updateLexerFromFile();
}

bool CodeEditor::loadFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return false;
    }
    
    QTextStream in(&file);
    QString content = in.readAll();
    file.close();
    
    setFilePath(filePath);
    setText(content);
    setModified(false);
    
    return true;
}

bool CodeEditor::saveFile(const QString &filePath)
{
    QString path = filePath;
    if (path.isEmpty()) {
        path = m_filePath;
    }
    
    if (path.isEmpty()) {
        return false;
    }
    
    QFile file(path);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        return false;
    }
    
    QTextStream out(&file);
    out << text();
    file.close();
    
    setFilePath(path);
    setModified(false);
    
    return true;
}

void CodeEditor::updateLexerFromFile()
{
    if (m_filePath.isEmpty()) {
        setLexerByName("cpp");
        return;
    }
    
    QFileInfo fileInfo(m_filePath);
    QString extension = fileInfo.suffix();
    
    QString lexerName = m_extensionToLexer.value("." + extension.toLower(), "cpp");
    setLexerByName(lexerName);
}

void CodeEditor::setLexerByName(const QString &name)
{
    if (m_lexer) {
        delete m_lexer;
        m_lexer = nullptr;
    }

    QFont font("Consolas", 10);

    if (name == "cpp") {
        m_lexer = new QsciLexerCPP(this);
    } else if (name == "python") {
        m_lexer = new QsciLexerPython(this);
    } else if (name == "html") {
        m_lexer = new QsciLexerHTML(this);
    } else if (name == "javascript") {
        m_lexer = new QsciLexerJavaScript(this);
    } else if (name == "json") {
        m_lexer = new QsciLexerJSON(this);
    } else if (name == "xml") {
        m_lexer = new QsciLexerXML(this);
    } else if (name == "sql") {
        m_lexer = new QsciLexerSQL(this);
    } else if (name == "lua") {
        m_lexer = new QsciLexerLua(this);
    } else if (name == "bash") {
        m_lexer = new QsciLexerBash(this);
    } else if (name == "none") {
        setLexer(nullptr);
        return;
    } else {
        m_lexer = new QsciLexerCPP(this);
    }

    if (m_lexer) {
        m_lexer->setFont(font);
        setLexer(m_lexer);
    }
}

QString CodeEditor::selectedText() const
{
    return QsciScintilla::selectedText();
}

void CodeEditor::insertText(const QString &text)
{
    insert(text);
}

void CodeEditor::goToLine(int line)
{
    int totalLines = SendScintilla(SCI_GETLINECOUNT);
    if (line > 0 && line <= totalLines) {
        setCursorPosition(line - 1, 0);
        ensureLineVisible(line - 1);
    }
}

void CodeEditor::findText(const QString &text, bool caseSensitive, bool wholeWord)
{
    findFirst(text, false, caseSensitive, wholeWord, false, false, 0, 0, true, false, false);
}

void CodeEditor::replaceText(const QString &find, const QString &replace, bool all)
{
    if (all) {
        while (findFirst(find, false, false, false, false, false, 0, 0, true, false, false)) {
            QsciScintilla::replace(replace);
        }
    } else {
        findFirst(find, false, false, false, false, false, 0, 0, true, false, false);
        if (hasSelectedText()) {
            QsciScintilla::replace(replace);
        }
    }
}

void CodeEditor::onTextChanged()
{
    emit modificationChanged(isModified());
}

void CodeEditor::onCursorPositionChanged(int line, int index)
{
    emit cursorPositionChanged(line + 1, index + 1);
}

int CodeEditor::currentLine() const
{
    return SendScintilla(SCI_GETCURRENTLINE);
}

int CodeEditor::currentColumn() const
{
    return SendScintilla(SCI_GETCURRENTPOS) - SendScintilla(SCI_GETLINESTART, SendScintilla(SCI_GETCURRENTLINE));
}

void CodeEditor::toggleBookmark(int line)
{
    if (markersAtLine(line) & (1 << 0)) {
        markerDelete(line, 1);
    } else {
        markerAdd(line, 1);
    }
}

void CodeEditor::clearAllBookmarks()
{
    markerDeleteAll(1);
}

void CodeEditor::toggleBreakpoint(int line)
{
    if (markersAtLine(line) & (1 << 1)) {
        markerDelete(line, 2);
    } else {
        markerAdd(line, 2);
    }
}

void CodeEditor::clearBreakpoint(int line)
{
    markerDelete(line, 2);
}

void CodeEditor::clearAllBreakpoints()
{
    markerDeleteAll(2);
}

void CodeEditor::commentLine()
{
    if (!hasSelectedText()) {
        int line = SendScintilla(SCI_GETCURRENTLINE);
        QString txt = QsciScintilla::text(line);
        if (!txt.startsWith("//")) {
            insertAt("//", line, 0);
        }
    } else {
        int startLine, startCol, endLine, endCol;
        getSelection(&startLine, &startCol, &endLine, &endCol);
        
        for (int i = startLine; i <= endLine; ++i) {
            QString txt = QsciScintilla::text(i);
            if (!txt.startsWith("//")) {
                insertAt("//", i, 0);
            }
        }
    }
}

void CodeEditor::uncommentLine()
{
    if (!hasSelectedText()) {
        int line = SendScintilla(SCI_GETCURRENTLINE);
        QString txt = QsciScintilla::text(line);
        if (txt.startsWith("//")) {
            int lineStart = SendScintilla(SCI_GETLINESTART, line);
            SendScintilla(SCI_DELETERANGE, lineStart, 2);
        }
    } else {
        int startLine, startCol, endLine, endCol;
        getSelection(&startLine, &startCol, &endLine, &endCol);
        
        for (int i = startLine; i <= endLine; ++i) {
            QString txt = QsciScintilla::text(i);
            if (txt.startsWith("//")) {
                int lineStart = SendScintilla(SCI_GETLINESTART, i);
                SendScintilla(SCI_DELETERANGE, lineStart, 2);
            }
        }
    }
}

void CodeEditor::commentBlock()
{
    if (!hasSelectedText()) {
        int line = SendScintilla(SCI_GETCURRENTLINE);
        insertAt("/* ", line, 0);
        QString lineText = QsciScintilla::text(line);
        insertAt(" */", line, lineText.length());
    } else {
        int startLine, startCol, endLine, endCol;
        getSelection(&startLine, &startCol, &endLine, &endCol);
        
        insertAt("/* ", startLine, 0);
        QString endText = QsciScintilla::text(endLine);
        if (endText.endsWith("\n")) {
            insertAt(" */", endLine, endText.length() - 1);
        } else {
            insertAt(" */", endLine, endText.length());
        }
    }
}

void CodeEditor::uncommentBlock()
{
    if (!hasSelectedText()) {
        int line = SendScintilla(SCI_GETCURRENTLINE);
        QString txt = QsciScintilla::text(line);
        if (txt.startsWith("/* ")) {
            int lineStart = SendScintilla(SCI_GETLINESTART, line);
            SendScintilla(SCI_DELETERANGE, lineStart, 3);
        }
        txt = QsciScintilla::text(line);
        if (txt.endsWith(" */")) {
            int lineEnd = SendScintilla(SCI_GETLINEENDPOSITION, line);
            SendScintilla(SCI_DELETERANGE, lineEnd - 3, 3);
        }
    } else {
        int startLine, startCol, endLine, endCol;
        getSelection(&startLine, &startCol, &endLine, &endCol);
        
        QString startText = QsciScintilla::text(startLine);
        if (startText.startsWith("/* ")) {
            int lineStart = SendScintilla(SCI_GETLINESTART, startLine);
            SendScintilla(SCI_DELETERANGE, lineStart, 3);
        }
        
        QString endText = QsciScintilla::text(endLine);
        if (endText.endsWith(" */")) {
            int lineEnd = SendScintilla(SCI_GETLINEENDPOSITION, endLine);
            SendScintilla(SCI_DELETERANGE, lineEnd - 3, 3);
        }
    }
}

void CodeEditor::deleteChar()
{
    if (hasSelectedText()) {
        removeSelectedText();
    } else {
        SendScintilla(SCI_DELETEBACK);
    }
}
