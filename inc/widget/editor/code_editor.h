#ifndef CODE_EDITOR_H
#define CODE_EDITOR_H

#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexer.h>
#include <QMap>
#include <QString>

class CodeEditor : public QsciScintilla
{
    Q_OBJECT

public:
    explicit CodeEditor(QWidget *parent = nullptr);
    ~CodeEditor();

    void setFilePath(const QString &path);
    QString filePath() const;

    void setLexerByExtension(const QString &extension);
    void setLexerByName(const QString &name);

    void setUndoDepth(int depth);
    int undoDepth() const;

    QString selectedText() const;
    void insertText(const QString &text);

    void goToLine(int line);
    void findText(const QString &text, bool caseSensitive = false, bool wholeWord = false);
    void replaceText(const QString &find, const QString &replace, bool all = false);
    
    int currentLine() const;
    int currentColumn() const;

signals:
    void filePathChanged(const QString &path);
    void modificationChanged(bool modified);
    void cursorPositionChanged(int line, int column);

private slots:
    void onTextChanged();
    void onCursorPositionChanged(int line, int index);

private:
    void setupEditor();
    void setupLexer();
    void setupMargins();
    void setupFolding();
    void setupAutoCompletion();
    void setupKeyBindings();
    void setupExtensionMap();

    QString m_filePath;
    QMap<QString, QString> m_extensionToLexer;
    QsciLexer *m_currentLexer;
};

#endif // CODE_EDITOR_H