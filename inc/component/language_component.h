#ifndef LANGUAGE_COMPONENT_H
#define LANGUAGE_COMPONENT_H

#include <QString>
#include <QStringList>
#include <QList>

// ---------------------------------------------------------------------------
// Data structures
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
    QString name;
    int     line;       // 0-based
    int     startByte;
    int     endByte;
    int     depth;
};

struct SegmentInfo {
    QString typeName;
    QString summary;
    int     startLine;
    int     endLine;
    int     startByte;
    int     endByte;
    QList<QPair<QString, QString>> properties;
};

// ---------------------------------------------------------------------------
// Capabilities
// ---------------------------------------------------------------------------

enum Capability {
    CapNone          = 0,
    CapSymbolOutline = 1 << 0,
    CapSegmentInfo   = 1 << 1,
    CapDiagnostics   = 1 << 2,
    CapCompletion    = 1 << 3,
    CapHover         = 1 << 4
};

Q_DECLARE_FLAGS(Capabilities, Capability)

// ---------------------------------------------------------------------------
// Completion item
// ---------------------------------------------------------------------------

struct CompletionItem {
    QString text;
    QString displayText;
    QString detail;       // type info, signature
    QString kind;         // "function", "class", "variable", "keyword", etc.
    int priority;         // higher = more likely
};

// ---------------------------------------------------------------------------
// ILanguageComponent - interface for all language analysis components
// ---------------------------------------------------------------------------

class ILanguageComponent
{
public:
    virtual ~ILanguageComponent() = default;

    // ---- Metadata ----
    virtual QString componentName() const = 0;
    virtual QString componentVersion() const = 0;
    virtual QString apiVersion() const = 0;

    // ---- Language support ----
    virtual bool supportsLanguage(const QString &lexerName) const = 0;
    virtual QStringList supportedLanguages() const = 0;
    virtual Capabilities capabilities() const = 0;

    // ---- Navigator: symbol outline ----
    virtual QList<SymbolInfo> parseSymbols(const QString &source,
                                            const QString &lexerName) = 0;

    // ---- Segment: cursor context info ----
    virtual SegmentInfo getSegmentAt(const QString &source,
                                      const QString &lexerName,
                                      int byteOffset) = 0;
    virtual QList<SegmentInfo> getErrorNodes(const QString &source,
                                              const QString &lexerName) = 0;

    // ---- Diagnostics: static real-time checking (optional) ----
    // Returns list of (line, column, message, severity)
    // severity: 0=info, 1=warning, 2=error
    virtual QList<QPair<int, QPair<QString, int>>> getDiagnostics(
        const QString &source, const QString &lexerName)
    {
        Q_UNUSED(source); Q_UNUSED(lexerName);
        return {};
    }

    // ---- Auto-completion (optional) ----
    virtual QList<CompletionItem> getCompletions(
        const QString &source, const QString &lexerName,
        int line, int column, const QString &prefix)
    {
        Q_UNUSED(source); Q_UNUSED(lexerName);
        Q_UNUSED(line); Q_UNUSED(column); Q_UNUSED(prefix);
        return {};
    }

    // ---- Instantiation assistant (optional) ----
    // e.g. Verilog module instantiation: generate instance code for a module
    virtual QString generateInstantiation(const QString &source,
                                           const QString &lexerName,
                                           const QString &symbolName)
    {
        Q_UNUSED(source); Q_UNUSED(lexerName); Q_UNUSED(symbolName);
        return QString();
    }
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Capabilities)

// Factory function signature for DLL plugins
typedef ILanguageComponent* (*ComponentFactoryFunc)();

#endif // LANGUAGE_COMPONENT_H
