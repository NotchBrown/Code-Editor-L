#ifndef TREE_SITTER_MANAGER_H
#define TREE_SITTER_MANAGER_H

#include "main.h"
#include <QLibrary>
#include <tree_sitter/api.h>

class CodeEditor;

// ---------------------------------------------------------------------------
// Data structures for parsed symbols
// ---------------------------------------------------------------------------

struct SymbolInfo {
    enum Type {
        Function,
        Method,
        Class,
        Struct,
        Enum,
        Union,
        Namespace,
        Variable,
        Typedef,
        Macro,
        Interface,
        Module,
        Unknown
    };

    Type    type;
    QString name;       // symbol name (or nearby text if unnamed)
    int     line;       // 0-based line where symbol starts
    int     startByte;  // byte offset in document
    int     endByte;    // end byte offset
    int     depth;      // nesting depth
};

struct SegmentInfo {
    QString typeName;       // e.g. "if_statement", "function_definition"
    QString summary;        // human-readable summary
    int     startLine;
    int     endLine;
    int     startByte;
    int     endByte;
    QList<QPair<QString, QString>> properties;  // key-value pairs
};

// ---------------------------------------------------------------------------
// TreeSitterManager - singleton that wraps tree-sitter parsing
// ---------------------------------------------------------------------------

class TreeSitterManager : public QObject
{
    Q_OBJECT

public:
    static TreeSitterManager* instance();

    // Check if tree-sitter supports a given lexer name
    bool supportsLanguage(const QString &lexerName) const;

    // Parse source code and extract symbols for Navigator
    QList<SymbolInfo> parseSymbols(const QString &source, const QString &lexerName);

    // Get segment info at a given byte offset
    SegmentInfo getSegmentAt(const QString &source, const QString &lexerName, int byteOffset);

    // Get error nodes (for static checking)
    QList<SegmentInfo> getErrorNodes(const QString &source, const QString &lexerName);

private:
    TreeSitterManager();
    ~TreeSitterManager();

    // Load grammar DLL for a language
    const TSLanguage* loadLanguage(const QString &lexerName);

    // Create parser for a language
    TSParser* getParser(const QString &lexerName);

    // Query-based symbol extraction (VS Code style)
    void extractSymbolsViaQuery(const QString &source, const QString &lexerName,
                                QList<SymbolInfo> &symbols);

    // Get query string for a language (based on official tags.scm)
    static QString tagsQueryForLanguage(const QString &lexerName);

    // Map query capture name to SymbolInfo::Type
    static SymbolInfo::Type captureToSymbolType(const char *captureName);

    // Try to extract symbol name from a query match (uses UTF-8 byte offsets)
    static QString extractSymbolName(const TSQueryMatch &match,
                                      const TSQuery *query,
                                      const QByteArray &utf8Source);

    // Get human-readable name for a symbol type
    static QString symbolTypeName(SymbolInfo::Type t);

    // Language name -> grammar DLL path mapping
    static QString grammarPath(const QString &lexerName);

    // Registered language -> TSLanguage mapping
    QMap<QString, const TSLanguage*> m_languages;
    QMap<QString, TSParser*> m_parsers;
    QMap<QString, QLibrary*> m_libraries;
    QMap<QString, TSQuery*> m_queries;       // cached queries

    static TreeSitterManager* m_instance;
};

#endif // TREE_SITTER_MANAGER_H
