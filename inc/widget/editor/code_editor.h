#ifndef CODE_EDITOR_H
#define CODE_EDITOR_H

#include "main.h"
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexer.h>
#include <Qsci/qsciprinter.h>

class CodeEditor : public QsciScintilla
{
    Q_OBJECT

public:
    explicit CodeEditor(QWidget *parent = nullptr);
    ~CodeEditor();

    void setFilePath(const QString &path);
    QString filePath() const;
    
    bool loadFile(const QString &filePath);
    bool saveFile(const QString &filePath = QString());
    
    // Encoding support
    QString currentEncoding() const;
    void setCurrentEncoding(const QString &encoding);
    bool reloadWithEncoding(const QString &encoding);
    bool saveWithEncoding(const QString &filePath, const QString &encoding);

    void setLexerByName(const QString &name);
    QString currentLexerName() const;
    void setTempLexer(const QString &name);

    QString selectedText() const;
    void insertText(const QString &text);

    void goToLine(int line);
    void findText(const QString &text, bool caseSensitive = false, bool wholeWord = false);
    void replaceText(const QString &find, const QString &replace, bool all = false);
    
    int currentLine() const;
    int currentColumn() const;
    
    void toggleBookmark(int line);
    void clearAllBookmarks();
    void toggleBreakpoint(int line);
    void clearBreakpoint(int line);
    void clearAllBreakpoints();
    
    void commentLine();
    void uncommentLine();
    void deleteChar();
    
    // Read-only support
    void setReadOnly(bool readOnly);
    bool isReadOnly() const;

signals:
    void filePathChanged(const QString &path);
    void modificationChanged(bool modified);
    void encodingChanged(const QString &encoding);

private slots:
    void onTextChanged();

private:
    // Comment syntax definition per language
    struct CommentSyntax {
        QString line;        // Line comment delimiter, e.g. "//", "#"
        QString blockStart;  // Block comment start, e.g. "/*", "<!--"
        QString blockEnd;    // Block comment end, e.g. "*/", "-->"
    };

    void setupEditor();
    void setupConnections();
    void updateLexerFromFile();
    void initCommentSyntax();
    CommentSyntax commentSyntaxForLanguage(const QString &lang) const;

    // Comment/uncomment helpers
    void applyLineComment(const QString &delim);
    void removeLineComment(const QString &delim);
    void applyBlockComment(const QString &open, const QString &close);
    bool tryRemoveBlockComment(const QString &open, const QString &close);
    QString textRange(int start, int end) const;

    QString m_filePath;
    QsciLexer *m_lexer;
    QString m_tempLexerName;
    QString m_currentLexerName; // Current lexer name for tracking
    QMap<QString, QString> m_extensionToLexer;
    QMap<QString, CommentSyntax> m_commentSyntax;
    QString m_currentEncoding; // Current file encoding
    bool m_manualLexerSet; // Whether user has manually set the lexer
    bool m_readOnly; // Whether the editor is in read-only mode
};

#endif // CODE_EDITOR_H
