#include "main.h"
#include "widget/editor/code_editor.h"
#include <QTextCodec>
#include <Qsci/qscilexercpp.h>
#include <Qsci/qscilexerpython.h>
#include <Qsci/qscilexerhtml.h>
#include <Qsci/qscilexerjavascript.h>
#include <Qsci/qscilexerxml.h>
#include <Qsci/qscilexersql.h>
#include <Qsci/qscilexerlua.h>
#include <Qsci/qscilexerbash.h>
#include <Qsci/qscilexerjson.h>
#include <Qsci/qscilexerjava.h>
#include <Qsci/qscilexercsharp.h>
#include <Qsci/qscilexerperl.h>
#include <Qsci/qscilexerruby.h>
#include <Qsci/qscilexercss.h>
#include <Qsci/qscilexeryaml.h>
#include <Qsci/qscilexermarkdown.h>
#include <Qsci/qscilexertex.h>
#include <Qsci/qscilexerdiff.h>
#include <Qsci/qscilexerd.h>
#include <Qsci/qscilexerfortran.h>
#include <Qsci/qscilexerfortran77.h>
#include <Qsci/qscilexermatlab.h>
#include <Qsci/qscilexermakefile.h>
#include <Qsci/qscilexercmake.h>
#include <Qsci/qscilexertcl.h>
#include <Qsci/qscilexerbatch.h>
#include <Qsci/qscilexerpov.h>
#include <Qsci/qscilexerpostscript.h>
#include <Qsci/qscilexerspice.h>
#include <Qsci/qscilexeridl.h>
#include <Qsci/qscilexerpascal.h>
#include <Qsci/qscilexerintelhex.h>
#include <Qsci/qscilexervhdl.h>
#include <Qsci/qscilexerverilog.h>
#include <Qsci/qscilexeravs.h>
#include <Qsci/qscilexeredifact.h>
#include <Qsci/qscilexerproperties.h>
#include <Qsci/qscilexerpo.h>
#include <Qsci/qscilexercoffeescript.h>
#include <Qsci/qscilexeroctave.h>
#include <Qsci/qscilexercustom.h>
#include <Qsci/qsciprinter.h>
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexer.h>
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
      m_lexer(nullptr),
      m_tempLexerName(""),
      m_currentLexerName("cpp"),
      m_currentEncoding("UTF-8")
{
    m_extensionToLexer[".cpp"] = "cpp";
    m_extensionToLexer[".h"] = "cpp";
    m_extensionToLexer[".cxx"] = "cpp";
    m_extensionToLexer[".hpp"] = "cpp";
    m_extensionToLexer[".c"] = "cpp";
    m_extensionToLexer[".cc"] = "cpp";
    m_extensionToLexer[".py"] = "python";
    m_extensionToLexer[".pyw"] = "python";
    m_extensionToLexer[".html"] = "html";
    m_extensionToLexer[".htm"] = "html";
    m_extensionToLexer[".xhtml"] = "html";
    m_extensionToLexer[".js"] = "javascript";
    m_extensionToLexer[".json"] = "json";
    m_extensionToLexer[".xml"] = "xml";
    m_extensionToLexer[".xsl"] = "xml";
    m_extensionToLexer[".xsd"] = "xml";
    m_extensionToLexer[".sql"] = "sql";
    m_extensionToLexer[".lua"] = "lua";
    m_extensionToLexer[".sh"] = "bash";
    m_extensionToLexer[".bash"] = "bash";
    m_extensionToLexer[".zsh"] = "bash";
    m_extensionToLexer[".java"] = "java";
    m_extensionToLexer[".cs"] = "csharp";
    m_extensionToLexer[".php"] = "php";
    m_extensionToLexer[".pl"] = "perl";
    m_extensionToLexer[".pm"] = "perl";
    m_extensionToLexer[".rb"] = "ruby";
    m_extensionToLexer[".css"] = "css";
    m_extensionToLexer[".scss"] = "css";
    m_extensionToLexer[".less"] = "css";
    m_extensionToLexer[".yaml"] = "yaml";
    m_extensionToLexer[".yml"] = "yaml";
    m_extensionToLexer[".md"] = "markdown";
    m_extensionToLexer[".markdown"] = "markdown";
    m_extensionToLexer[".tex"] = "tex";
    m_extensionToLexer[".latex"] = "tex";
    m_extensionToLexer[".diff"] = "diff";
    m_extensionToLexer[".patch"] = "diff";
    m_extensionToLexer[".d"] = "d";
    m_extensionToLexer[".di"] = "d";
    m_extensionToLexer[".f"] = "fortran";
    m_extensionToLexer[".for"] = "fortran";
    m_extensionToLexer[".f90"] = "fortran";
    m_extensionToLexer[".f95"] = "fortran";
    m_extensionToLexer[".f03"] = "fortran";
    m_extensionToLexer[".f08"] = "fortran";
    m_extensionToLexer[".f77"] = "fortran77";
    m_extensionToLexer[".m"] = "matlab";
    m_extensionToLexer[".mak"] = "makefile";
    m_extensionToLexer[".mk"] = "makefile";
    m_extensionToLexer["Makefile"] = "makefile";
    m_extensionToLexer["makefile"] = "makefile";
    m_extensionToLexer[".cmake"] = "cmake";
    m_extensionToLexer[".CMakeLists.txt"] = "cmake";
    m_extensionToLexer[".tcl"] = "tcl";
    m_extensionToLexer[".bat"] = "batch";
    m_extensionToLexer[".cmd"] = "batch";
    m_extensionToLexer[".pov"] = "pov";
    m_extensionToLexer[".ps"] = "postscript";
    m_extensionToLexer[".eps"] = "postscript";
    m_extensionToLexer[".spice"] = "spice";
    m_extensionToLexer[".cir"] = "spice";
    m_extensionToLexer[".idl"] = "idl";
    m_extensionToLexer[".odl"] = "idl";
    m_extensionToLexer[".pas"] = "pascal";
    m_extensionToLexer[".pp"] = "pascal";
    m_extensionToLexer[".inc"] = "pascal";
    m_extensionToLexer[".ihex"] = "intelhex";
    m_extensionToLexer[".vhdl"] = "vhdl";
    m_extensionToLexer[".vhd"] = "vhdl";
    m_extensionToLexer[".v"] = "verilog";
    
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
    if (!file.open(QFile::ReadOnly)) {
        return false;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QString content = QString::fromUtf8(data);
    
    // Try to detect encoding if it's not valid UTF-8
    if (content.contains(QChar(0xFFFD))) {
        // Try other encodings
        QTextCodec *codec = QTextCodec::codecForName("GBK");
        if (codec) {
            content = codec->toUnicode(data);
            m_currentEncoding = "GBK";
        } else {
            content = QString::fromLocal8Bit(data);
            m_currentEncoding = "System";
        }
    } else {
        m_currentEncoding = "UTF-8";
    }
    
    setFilePath(filePath);
    setText(content);
    setModified(false);
    
    emit encodingChanged(m_currentEncoding);
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
    
    return saveWithEncoding(path, m_currentEncoding);
}

void CodeEditor::updateLexerFromFile()
{
    if (m_filePath.isEmpty()) {
        // If no file path, use temporary lexer if set, otherwise default to cpp
        if (!m_tempLexerName.isEmpty()) {
            setLexerByName(m_tempLexerName);
        } else {
            setLexerByName("cpp");
        }
        return;
    }
    
    QFileInfo fileInfo(m_filePath);
    QString extension = fileInfo.suffix();
    
    // Check for special files that don't have extensions
    if (fileInfo.fileName() == "Makefile" || fileInfo.fileName() == "makefile") {
        setLexerByName("makefile");
        return;
    }
    
    if (fileInfo.fileName() == "CMakeLists.txt") {
        setLexerByName("cmake");
        return;
    }
    
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
        m_currentLexerName = "cpp";
    } else if (name == "python") {
        m_lexer = new QsciLexerPython(this);
        m_currentLexerName = "python";
    } else if (name == "html") {
        m_lexer = new QsciLexerHTML(this);
        m_currentLexerName = "html";
    } else if (name == "javascript") {
        m_lexer = new QsciLexerJavaScript(this);
        m_currentLexerName = "javascript";
    } else if (name == "json") {
        m_lexer = new QsciLexerJSON(this);
        m_currentLexerName = "json";
    } else if (name == "xml") {
        m_lexer = new QsciLexerXML(this);
        m_currentLexerName = "xml";
    } else if (name == "sql") {
        m_lexer = new QsciLexerSQL(this);
        m_currentLexerName = "sql";
    } else if (name == "lua") {
        m_lexer = new QsciLexerLua(this);
        m_currentLexerName = "lua";
    } else if (name == "bash") {
        m_lexer = new QsciLexerBash(this);
        m_currentLexerName = "bash";
    } else if (name == "java") {
        m_lexer = new QsciLexerJava(this);
        m_currentLexerName = "java";
    } else if (name == "csharp") {
        m_lexer = new QsciLexerCSharp(this);
        m_currentLexerName = "csharp";
    } else if (name == "perl") {
        m_lexer = new QsciLexerPerl(this);
        m_currentLexerName = "perl";
    } else if (name == "ruby") {
        m_lexer = new QsciLexerRuby(this);
        m_currentLexerName = "ruby";
    } else if (name == "css") {
        m_lexer = new QsciLexerCSS(this);
        m_currentLexerName = "css";
    } else if (name == "yaml") {
        m_lexer = new QsciLexerYAML(this);
        m_currentLexerName = "yaml";
    } else if (name == "markdown") {
        m_lexer = new QsciLexerMarkdown(this);
        m_currentLexerName = "markdown";
    } else if (name == "tex") {
        m_lexer = new QsciLexerTeX(this);
        m_currentLexerName = "tex";
    } else if (name == "diff") {
        m_lexer = new QsciLexerDiff(this);
        m_currentLexerName = "diff";
    } else if (name == "d") {
        m_lexer = new QsciLexerD(this);
        m_currentLexerName = "d";
    } else if (name == "fortran") {
        m_lexer = new QsciLexerFortran(this);
        m_currentLexerName = "fortran";
    } else if (name == "fortran77") {
        m_lexer = new QsciLexerFortran77(this);
        m_currentLexerName = "fortran77";
    } else if (name == "matlab") {
        m_lexer = new QsciLexerMatlab(this);
        m_currentLexerName = "matlab";
    } else if (name == "makefile") {
        m_lexer = new QsciLexerMakefile(this);
        m_currentLexerName = "makefile";
    } else if (name == "cmake") {
        m_lexer = new QsciLexerCMake(this);
        m_currentLexerName = "cmake";
    } else if (name == "tcl") {
        m_lexer = new QsciLexerTCL(this);
        m_currentLexerName = "tcl";
    } else if (name == "batch") {
        m_lexer = new QsciLexerBatch(this);
        m_currentLexerName = "batch";
    } else if (name == "pov") {
        m_lexer = new QsciLexerPOV(this);
        m_currentLexerName = "pov";
    } else if (name == "postscript") {
        m_lexer = new QsciLexerPostScript(this);
        m_currentLexerName = "postscript";
    } else if (name == "spice") {
        m_lexer = new QsciLexerSpice(this);
        m_currentLexerName = "spice";
    } else if (name == "idl") {
        m_lexer = new QsciLexerIDL(this);
        m_currentLexerName = "idl";
    } else if (name == "pascal") {
        m_lexer = new QsciLexerPascal(this);
        m_currentLexerName = "pascal";
    } else if (name == "intelhex") {
        m_lexer = new QsciLexerIntelHex(this);
        m_currentLexerName = "intelhex";
    } else if (name == "vhdl") {
        m_lexer = new QsciLexerVHDL(this);
        m_currentLexerName = "vhdl";
    } else if (name == "verilog") {
        m_lexer = new QsciLexerVerilog(this);
        m_currentLexerName = "verilog";
    } else if (name == "avs") {
        m_lexer = new QsciLexerAVS(this);
        m_currentLexerName = "avs";
    } else if (name == "edifact") {
        m_lexer = new QsciLexerEDIFACT(this);
        m_currentLexerName = "edifact";
    } else if (name == "properties") {
        m_lexer = new QsciLexerProperties(this);
        m_currentLexerName = "properties";
    } else if (name == "po") {
        m_lexer = new QsciLexerPO(this);
        m_currentLexerName = "po";
    } else if (name == "coffeescript") {
        m_lexer = new QsciLexerCoffeeScript(this);
        m_currentLexerName = "coffeescript";
    } else if (name == "octave") {
        m_lexer = new QsciLexerOctave(this);
        m_currentLexerName = "octave";
    } else if (name == "none") {
        setLexer(nullptr);
        m_currentLexerName = "none";
        return;
    } else {
        m_lexer = new QsciLexerCPP(this);
        m_currentLexerName = "cpp";
    }

    if (m_lexer) {
        m_lexer->setFont(font);
        setLexer(m_lexer);
    }
}

QString CodeEditor::currentLexerName() const
{
    return m_currentLexerName;
}

void CodeEditor::setTempLexer(const QString &name)
{
    m_tempLexerName = name;
    updateLexerFromFile();
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

// Encoding related functions
QString CodeEditor::currentEncoding() const
{
    return m_currentEncoding;
}

void CodeEditor::setCurrentEncoding(const QString &encoding)
{
    m_currentEncoding = encoding;
    emit encodingChanged(m_currentEncoding);
}

bool CodeEditor::reloadWithEncoding(const QString &encoding)
{
    if (m_filePath.isEmpty()) {
        m_currentEncoding = encoding;
        emit encodingChanged(m_currentEncoding);
        return true;
    }
    
    QFile file(m_filePath);
    if (!file.open(QFile::ReadOnly)) {
        return false;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QString content;
    if (encoding == "UTF-8") {
        content = QString::fromUtf8(data);
    } else if (encoding == "UTF-8 BOM") {
        // Skip BOM if present
        if (data.startsWith("\xEF\xBB\xBF")) {
            content = QString::fromUtf8(data.mid(3));
        } else {
            content = QString::fromUtf8(data);
        }
    } else {
        QTextCodec *codec = QTextCodec::codecForName(encoding.toLatin1());
        if (codec) {
            content = codec->toUnicode(data);
        } else {
            content = QString::fromLocal8Bit(data);
        }
    }
    
    QString oldText = text();
    bool wasModified = isModified();
    
    setText(content);
    m_currentEncoding = encoding;
    setModified(wasModified);
    
    emit encodingChanged(m_currentEncoding);
    return true;
}

bool CodeEditor::saveWithEncoding(const QString &filePath, const QString &encoding)
{
    QFile file(filePath);
    if (!file.open(QFile::WriteOnly)) {
        return false;
    }
    
    QString content = text();
    QByteArray data;
    
    if (encoding == "UTF-8") {
        data = content.toUtf8();
    } else if (encoding == "UTF-8 BOM") {
        data = "\xEF\xBB\xBF" + content.toUtf8();
    } else {
        QTextCodec *codec = QTextCodec::codecForName(encoding.toLatin1());
        if (codec) {
            data = codec->fromUnicode(content);
        } else {
            data = content.toLocal8Bit();
        }
    }
    
    file.write(data);
    file.close();
    
    setFilePath(filePath);
    m_currentEncoding = encoding;
    setModified(false);
    
    return true;
}
