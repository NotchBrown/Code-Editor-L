#include "util/tree_sitter_manager.h"
#include <QCoreApplication>
#include <QDir>
#include <QDebug>
#include <QSet>

TreeSitterManager* TreeSitterManager::m_instance = nullptr;

// ---------------------------------------------------------------------------
// Singleton
// ---------------------------------------------------------------------------

TreeSitterManager::TreeSitterManager()
{
}

TreeSitterManager::~TreeSitterManager()
{
    for (auto it = m_parsers.begin(); it != m_parsers.end(); ++it) {
        ts_parser_delete(it.value());
    }
    m_parsers.clear();
    // Languages are owned by the DLLs; just clear pointers
    m_languages.clear();
    for (auto it = m_libraries.begin(); it != m_libraries.end(); ++it) {
        delete it.value();
    }
    m_libraries.clear();

    for (auto it = m_queries.begin(); it != m_queries.end(); ++it) {
        ts_query_delete(it.value());
    }
    m_queries.clear();
}

TreeSitterManager* TreeSitterManager::instance()
{
    if (!m_instance) {
        m_instance = new TreeSitterManager();
    }
    return m_instance;
}

// ---------------------------------------------------------------------------
// Language / grammar mapping
// ---------------------------------------------------------------------------

// Map QScintilla lexer name → tree-sitter grammar DLL basename
static QString lexerToGrammarName(const QString &lexerName)
{
    if (lexerName == "c")            return "ts_c";
    if (lexerName == "cpp")          return "ts_cpp";
    if (lexerName == "python")       return "ts_python";
    if (lexerName == "javascript")   return "ts_javascript";
    if (lexerName == "java")         return "ts_java";
    if (lexerName == "csharp")       return "ts_csharp";
    if (lexerName == "go")           return "ts_go";
    if (lexerName == "rust")         return "ts_rust";
    if (lexerName == "php")          return "ts_php";
    if (lexerName == "ruby")         return "ts_ruby";
    if (lexerName == "bash")         return "ts_bash";
    if (lexerName == "html")         return "ts_html";
    if (lexerName == "css")          return "ts_css";
    if (lexerName == "json")         return "ts_json";
    if (lexerName == "scala")        return "ts_scala";
    if (lexerName == "ocaml")        return "ts_ocaml";
    if (lexerName == "julia")        return "ts_julia";
    return QString();
}

QString TreeSitterManager::grammarPath(const QString &lexerName)
{
    QString grammarName = lexerToGrammarName(lexerName);
    if (grammarName.isEmpty()) return QString();

    QStringList searchPaths;
    searchPaths << QCoreApplication::applicationDirPath() + "/grammars";
    searchPaths << QDir::currentPath() + "/grammars";
    searchPaths << "D:/Qt5Project/CodeEditorLite/lib/tree_sitter_mingw64/grammars";

    for (const QString &dir : searchPaths) {
        QString fullPath = dir + "/" + grammarName + ".dll";
        if (QFile::exists(fullPath)) {
            return fullPath;
        }
    }
    return QString();
}

bool TreeSitterManager::supportsLanguage(const QString &lexerName) const
{
    return !lexerToGrammarName(lexerName).isEmpty();
}

const TSLanguage* TreeSitterManager::loadLanguage(const QString &lexerName)
{
    if (m_languages.contains(lexerName)) {
        return m_languages[lexerName];
    }

    QString path = grammarPath(lexerName);
    if (path.isEmpty()) {
        qWarning() << "TreeSitter: no grammar DLL for" << lexerName;
        return nullptr;
    }

    QLibrary *lib = new QLibrary(path);
    if (!lib->load()) {
        qWarning() << "TreeSitter: failed to load" << path << lib->errorString();
        delete lib;
        return nullptr;
    }

    // Build the language function name from grammar DLL naming
    // Grammar DLLs export: tree_sitter_c(), tree_sitter_cpp(), tree_sitter_python(), etc.
    // The function name in the DLL matches the grammar name part after "ts_"
    QString grammarName = lexerToGrammarName(lexerName);  // e.g. "ts_cpp"
    QString funcName = "tree_sitter_" + grammarName.mid(3); // e.g. "tree_sitter_cpp"

    typedef const TSLanguage* (*LangFunc)();
    LangFunc func = reinterpret_cast<LangFunc>(lib->resolve(funcName.toLatin1().constData()));
    if (!func) {
        qWarning() << "TreeSitter: cannot resolve" << funcName << "in" << path;
        lib->unload();
        delete lib;
        return nullptr;
    }

    const TSLanguage *lang = func();
    m_languages[lexerName] = lang;
    m_libraries[lexerName] = lib;
    qDebug() << "[TreeSitter] loaded grammar for" << lexerName << "from" << path;
    return lang;
}

TSParser* TreeSitterManager::getParser(const QString &lexerName)
{
    if (m_parsers.contains(lexerName)) {
        qDebug() << "[TreeSitter] getParser: using cached parser for" << lexerName;
        return m_parsers[lexerName];
    }

    qDebug() << "[TreeSitter] getParser: loading new parser for" << lexerName;
    const TSLanguage *lang = loadLanguage(lexerName);
    if (!lang) {
        qWarning() << "[TreeSitter] getParser: loadLanguage failed for" << lexerName;
        return nullptr;
    }

    TSParser *parser = ts_parser_new();
    if (!parser) return nullptr;

    if (!ts_parser_set_language(parser, lang)) {
        qWarning() << "[TreeSitter] getParser: failed to set language for" << lexerName;
        ts_parser_delete(parser);
        return nullptr;
    }

    m_parsers[lexerName] = parser;
    qDebug() << "[TreeSitter] getParser: parser created OK for" << lexerName;
    return parser;
}

// ---------------------------------------------------------------------------
// Symbol extraction
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Query-based symbol extraction (VS Code / Eclipse style)
// ---------------------------------------------------------------------------

QString TreeSitterManager::tagsQueryForLanguage(const QString &lexerName)
{
    // Based on official tags.scm from each tree-sitter grammar repository
    if (lexerName == "c") {
        return R"(
(struct_specifier name: (type_identifier) @name) @definition.struct
(declaration type: (union_specifier name: (type_identifier) @name)) @definition.class
(function_definition
  declarator: (function_declarator
    declarator: (identifier) @name)) @definition.function
(type_definition declarator: (type_identifier) @name) @definition.type
(enum_specifier name: (type_identifier) @name) @definition.type
(preproc_def name: (identifier) @name) @definition.macro
(preproc_function_def name: (identifier) @name) @definition.macro
)";
    }
    if (lexerName == "cpp") {
        return R"(
(function_definition
  declarator: (function_declarator
    declarator: (identifier) @name)) @definition.function
(function_definition
  declarator: (function_declarator
    declarator: (qualified_identifier
      name: (identifier) @name))) @definition.function
(class_specifier name: (type_identifier) @name) @definition.class
(struct_specifier name: (type_identifier) @name) @definition.struct
(enum_specifier name: (type_identifier) @name) @definition.type
(type_definition declarator: (type_identifier) @name) @definition.type
)";
    }
    if (lexerName == "python") {
        return R"(
(class_definition name: (identifier) @name) @definition.class
(function_definition name: (identifier) @name) @definition.function
(assignment left: (identifier) @name) @definition.variable
)";
    }
    if (lexerName == "javascript" || lexerName == "js") {
        return R"(
(method_definition name: (property_identifier) @name) @definition.method
(class_declaration name: (_) @name) @definition.class
(function_declaration name: (identifier) @name) @definition.function
(function_expression name: (identifier) @name) @definition.function
(generator_function_declaration name: (identifier) @name) @definition.function
(variable_declaration (variable_declarator name: (identifier) @name)) @definition.variable
(lexical_declaration (variable_declarator name: (identifier) @name)) @definition.variable
)";
    }
    return QString();
}

SymbolInfo::Type TreeSitterManager::captureToSymbolType(const char *captureName)
{
    if (!captureName) return SymbolInfo::Unknown;
    QString n(captureName);
    if (n == "definition.class")   return SymbolInfo::Class;
    if (n == "definition.struct")  return SymbolInfo::Struct;
    if (n == "definition.function") return SymbolInfo::Function;
    if (n == "definition.method")  return SymbolInfo::Method;
    if (n == "definition.type")    return SymbolInfo::Typedef;
    if (n == "definition.namespace") return SymbolInfo::Namespace;
    if (n == "definition.variable") return SymbolInfo::Variable;
    if (n == "definition.constant") return SymbolInfo::Variable;
    if (n == "definition.macro")   return SymbolInfo::Macro;
    if (n == "reference.macro")    return SymbolInfo::Macro;
    if (n == "definition.module")  return SymbolInfo::Module;
    if (n == "definition.interface") return SymbolInfo::Interface;
    return SymbolInfo::Unknown;
}

// Extract a substring from UTF-8 bytes using byte offsets (tree-sitter's native unit).
static QString extractByteRange(const QByteArray &utf8Source, uint32_t startByte, uint32_t endByte)
{
    if (endByte <= startByte || startByte >= (uint32_t)utf8Source.size()) return QString();
    uint32_t len = qMin(endByte - startByte, (uint32_t)(utf8Source.size() - startByte));
    return QString::fromUtf8(utf8Source.constData() + startByte, len);
}

QString TreeSitterManager::extractSymbolName(const TSQueryMatch &match,
                                               const TSQuery *query,
                                               const QByteArray &utf8Source)
{
    for (uint32_t i = 0; i < match.capture_count; ++i) {
        const TSQueryCapture &cap = match.captures[i];
        uint32_t nameLen;
        const char *capName = ts_query_capture_name_for_id(query, cap.index, &nameLen);
        if (capName && strcmp(capName, "name") == 0) {
            uint32_t s = ts_node_start_byte(cap.node);
            uint32_t e = ts_node_end_byte(cap.node);
            if (e > s && s < (uint32_t)utf8Source.size()) {
                return extractByteRange(utf8Source, s, e);
            }
        }
    }
    return QString();
}

QString TreeSitterManager::symbolTypeName(SymbolInfo::Type t)
{
    switch (t) {
        case SymbolInfo::Function:   return "Functions";
        case SymbolInfo::Method:     return "Methods";
        case SymbolInfo::Class:      return "Classes";
        case SymbolInfo::Struct:     return "Structs";
        case SymbolInfo::Enum:       return "Enums";
        case SymbolInfo::Union:      return "Unions";
        case SymbolInfo::Namespace:  return "Namespaces";
        case SymbolInfo::Variable:   return "Variables";
        case SymbolInfo::Typedef:    return "Typedefs";
        case SymbolInfo::Macro:      return "Macros";
        case SymbolInfo::Interface:  return "Interfaces";
        case SymbolInfo::Module:     return "Modules";
        default:                     return "Other";
    }
}

void TreeSitterManager::extractSymbolsViaQuery(const QString &source,
                                                const QString &lexerName,
                                                QList<SymbolInfo> &symbols)
{
    QString queryStr = tagsQueryForLanguage(lexerName);
    if (queryStr.isEmpty()) return;

    qDebug() << "[TreeSitter] extractSymbolsViaQuery: lang=" << lexerName
             << "sourceLen=" << source.length()
             << "queryLen=" << queryStr.length();

    // Ensure language and parser are loaded FIRST (needed for query compilation)
    TSParser *parser = getParser(lexerName);
    if (!parser) {
        qWarning() << "[TreeSitter] no parser for" << lexerName;
        return;
    }

    const TSLanguage *lang = m_languages.value(lexerName);
    if (!lang) {
        qWarning() << "[TreeSitter] language not loaded for" << lexerName;
        return;
    }
    qDebug() << "[TreeSitter] language loaded OK, parser ready";

    // Cache compiled queries
    TSQuery *query = nullptr;
    if (m_queries.contains(lexerName)) {
        query = m_queries[lexerName];
    } else {
        uint32_t errorOffset;
        TSQueryError errorType;
        QByteArray queryBytes = queryStr.toUtf8();
        query = ts_query_new(lang,
                             queryBytes.constData(),
                             queryBytes.length(),
                             &errorOffset, &errorType);
        if (!query) {
            qWarning() << "[TreeSitter] FAILED to compile query for" << lexerName
                       << "errorType=" << errorType << "at offset=" << errorOffset;
            int start = qMax(0, (int)errorOffset - 20);
            int end = qMin(queryStr.length(), (int)errorOffset + 20);
            qWarning() << "[TreeSitter] query context:" << queryStr.mid(start, end - start);
            return;
        }
        qDebug() << "[TreeSitter] query compiled OK for" << lexerName;
        m_queries[lexerName] = query;
    }

    QByteArray utf8Source = source.toUtf8();
    qDebug() << "[TreeSitter] parsing" << utf8Source.length() << "bytes";
    TSTree *tree = ts_parser_parse_string(parser, nullptr,
                                           utf8Source.constData(),
                                           utf8Source.length());
    if (!tree) {
        qWarning() << "[TreeSitter] FAILED to parse" << lexerName;
        return;
    }

    TSNode root = ts_tree_root_node(tree);
    qDebug() << "[TreeSitter] parsed OK, root type:" << ts_node_type(root)
             << "childCount=" << ts_node_child_count(root);

    // --- AST node type dump (first 2 levels) ---
    {
        QSet<QString> seenTypes;
        QList<QPair<QString,int>> nodeStats;
        uint32_t n = ts_node_named_child_count(root);
        for (uint32_t i = 0; i < n && i < 60; ++i) {
            TSNode child = ts_node_named_child(root, i);
            if (ts_node_is_null(child)) continue;
            QString type1(ts_node_type(child));
            if (!seenTypes.contains(type1)) {
                seenTypes.insert(type1);
                nodeStats.append(qMakePair(type1, 1));
            }
            // Second level
            uint32_t n2 = ts_node_named_child_count(child);
            for (uint32_t j = 0; j < n2 && j < 8; ++j) {
                TSNode grand = ts_node_named_child(child, j);
                if (ts_node_is_null(grand)) continue;
                QString type2 = QString(ts_node_type(grand)) + " (in " + type1 + ")";
                if (!seenTypes.contains(type2)) {
                    seenTypes.insert(type2);
                    nodeStats.append(qMakePair(type2, 1));
                }
            }
        }
        qDebug() << "[TreeSitter] AST node types found:";
        for (auto &p : nodeStats) {
            qDebug() << "  " << p.first;
        }
    }

    TSQueryCursor *cursor = ts_query_cursor_new();
    ts_query_cursor_exec(cursor, query, root);
    qDebug() << "[TreeSitter] query cursor created, executing...";

    TSQueryMatch match;
    int matchCount = 0;
    while (ts_query_cursor_next_match(cursor, &match)) {
        matchCount++;
        if (matchCount <= 5) {
            qDebug() << "[TreeSitter]   match#" << matchCount << "pattern=" << match.pattern_index
                     << "captures=" << match.capture_count;
            for (uint32_t ci = 0; ci < match.capture_count; ++ci) {
                uint32_t clen;
                const char *cname = ts_query_capture_name_for_id(query, match.captures[ci].index, &clen);
                const char *ntype = ts_node_type(match.captures[ci].node);
                uint32_t sr = ts_node_start_point(match.captures[ci].node).row;
                qDebug() << "[TreeSitter]     capture" << ci << ": name=" << (cname ? cname : "(null)")
                         << "node=" << (ntype ? ntype : "(null)") << "line=" << sr;
            }
        }
        // Determine symbol type from capture names
        SymbolInfo::Type symType = SymbolInfo::Unknown;
        for (uint32_t i = 0; i < match.capture_count; ++i) {
            uint32_t nameLen;
            const char *capName = ts_query_capture_name_for_id(query, match.captures[i].index, &nameLen);
            SymbolInfo::Type t = captureToSymbolType(capName);
            if (t != SymbolInfo::Unknown) {
                symType = t;
                break;
            }
        }
        if (symType == SymbolInfo::Unknown) continue;

        SymbolInfo info;
        info.type = symType;
        // Use UTF-8 byte offsets to extract name properly
        info.name = extractSymbolName(match, query, utf8Source);

        // Use the first capture's node for position
        if (match.capture_count > 0) {
            TSNode node = match.captures[0].node;
            info.startByte = ts_node_start_byte(node);
            info.endByte   = ts_node_end_byte(node);
            info.line      = ts_node_start_point(node).row;
        }

        if (info.name.isEmpty()) {
            // Fallback: use first line of the matched node (via UTF-8 bytes)
            if (match.capture_count > 0) {
                TSNode node = match.captures[0].node;
                uint32_t s = ts_node_start_byte(node);
                uint32_t e = ts_node_end_byte(node);
                // Limit to first line
                for (uint32_t i = s; i < e && i < (uint32_t)utf8Source.size(); ++i) {
                    if (utf8Source[i] == '\n') { e = i; break; }
                }
                info.name = extractByteRange(utf8Source, s, e).trimmed().left(60);
            }
        }

        symbols.append(info);
    }

    qDebug() << "[TreeSitter] found" << matchCount << "query matches, extracted" << symbols.size() << "symbols for" << lexerName;
    for (int si = 0; si < qMin(symbols.size(), 10); ++si) {
        qDebug() << "[TreeSitter]   symbol:" << symbols[si].name << "type=" << symbols[si].type << "line=" << symbols[si].line;
    }

    ts_query_cursor_delete(cursor);
    ts_tree_delete(tree);
}

QList<SymbolInfo> TreeSitterManager::parseSymbols(const QString &source, const QString &lexerName)
{
    QList<SymbolInfo> symbols;
    if (source.isEmpty()) {
        qDebug() << "[TreeSitter] parseSymbols: source is empty for" << lexerName;
        return symbols;
    }

    qDebug() << "[TreeSitter] parseSymbols:" << lexerName << "sourceLen=" << source.length();
    extractSymbolsViaQuery(source, lexerName, symbols);
    qDebug() << "[TreeSitter] parseSymbols: returning" << symbols.size() << "symbols for" << lexerName;
    return symbols;
}

// ---------------------------------------------------------------------------
// Segment / context info
// ---------------------------------------------------------------------------

SegmentInfo TreeSitterManager::getSegmentAt(const QString &source, const QString &lexerName, int byteOffset)
{
    SegmentInfo info;
    if (source.isEmpty()) return info;

    qDebug() << "[TreeSitter] getSegmentAt: byteOffset=" << byteOffset;
    TSParser *parser = getParser(lexerName);
    if (!parser) return info;

    QByteArray utf8Source = source.toUtf8();
    TSTree *tree = ts_parser_parse_string(parser, nullptr, utf8Source.constData(), utf8Source.length());
    if (!tree) return info;

    TSNode root = ts_tree_root_node(tree);

    // Find the innermost named node at the given position
    TSNode node = ts_node_descendant_for_byte_range(root, (uint32_t)byteOffset, (uint32_t)byteOffset);
    if (ts_node_is_null(node)) {
        ts_tree_delete(tree);
        return info;
    }

    // Walk up to find a meaningful parent (skip anonymous/expression-level nodes)
    TSNode target = node;
    while (!ts_node_is_null(target)) {
        const char *type = ts_node_type(target);
        QString t(type);

        // Look for interesting structural nodes
        if (t.contains("function_definition") || t.contains("function_declaration") ||
            t.contains("if_statement") || t == "if" ||
            t.contains("while_statement") || t == "while" ||
            t.contains("for_statement") || t == "for" ||
            t.contains("class_specifier") || t.contains("class_definition") ||
            t.contains("struct_specifier") ||
            t.contains("switch_statement") || t == "switch" ||
            t.contains("try_statement") || t == "try" ||
            t.contains("catch") ||
            t.contains("else_clause") || t == "else" ||
            t.contains("do_statement") || t == "do" ||
            t.contains("case_statement") || t == "case" ||
            t.contains("template_declaration") ||
            t.contains("lambda") ||
            t.contains("compound_statement"))
        {
            info.typeName = t;
            info.startLine = ts_node_start_point(target).row;
            info.endLine   = ts_node_end_point(target).row;
            info.startByte = ts_node_start_byte(target);
            info.endByte   = ts_node_end_byte(target);

            // Extract summary from first line
            int s = info.startByte;
            int e = info.endByte;
            for (int i = s; i < e && i < source.length(); ++i) {
                if (source[i] == '\n') { e = i; break; }
            }
            info.summary = source.mid(s, e - s).trimmed();

            // Extract properties based on node type
            if (t.contains("if_statement") || t == "if" || t.contains("else_clause") || t == "else") {
                // Find condition
                TSNode cond = ts_node_child_by_field_name(target, "condition", strlen("condition"));
                if (!ts_node_is_null(cond)) {
                    int cs = ts_node_start_byte(cond);
                    int ce = ts_node_end_byte(cond);
                    QString condText = source.mid(cs, ce - cs).trimmed();
                    info.properties.append(qMakePair(QString("Condition"), condText));
                }
            } else if (t.contains("while_statement") || t == "while") {
                TSNode cond = ts_node_child_by_field_name(target, "condition", strlen("condition"));
                if (!ts_node_is_null(cond)) {
                    int cs = ts_node_start_byte(cond);
                    int ce = ts_node_end_byte(cond);
                    info.properties.append(qMakePair(QString("Condition"), source.mid(cs, ce - cs).trimmed()));
                }
            } else if (t.contains("for_statement") || t == "for") {
                // For C: for_statement has initializer, condition, update children
                uint32_t nc = ts_node_named_child_count(target);
                for (uint32_t i = 0; i < nc; ++i) {
                    TSNode child = ts_node_named_child(target, i);
                    const char *ct = ts_node_type(child);
                    int cs = ts_node_start_byte(child);
                    int ce = ts_node_end_byte(child);
                    info.properties.append(qMakePair(QString(ct), source.mid(cs, ce - cs).trimmed()));
                }
            } else if (t.contains("function") || t.contains("method")) {
                // Get function name and parameters
                TSNode declName = ts_node_child_by_field_name(target, "name", strlen("name"));
                if (!ts_node_is_null(declName)) {
                    int ns = ts_node_start_byte(declName);
                    int ne = ts_node_end_byte(declName);
                    info.properties.append(qMakePair(QString("Name"), source.mid(ns, ne - ns)));
                }
                // Find declarator for parameters
                uint32_t nc = ts_node_named_child_count(target);
                for (uint32_t i = 0; i < nc; ++i) {
                    TSNode child = ts_node_named_child(target, i);
                    const char *ct = ts_node_type(child);
                    if (strcmp(ct, "function_declarator") == 0 ||
                        strcmp(ct, "method_declarator") == 0) {
                        // Get parameters
                        uint32_t pc = ts_node_named_child_count(child);
                        QStringList params;
                        for (uint32_t j = 0; j < pc; ++j) {
                            TSNode param = ts_node_named_child(child, j);
                            int ps = ts_node_start_byte(param);
                            int pe = ts_node_end_byte(param);
                            params << source.mid(ps, pe - ps).trimmed();
                        }
                        if (!params.isEmpty()) {
                            info.properties.append(qMakePair(QString("Parameters"), params.join(", ")));
                        }
                        break;
                    }
                }
            } else if (t.contains("class") || t.contains("struct")) {
                TSNode nameNode = ts_node_child_by_field_name(target, "name", strlen("name"));
                if (!ts_node_is_null(nameNode)) {
                    int ns = ts_node_start_byte(nameNode);
                    int ne = ts_node_end_byte(nameNode);
                    info.properties.append(qMakePair(QString("Name"), source.mid(ns, ne - ns)));
                }
                // Check for base classes
                TSNode bases = ts_node_child_by_field_name(target, "bases", strlen("bases"));
                if (!ts_node_is_null(bases)) {
                    int bs = ts_node_start_byte(bases);
                    int be = ts_node_end_byte(bases);
                    info.properties.append(qMakePair(QString("Inherits"), source.mid(bs, be - bs)));
                }
            } else if (t.contains("switch")) {
                TSNode cond = ts_node_child_by_field_name(target, "condition", strlen("condition"));
                if (!ts_node_is_null(cond)) {
                    int cs = ts_node_start_byte(cond);
                    int ce = ts_node_end_byte(cond);
                    info.properties.append(qMakePair(QString("Expression"), source.mid(cs, ce - cs)));
                }
            } else if (t.contains("case")) {
                TSNode val = ts_node_child_by_field_name(target, "value", strlen("value"));
                if (!ts_node_is_null(val)) {
                    int vs = ts_node_start_byte(val);
                    int ve = ts_node_end_byte(val);
                    info.properties.append(qMakePair(QString("Value"), source.mid(vs, ve - vs)));
                }
            }

            break; // Found our context node
        }
        target = ts_node_parent(target);
    }

    ts_tree_delete(tree);
    return info;
}

// ---------------------------------------------------------------------------
// Error detection (static checking)
// ---------------------------------------------------------------------------

QList<SegmentInfo> TreeSitterManager::getErrorNodes(const QString &source, const QString &lexerName)
{
    QList<SegmentInfo> errors;
    if (source.isEmpty()) return errors;

    qDebug() << "[TreeSitter] getErrorNodes:" << lexerName << "sourceLen=" << source.length();
    TSParser *parser = getParser(lexerName);
    if (!parser) return errors;

    QByteArray utf8Source = source.toUtf8();
    TSTree *tree = ts_parser_parse_string(parser, nullptr, utf8Source.constData(), utf8Source.length());
    if (!tree) return errors;

    TSNode root = ts_tree_root_node(tree);

    // Recursively find ERROR and MISSING nodes
    std::function<void(TSNode)> findErrors = [&](TSNode node) {
        if (ts_node_is_error(node) || ts_node_is_missing(node)) {
            SegmentInfo err;
            err.typeName = ts_node_is_missing(node) ? "missing" : "error";
            err.startLine = ts_node_start_point(node).row;
            err.endLine   = ts_node_end_point(node).row;
            err.startByte = ts_node_start_byte(node);
            err.endByte   = ts_node_end_byte(node);
            int s = err.startByte;
            int e = qMin(err.startByte + 80, err.endByte);
            if (e > source.length()) e = source.length();
            err.summary = source.mid(s, e - s).trimmed();
            err.properties.append(qMakePair(QString("Expected"),
                QString(ts_node_string(node))));
            errors.append(err);
        }
        uint32_t n = ts_node_named_child_count(node);
        for (uint32_t i = 0; i < n; ++i) {
            findErrors(ts_node_named_child(node, i));
        }
    };

    findErrors(root);
    ts_tree_delete(tree);
    return errors;
}
