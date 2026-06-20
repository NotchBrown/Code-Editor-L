// addon_template.cpp — Per-language tree-sitter addon
// Compile with -DLANG_NAME=xxx where xxx is one of:
// c, cpp, python, javascript, bash, java, csharp, ruby, html, css, json, verilog

#include <QString>
#include <QStringList>
#include <QDebug>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QLibrary>
#include <tree_sitter/api.h>
#include "component/language_component.h"

// =========================================================================
// Language-specific configuration
// =========================================================================

#ifndef LANG_NAME
#error "LANG_NAME must be defined"
#endif

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

static QString langName() { return TOSTRING(LANG_NAME); }

// Map language name to grammar DLL name and function
static QString grammarDllName()
{
    QString n = langName();
    if (n == "csharp") return "ts_c-sharp";
    if (n == "php") return "ts_php";
    if (n == "php_only") return "ts_php_only";
    return "ts_" + n;
}

static QString grammarFuncName()
{
    QString n = langName();
    if (n == "csharp") return "tree_sitter_c_sharp";
    if (n == "php") return "tree_sitter_php";
    if (n == "php_only") return "tree_sitter_php_only";
    return "tree_sitter_" + n;
}

// =========================================================================
// Query rules — only include those for this language
// =========================================================================

static QString queryString()
{
    QString n = langName();

    if (n == "c") {
        return R"(
(struct_specifier (type_identifier) @name (field_declaration_list)) @definition.struct
(union_specifier (type_identifier) @name (field_declaration_list)) @definition.union
(function_definition declarator: (function_declarator declarator: (identifier) @name)) @definition.function
(type_definition (type_identifier) @name) @definition.type
(enum_specifier (type_identifier) @name (enumerator_list)) @definition.enum
(preproc_def (identifier) @name) @definition.macro
(preproc_function_def (identifier) @name) @definition.macro
)";
    }
    if (n == "cpp") {
        return R"(
(function_definition declarator: (function_declarator declarator: (identifier) @name)) @definition.function
(function_definition declarator: (function_declarator declarator: (qualified_identifier name: (identifier) @name))) @definition.function
(class_specifier (type_identifier) @name (field_declaration_list)) @definition.class
(struct_specifier (type_identifier) @name (field_declaration_list)) @definition.struct
(union_specifier (type_identifier) @name (field_declaration_list)) @definition.union
(enum_specifier (type_identifier) @name (enumerator_list)) @definition.enum
(type_definition (type_identifier) @name) @definition.type
(namespace_definition (namespace_identifier) @name) @definition.namespace
)";
    }
    if (n == "python") {
        return R"(
(class_definition (identifier) @name) @definition.class
(function_definition (identifier) @name) @definition.function
(module (assignment left: (identifier) @name)) @definition.variable
)";
    }
    if (n == "javascript") {
        return R"(
(method_definition (property_identifier) @name) @definition.method
(class_declaration name: (_) @name) @definition.class
(function_declaration (identifier) @name) @definition.function
(generator_function_declaration (identifier) @name) @definition.function
(program (lexical_declaration (variable_declarator (identifier) @name))) @definition.variable
(program (variable_declaration (variable_declarator (identifier) @name))) @definition.variable
)";
    }
    if (n == "bash") {
        return R"(
(function_definition name: (word) @name) @definition.function
)";
    }
    if (n == "java") {
        return R"(
(class_declaration (identifier) @name) @definition.class
(interface_declaration (identifier) @name) @definition.interface
(enum_declaration (identifier) @name) @definition.enum
(method_declaration (identifier) @name) @definition.method
(field_declaration (variable_declarator (identifier) @name)) @definition.variable
(constructor_declaration (identifier) @name) @definition.method
)";
    }
    if (n == "csharp") {
        return R"(
(class_declaration (identifier) @name) @definition.class
(struct_declaration (identifier) @name) @definition.struct
(interface_declaration (identifier) @name) @definition.interface
(enum_declaration (identifier) @name) @definition.enum
(method_declaration (identifier) @name) @definition.method
(constructor_declaration (identifier) @name) @definition.method
(property_declaration (identifier) @name) @definition.variable
(namespace_declaration (identifier) @name) @definition.namespace
)";
    }
    if (n == "ruby") {
        return R"(
(class (constant) @name) @definition.class
(module (constant) @name) @definition.namespace
(method (identifier) @name) @definition.function
(singleton_method (identifier) @name) @definition.function
)";
    }
    if (n == "json") {
        return R"(
(document (object (pair key: (string (string_content) @name)))) @definition.variable
)";
    }
    if (n == "verilog") {
        return R"(
(module_header (simple_identifier) @name) @definition.class
)";
    }
    if (n == "php" || n == "php_only") {
        return R"(
(namespace_definition (namespace_name) @name) @definition.module
(interface_declaration name: (name) @name) @definition.interface
(trait_declaration name: (name) @name) @definition.interface
(class_declaration name: (name) @name) @definition.class
(function_definition name: (name) @name) @definition.function
(method_declaration name: (name) @name) @definition.function
)";
    }
    if (n == "go") {
        return R"(
(function_declaration (identifier) @name) @definition.function
(method_declaration (field_identifier) @name) @definition.method
(type_spec (type_identifier) @name) @definition.type
)";
    }
    if (n == "rust") {
        return R"(
(declaration_list (function_item (identifier) @name)) @definition.method
(function_item (identifier) @name) @definition.function
(struct_item (type_identifier) @name) @definition.class
(enum_item (type_identifier) @name) @definition.class
(union_item (type_identifier) @name) @definition.class
(type_item (type_identifier) @name) @definition.class
(trait_item (type_identifier) @name) @definition.interface
(mod_item (identifier) @name) @definition.module
(macro_definition (identifier) @name) @definition.macro
)";
    }
    if (n == "scala") {
        return R"(
(package_clause (package_identifier) @name) @definition.module
(trait_definition (identifier) @name) @definition.interface
(enum_definition (identifier) @name) @definition.enum
(class_definition (identifier) @name) @definition.class
(object_definition (identifier) @name) @definition.class
(function_definition (identifier) @name) @definition.function
(val_definition (identifier) @name) @definition.variable
(var_definition (identifier) @name) @definition.variable
(type_definition (type_identifier) @name) @definition.type
)";
    }
    if (n == "haskell") {
        return R"(
(module (module_id) @name) @definition.module
(decl name: (variable) @name) @definition.function
)";
    }
    if (n == "typescript" || n == "tsx") {
        return R"(
(function_signature (identifier) @name) @definition.function
(method_signature (property_identifier) @name) @definition.method
(abstract_method_signature (property_identifier) @name) @definition.method
(abstract_class_declaration (type_identifier) @name) @definition.class
(interface_declaration (type_identifier) @name) @definition.interface
(module (identifier) @name) @definition.module
(function_declaration (identifier) @name) @definition.function
(class_declaration name: (_) @name) @definition.class
(method_definition (property_identifier) @name) @definition.method
)";
    }
    // html, css — queries need AST investigation, return empty for fallback
    return QString();
}

// =========================================================================
// Capture → SymbolInfo::Type mapping
// =========================================================================

static SymbolInfo::Type captureToType(const char *name)
{
    if (!name) return SymbolInfo::Unknown;
    QString n(name);
    if (n == "definition.class")     return SymbolInfo::Class;
    if (n == "definition.struct")    return SymbolInfo::Struct;
    if (n == "definition.function")  return SymbolInfo::Function;
    if (n == "definition.method")    return SymbolInfo::Method;
    if (n == "definition.type")      return SymbolInfo::Typedef;
    if (n == "definition.namespace") return SymbolInfo::Namespace;
    if (n == "definition.variable")  return SymbolInfo::Variable;
    if (n == "definition.macro")     return SymbolInfo::Macro;
    if (n == "definition.enum")      return SymbolInfo::Enum;
    if (n == "definition.union")     return SymbolInfo::Union;
    if (n == "definition.interface") return SymbolInfo::Interface;
    if (n == "definition.module")    return SymbolInfo::Module;
    return SymbolInfo::Unknown;
}

static QString extractBytes(const QByteArray &utf8, uint32_t s, uint32_t e)
{
    if (e <= s || s >= (uint32_t)utf8.size()) return {};
    return QString::fromUtf8(utf8.constData() + s, qMin(e - s, (uint32_t)(utf8.size() - s)));
}

// =========================================================================
// Component implementation
// =========================================================================

class LangComponent : public ILanguageComponent
{
    QLibrary *m_lib = nullptr;
    const TSLanguage *m_lang = nullptr;
    TSParser *m_parser = nullptr;
    TSQuery *m_query = nullptr;

public:
    LangComponent();
    ~LangComponent() override;

    QString componentName() const override    { return "ts_" + langName(); }
    QString componentVersion() const override { return "1.0.0"; }
    QString apiVersion() const override       { return "1.0"; }
    bool supportsLanguage(const QString &l) const override { return l == langName(); }
    QStringList supportedLanguages() const override { return {langName()}; }
    Capabilities capabilities() const override {
        return queryString().isEmpty() ? Capabilities()
             : Capabilities(CapSymbolOutline | CapSegmentInfo);
    }

    QList<SymbolInfo> parseSymbols(const QString &src, const QString &lang) override;
    SegmentInfo getSegmentAt(const QString &src, const QString &lang, int off) override;
    QList<SegmentInfo> getErrorNodes(const QString &src, const QString &lang) override;
};

LangComponent::LangComponent()
{
    QStringList searchPaths;
    searchPaths << QCoreApplication::applicationDirPath() + "/../addons/" + componentName() + "/grammars";
    searchPaths << QCoreApplication::applicationDirPath() + "/../addons/" + componentName();
    searchPaths << QCoreApplication::applicationDirPath() + "/grammars";

    QString fullPath;
    QString dllName = grammarDllName() + ".dll";
    for (const auto &p : searchPaths) {
        QString fp = p + "/" + dllName;
        if (QFile::exists(fp)) { fullPath = fp; break; }
    }

    if (fullPath.isEmpty()) {
        qWarning() << "[Addon:" << componentName() << "] grammar DLL not found:" << dllName;
        return;
    }

    m_lib = new QLibrary(fullPath);
    if (!m_lib->load()) {
        qWarning() << "[Addon:" << componentName() << "] load failed:" << m_lib->errorString();
        delete m_lib; m_lib = nullptr; return;
    }

    auto func = reinterpret_cast<const TSLanguage*(*)()>(
        m_lib->resolve(grammarFuncName().toLatin1().constData()));
    if (!func) {
        qWarning() << "[Addon:" << componentName() << "] symbol not found:" << grammarFuncName();
        m_lib->unload(); delete m_lib; m_lib = nullptr; return;
    }

    m_lang = func();
    m_parser = ts_parser_new();
    if (m_parser) ts_parser_set_language(m_parser, m_lang);

    // Compile query
    QString qs = queryString();
    if (!qs.isEmpty()) {
        uint32_t errOff; TSQueryError errType;
        QByteArray qb = qs.toUtf8();
        m_query = ts_query_new(m_lang, qb.constData(), qb.length(), &errOff, &errType);
        if (!m_query)
            qWarning() << "[Addon:" << componentName() << "] query compile error" << errType;
    }

    qDebug() << "[Addon:" << componentName() << "] loaded";
}

LangComponent::~LangComponent()
{
    if (m_query) ts_query_delete(m_query);
    if (m_parser) ts_parser_delete(m_parser);
    if (m_lib) { m_lib->unload(); delete m_lib; }
}

QList<SymbolInfo> LangComponent::parseSymbols(const QString &source, const QString &)
{
    QList<SymbolInfo> symbols;
    if (source.isEmpty() || !m_parser || !m_query) return symbols;

    QByteArray utf8 = source.toUtf8();
    TSTree *tree = ts_parser_parse_string(m_parser, nullptr, utf8.constData(), utf8.length());
    if (!tree) return symbols;

    TSNode root = ts_tree_root_node(tree);
    TSQueryCursor *cursor = ts_query_cursor_new();
    ts_query_cursor_exec(cursor, m_query, root);

    TSQueryMatch match;
    while (ts_query_cursor_next_match(cursor, &match)) {
        SymbolInfo::Type symType = SymbolInfo::Unknown;
        for (uint32_t i = 0; i < match.capture_count; ++i) {
            uint32_t len;
            const char *cn = ts_query_capture_name_for_id(m_query, match.captures[i].index, &len);
            SymbolInfo::Type t = captureToType(cn);
            if (t != SymbolInfo::Unknown) { symType = t; break; }
        }
        if (symType == SymbolInfo::Unknown) continue;

        SymbolInfo info;
        info.type = symType;
        for (uint32_t i = 0; i < match.capture_count; ++i) {
            uint32_t len;
            const char *cn = ts_query_capture_name_for_id(m_query, match.captures[i].index, &len);
            if (cn && strcmp(cn, "name") == 0) {
                uint32_t s = ts_node_start_byte(match.captures[i].node);
                uint32_t e = ts_node_end_byte(match.captures[i].node);
                info.name = extractBytes(utf8, s, e);
                break;
            }
        }
        if (match.capture_count > 0) {
            TSNode n = match.captures[0].node;
            info.startByte = ts_node_start_byte(n);
            info.endByte = ts_node_end_byte(n);
            info.line = ts_node_start_point(n).row;
        }
        symbols.append(info);
    }

    ts_query_cursor_delete(cursor);
    ts_tree_delete(tree);
    return symbols;
}

SegmentInfo LangComponent::getSegmentAt(const QString &source, const QString &, int byteOffset)
{
    SegmentInfo info;
    if (source.isEmpty() || !m_parser) return info;
    QByteArray utf8 = source.toUtf8();
    TSTree *tree = ts_parser_parse_string(m_parser, nullptr, utf8.constData(), utf8.length());
    if (!tree) return info;
    TSNode root = ts_tree_root_node(tree);
    TSNode node = ts_node_descendant_for_byte_range(root, (uint32_t)byteOffset, (uint32_t)byteOffset);
    if (!ts_node_is_null(node)) {
        TSNode t = node;
        while (!ts_node_is_null(t)) {
            QString type(ts_node_type(t));
            if (type.contains("function") || type.contains("if_") || type.contains("while_")
                || type.contains("for_") || type.contains("class_") || type.contains("struct_")
                || type.contains("module_")) {
                info.typeName = type;
                info.startByte = ts_node_start_byte(t);
                info.endByte = ts_node_end_byte(t);
                info.startLine = ts_node_start_point(t).row;
                info.endLine = ts_node_end_point(t).row;
                break;
            }
            t = ts_node_parent(t);
        }
    }
    ts_tree_delete(tree);
    return info;
}

QList<SegmentInfo> LangComponent::getErrorNodes(const QString &source, const QString &)
{
    QList<SegmentInfo> errors;
    if (source.isEmpty() || !m_parser) return errors;
    QByteArray utf8 = source.toUtf8();
    TSTree *tree = ts_parser_parse_string(m_parser, nullptr, utf8.constData(), utf8.length());
    if (!tree) return errors;
    TSNode root = ts_tree_root_node(tree);
    uint32_t n = ts_node_named_child_count(root);
    for (uint32_t i = 0; i < n; ++i) {
        TSNode c = ts_node_named_child(root, i);
        const char *t = ts_node_type(c);
        if (strcmp(t, "ERROR") == 0 || strcmp(t, "MISSING") == 0) {
            SegmentInfo e; e.typeName = t;
            e.startByte = ts_node_start_byte(c); e.endByte = ts_node_end_byte(c);
            e.startLine = ts_node_start_point(c).row; e.endLine = ts_node_end_point(c).row;
            errors.append(e);
        }
    }
    ts_tree_delete(tree);
    return errors;
}

// =========================================================================
// DLL export
// =========================================================================

static LangComponent *g_comp = nullptr;

extern "C" __declspec(dllexport) ILanguageComponent* componentFactory()
{
    if (!g_comp) g_comp = new LangComponent();
    return g_comp;
}
