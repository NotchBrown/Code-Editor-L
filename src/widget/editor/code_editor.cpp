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
#include <QFileInfo>

// Scintilla constants
const int SCI_SETUNDODEPTH = 262;
const int SCI_GETUNDODEPTH = 263;
const int SCI_GETCURRENTLINE = 216;
const int SCI_GETCURRENTPOS = 200;
const int SCI_GETLINESTART = 214;

CodeEditor::CodeEditor(QWidget *parent)
    : QsciScintilla(parent),
      m_currentLexer(nullptr)
{
    setupEditor();
}

CodeEditor::~CodeEditor()
{
    if (m_currentLexer) {
        delete m_currentLexer;
    }
}

void CodeEditor::setupEditor()
{
    setUtf8(true);
    setWrapMode(WrapWord);
    setWrapVisualFlags(WrapFlagByText);
    
    setupMargins();
    setupFolding();
    setupLexer();
    setupAutoCompletion();
    setupKeyBindings();
    setupExtensionMap();

    connect(this, &QsciScintilla::textChanged, this, &CodeEditor::onTextChanged);
    connect(this, &QsciScintilla::cursorPositionChanged, this, &CodeEditor::onCursorPositionChanged);
}

void CodeEditor::setupMargins()
{
    setMarginType(0, QsciScintilla::NumberMargin);
    setMarginWidth(0, 50);
    setMarginLineNumbers(0, true);
    setMarginsBackgroundColor(QColor(240, 240, 240));
    setMarginsForegroundColor(QColor(100, 100, 100));
    
    setMarginType(1, QsciScintilla::SymbolMargin);
    setMarginWidth(1, 16);
    setMarginSensitivity(1, true);
}

void CodeEditor::setupFolding()
{
    setFolding(QsciScintilla::BoxedTreeFoldStyle);
    setFoldMarginColors(QColor(245, 245, 245), QColor(240, 240, 240));
}

void CodeEditor::setupLexer()
{
    QsciLexerCPP *lexer = new QsciLexerCPP(this);
    QFont font("Consolas", 10);
    lexer->setDefaultFont(font);
    lexer->setFont(font, -1);
    
    setLexer(lexer);
    m_currentLexer = lexer;
}

void CodeEditor::setupAutoCompletion()
{
    setAutoCompletionThreshold(2);
    setAutoCompletionSource(QsciScintilla::AcsAll);
    setAutoCompletionCaseSensitivity(false);
    setAutoCompletionReplaceWord(true);
}

void CodeEditor::setupKeyBindings()
{
    setTabWidth(4);
    setIndentationWidth(4);
    setAutoIndent(true);
    setIndentationGuides(true);
    setBackspaceUnindents(true);
    setBraceMatching(QsciScintilla::SloppyBraceMatch);
}

void CodeEditor::setupExtensionMap()
{
    m_extensionToLexer[".cpp"] = "cpp";
    m_extensionToLexer[".h"] = "cpp";
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
    m_extensionToLexer[".txt"] = "none";
}

void CodeEditor::setFilePath(const QString &path)
{
    m_filePath = path;
    emit filePathChanged(path);
}

QString CodeEditor::filePath() const
{
    return m_filePath;
}

void CodeEditor::setLexerByExtension(const QString &extension)
{
    QString lexerName = m_extensionToLexer.value(extension.toLower(), "cpp");
    setLexerByName(lexerName);
}

void CodeEditor::setLexerByName(const QString &name)
{
    QsciLexer *newLexer = nullptr;
    QFont font("Consolas", 10);

    if (name == "cpp") {
        newLexer = new QsciLexerCPP(this);
    } else if (name == "python") {
        newLexer = new QsciLexerPython(this);
    } else if (name == "html") {
        newLexer = new QsciLexerHTML(this);
    } else if (name == "javascript") {
        newLexer = new QsciLexerJavaScript(this);
    } else if (name == "json") {
        newLexer = new QsciLexerJSON(this);
    } else if (name == "xml") {
        newLexer = new QsciLexerXML(this);
    } else if (name == "sql") {
        newLexer = new QsciLexerSQL(this);
    } else if (name == "lua") {
        newLexer = new QsciLexerLua(this);
    } else if (name == "bash") {
        newLexer = new QsciLexerBash(this);
    } else if (name == "none") {
        setLexer(nullptr);
        return;
    }

    if (newLexer) {
        newLexer->setDefaultFont(font);
        newLexer->setFont(font, -1);
        
        if (m_currentLexer) {
            delete m_currentLexer;
        }
        
        m_currentLexer = newLexer;
        setLexer(m_currentLexer);
    }
}

void CodeEditor::setUndoDepth(int depth)
{
    SendScintilla(SCI_SETUNDODEPTH, depth);
}

int CodeEditor::undoDepth() const
{
    return SendScintilla(SCI_GETUNDODEPTH);
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
    setCursorPosition(line - 1, 0);
    ensureLineVisible(line - 1);
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