#include "main.h"
#include "widget/editor/code_editor.h"
#include <QDebug>
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
const int SCI_DELETERANGE = 2645;
const int SCI_POSITIONFROMLINE = 2167;
const int SCI_GETCURRENTPOS = 2008;
const int SCI_GETLINEENDPOSITION = 2136;
const int SCI_GETLINECOUNT = 2154;
const int SCI_GETSTYLEAT = 2010;
const int SCI_GETLINEINDENTPOSITION = 2128;
const int SCI_LINEFROMPOSITION = 2166;
const int SCI_BEGINUNDOACTION = 2078;
const int SCI_ENDUNDOACTION = 2079;
const int SCI_INSERTTEXT = 2003;
const int SCI_GETCHARAT = 2007;
const int SCI_WORDSTARTPOSITION = 2266;
const int SCI_WORDENDPOSITION = 2267;

CodeEditor::CodeEditor(QWidget *parent)
    : QsciScintilla(parent),
      m_filePath(""),
      m_lexer(nullptr),
      m_tempLexerName(""),
      m_currentLexerName("cpp"),
      m_currentEncoding("UTF-8"),
      m_manualLexerSet(false),
      m_readOnly(false)
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
    m_extensionToLexer[".go"] = "go";
    m_extensionToLexer[".rs"] = "rust";
    m_extensionToLexer[".scala"] = "scala";
    m_extensionToLexer[".sc"] = "scala";
    m_extensionToLexer[".hs"] = "haskell";
    m_extensionToLexer[".lhs"] = "haskell";
    m_extensionToLexer[".ts"] = "typescript";
    m_extensionToLexer[".tsx"] = "tsx";
    m_extensionToLexer[".mts"] = "typescript";
    m_extensionToLexer[".cts"] = "typescript";
    
    initCommentSyntax();
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
    setMarginMarkerMask(1, 0x1FFFFFFF);  // show all markers in margin 1
    
    // Define bookmark marker (marker number 1) using the built-in Bookmark symbol
    markerDefine(QsciScintilla::Bookmark, 1);
    setMarkerBackgroundColor(QColor(0, 120, 215), 1);   // Blue bookmark
    setMarkerForegroundColor(QColor(255, 255, 255), 1);  // White foreground
    
    // Define breakpoint marker (marker number 2) using a red circle
    markerDefine(QsciScintilla::Circle, 2);
    setMarkerBackgroundColor(QColor(220, 50, 50), 2);    // Red circle
    setMarkerForegroundColor(QColor(255, 255, 255), 2);  // White foreground
    
    setLexerByName("cpp");
}

void CodeEditor::setupConnections()
{
    connect(this, &QsciScintilla::textChanged, this, &CodeEditor::onTextChanged);
    // Note: QsciScintilla::cursorPositionChanged is already emitted by parent class
    // We don't need to connect it here, MainWindow will connect directly
}

void CodeEditor::initCommentSyntax()
{
    // C-family: // and /* */
    m_commentSyntax["cpp"] = {"//", "/*", "*/"};
    m_commentSyntax["javascript"] = {"//", "/*", "*/"};
    m_commentSyntax["java"] = {"//", "/*", "*/"};
    m_commentSyntax["csharp"] = {"//", "/*", "*/"};
    m_commentSyntax["d"] = {"//", "/*", "*/"};
    m_commentSyntax["idl"] = {"//", "/*", "*/"};
    m_commentSyntax["pascal"] = {"//", "{", "}"};
    m_commentSyntax["php"] = {"//", "/*", "*/"};
    m_commentSyntax["verilog"] = {"//", "/*", "*/"};
    m_commentSyntax["pov"] = {"//", "/*", "*/"};
    m_commentSyntax["avs"] = {"//", "/*", "*/"};

    // Shell-style: #
    m_commentSyntax["python"] = {"#", "\"\"\"", "\"\"\""};
    m_commentSyntax["bash"] = {"#", "", ""};
    m_commentSyntax["perl"] = {"#", "", ""};
    m_commentSyntax["ruby"] = {"#", "=begin", "=end"};
    m_commentSyntax["yaml"] = {"#", "", ""};
    m_commentSyntax["makefile"] = {"#", "", ""};
    m_commentSyntax["cmake"] = {"#", "", ""};
    m_commentSyntax["tcl"] = {"#", "", ""};
    m_commentSyntax["coffeescript"] = {"#", "###", "###"};
    m_commentSyntax["properties"] = {"#", "", ""};
    m_commentSyntax["po"] = {"#", "", ""};

    // SQL: --
    m_commentSyntax["sql"] = {"--", "/*", "*/"};

    // Lua: --
    m_commentSyntax["lua"] = {"--", "--[[", "]]"};

    // HTML/XML: <!-- -->
    m_commentSyntax["html"] = {"", "<!--", "-->"};
    m_commentSyntax["xml"] = {"", "<!--", "-->"};

    // CSS: /* */
    m_commentSyntax["css"] = {"", "/*", "*/"};

    // TeX: %
    m_commentSyntax["tex"] = {"%", "", ""};

    // Fortran: !
    m_commentSyntax["fortran"] = {"!", "/*", "*/"};
    m_commentSyntax["fortran77"] = {"c", "/*", "*/"};

    // MATLAB: %
    m_commentSyntax["matlab"] = {"%", "%{", "%}"};

    // Assembly: ;
    m_commentSyntax["asm"] = {";", "", ""};
    m_commentSyntax["spice"] = {";", "", ""};

    // VHDL: --
    m_commentSyntax["vhdl"] = {"--", "", ""};

    // Batch: REM
    m_commentSyntax["batch"] = {"REM ", "", ""};

    // PostScript: %
    m_commentSyntax["postscript"] = {"%", "", ""};

    // Default fallback for unknown languages: C-style
}

CodeEditor::CommentSyntax CodeEditor::commentSyntaxForLanguage(const QString &lang) const
{
    if (m_commentSyntax.contains(lang)) {
        return m_commentSyntax[lang];
    }
    // Default to C-style for unknown languages
    return {"//", "/*", "*/"};
}

QString CodeEditor::textRange(int start, int end) const
{
    return QsciScintilla::text(start, end);
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
    
    // If user has manually set the lexer, don't override
    if (m_manualLexerSet && !m_tempLexerName.isEmpty()) {
        setLexerByName(m_tempLexerName);
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
    } else if (name == "php") {
        // No QsciLexerPHP, use CPP as fallback
        m_lexer = new QsciLexerCPP(this);
        m_currentLexerName = "php";
    } else if (name == "go") {
        m_lexer = new QsciLexerCPP(this);
        m_currentLexerName = "go";
    } else if (name == "rust") {
        m_lexer = new QsciLexerCPP(this);
        m_currentLexerName = "rust";
    } else if (name == "scala") {
        m_lexer = new QsciLexerCPP(this);
        m_currentLexerName = "scala";
    } else if (name == "haskell") {
        m_lexer = new QsciLexerCPP(this);
        m_currentLexerName = "haskell";
    } else if (name == "typescript") {
        m_lexer = new QsciLexerJavaScript(this);
        m_currentLexerName = "typescript";
    } else if (name == "tsx") {
        m_lexer = new QsciLexerJavaScript(this);
        m_currentLexerName = "tsx";
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
    m_manualLexerSet = true; // Mark that user has manually set the lexer
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

int CodeEditor::currentLine() const
{
    int line, index;
    getCursorPosition(&line, &index);
    return line;
}

int CodeEditor::currentColumn() const
{
    int line, index;
    getCursorPosition(&line, &index);
    return index;
}

void CodeEditor::toggleBookmark(int line)
{
    if (markersAtLine(line) & (1 << 1)) {
        markerDelete(line, 1);
    } else {
        markerAdd(line, 1);
    }
}

void CodeEditor::clearAllBookmarks()
{
    markerDeleteAll(1);
}

bool CodeEditor::hasBookmark(int line) const
{
    return (markersAtLine(line) & (1 << 1)) != 0;
}

int CodeEditor::findNextBookmark(int line) const
{
    // markerFindNext starts from line+1 by default
    int next = const_cast<CodeEditor*>(this)->markerFindNext(line, 1 << 1);
    return next;
}

int CodeEditor::findPreviousBookmark(int line) const
{
    int prev = const_cast<CodeEditor*>(this)->markerFindPrevious(line, 1 << 1);
    return prev;
}

QList<int> CodeEditor::allBookmarkLines() const
{
    QList<int> result;
    int totalLines = QsciScintilla::lines();
    for (int i = 0; i < totalLines; ++i) {
        if (hasBookmark(i)) {
            result.append(i);
        }
    }
    return result;
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

// ============================================================
// Comment / Uncomment  —  业界标准做法：
//   位置匹配 (SCI_GETLINEINDENTPOSITION) + 字符匹配 (mid/startsWith) + 范围删除 (SCI_DELETERANGE)
//   菜单只有 Comment/Uncomment 两个入口，各自按语言分别处理行注释与块注释
// ============================================================

void CodeEditor::commentLine()
{
    CommentSyntax syntax = commentSyntaxForLanguage(m_currentLexerName);

    // 优先行注释，没有则用块注释
    if (!syntax.line.isEmpty()) {
        applyLineComment(syntax.line);
    } else if (!syntax.blockStart.isEmpty() && !syntax.blockEnd.isEmpty()) {
        applyBlockComment(syntax.blockStart, syntax.blockEnd);
    }
}

void CodeEditor::uncommentLine()
{
    CommentSyntax syntax = commentSyntaxForLanguage(m_currentLexerName);

    // 先尝试移除块注释 (通过样式检测或文本搜索)
    if (!syntax.blockStart.isEmpty() && !syntax.blockEnd.isEmpty()) {
        if (tryRemoveBlockComment(syntax.blockStart, syntax.blockEnd))
            return;
    }

    // 再尝试移除行注释
    if (!syntax.line.isEmpty()) {
        removeLineComment(syntax.line);
    }
}

// ---------- 私有辅助函数 ----------

void CodeEditor::applyLineComment(const QString &delim)
{
    int startLine, endLine;
    if (hasSelectedText()) {
        int startCol, endCol;
        getSelection(&startLine, &startCol, &endLine, &endCol);
        // 如果选区末尾恰在行首 (只选了前一行尾的换行)，不包含该行
        if (endCol == 0 && endLine > startLine)
            endLine--;
    } else {
        int line, col;
        getCursorPosition(&line, &col);
        startLine = endLine = line;
    }

    SendScintilla(SCI_BEGINUNDOACTION);
    for (int line = startLine; line <= endLine; ++line) {
        int indentPos = SendScintilla(SCI_GETLINEINDENTPOSITION, line);
        // 双重验证：位置 + 字符匹配，避免重复注释
        int lineEnd = SendScintilla(SCI_GETLINEENDPOSITION, line);
        QString lineText = textRange(indentPos, lineEnd);
        if (!lineText.startsWith(delim)) {
            SendScintilla(SCI_INSERTTEXT, indentPos, delim.toUtf8().constData());
        }
    }
    SendScintilla(SCI_ENDUNDOACTION);
}

void CodeEditor::removeLineComment(const QString &delim)
{
    int startLine, endLine;
    if (hasSelectedText()) {
        int startCol, endCol;
        getSelection(&startLine, &startCol, &endLine, &endCol);
        if (endCol == 0 && endLine > startLine)
            endLine--;
    } else {
        int line, col;
        getCursorPosition(&line, &col);
        startLine = endLine = line;
    }

    SendScintilla(SCI_BEGINUNDOACTION);
    // 倒序遍历，保证删除后行号不变
    for (int line = endLine; line >= startLine; --line) {
        int indentPos = SendScintilla(SCI_GETLINEINDENTPOSITION, line);
        int lineEnd = SendScintilla(SCI_GETLINEENDPOSITION, line);
        QString lineText = textRange(indentPos, lineEnd);
        // 位置匹配 + 字符匹配：在缩进位置检测注释符
        if (lineText.startsWith(delim)) {
            SendScintilla(SCI_DELETERANGE, indentPos, delim.length());
        }
    }
    SendScintilla(SCI_ENDUNDOACTION);
}

void CodeEditor::applyBlockComment(const QString &open, const QString &close)
{
    int selStart, selEnd;
    if (hasSelectedText()) {
        int sLine, sCol, eLine, eCol;
        getSelection(&sLine, &sCol, &eLine, &eCol);
        selStart = positionFromLineIndex(sLine, sCol);
        selEnd   = positionFromLineIndex(eLine, eCol);
    } else {
        // 无选区：包裹光标所在单词
        selStart = SendScintilla(SCI_GETCURRENTPOS);
        // 尝试扩展到一个单词
        int wordStart = SendScintilla(SCI_WORDSTARTPOSITION, selStart, true);
        int wordEnd   = SendScintilla(SCI_WORDENDPOSITION, selStart, true);
        if (wordStart < wordEnd) {
            selStart = wordStart;
            selEnd   = wordEnd;
        }
    }

    SendScintilla(SCI_BEGINUNDOACTION);
    // 先插 close (不改变 open 位置)，再插 open
    SendScintilla(SCI_INSERTTEXT, selEnd, close.toUtf8().constData());
    SendScintilla(SCI_INSERTTEXT, selStart, open.toUtf8().constData());
    SendScintilla(SCI_ENDUNDOACTION);
}

bool CodeEditor::tryRemoveBlockComment(const QString &open, const QString &close)
{
    int checkStart, checkEnd;

    if (hasSelectedText()) {
        int sLine, sCol, eLine, eCol;
        getSelection(&sLine, &sCol, &eLine, &eCol);
        checkStart = positionFromLineIndex(sLine, sCol);
        checkEnd   = positionFromLineIndex(eLine, eCol);
    } else {
        checkStart = SendScintilla(SCI_GETCURRENTPOS);
        checkEnd   = checkStart;
    }

    // 方法1: 样式检测 — 如果光标/选区位置处于注释样式中
    int styleAtStart = SendScintilla(SCI_GETSTYLEAT, checkStart) & 0x1f;
    int styleAtEnd   = SendScintilla(SCI_GETSTYLEAT, checkEnd) & 0x1f;

    // 注释样式的值因语言而异，通用做法是检查文本匹配
    // 方法2: 在选区前后搜索块注释分隔符 (优先)
    QString fullText = QsciScintilla::text();

    // 向前搜索 open (从选区起点往前 open.length() 开始搜)
    int openPos = fullText.lastIndexOf(open, checkStart + open.length() - 1);
    // 向后搜索 close (从选区终点往后 close.length() 开始搜)
    int closePos = fullText.indexOf(close, checkEnd - close.length() + 1);

    if (openPos == -1 || closePos == -1 || openPos >= closePos)
        return false;

    // 确认 open 和 close 之间包含选区/光标
    if (checkStart >= openPos && checkEnd <= closePos + close.length()) {
        SendScintilla(SCI_BEGINUNDOACTION);
        SendScintilla(SCI_DELETERANGE, closePos, close.length());
        SendScintilla(SCI_DELETERANGE, openPos, open.length());
        SendScintilla(SCI_ENDUNDOACTION);
        return true;
    }

    return false;
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

// Read-only support
void CodeEditor::setReadOnly(bool readOnly)
{
    m_readOnly = readOnly;
    QsciScintilla::setReadOnly(readOnly);
}

bool CodeEditor::isReadOnly() const
{
    return m_readOnly;
}
