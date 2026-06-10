#ifndef CODE_EDITOR_H
#define CODE_EDITOR_H

#include "main.h"
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexer.h>

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

    void setLexerByName(const QString &name);

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
    void commentBlock();
    void uncommentBlock();
    void deleteChar();

signals:
    void filePathChanged(const QString &path);
    void modificationChanged(bool modified);
    void cursorPositionChanged(int line, int column);

private slots:
    void onTextChanged();
    void onCursorPositionChanged(int line, int index);

private:
    void setupEditor();
    void setupConnections();
    void updateLexerFromFile();

    QString m_filePath;
    QsciLexer *m_lexer;
    QMap<QString, QString> m_extensionToLexer;
};

#endif // CODE_EDITOR_H
