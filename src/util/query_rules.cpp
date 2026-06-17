#include "util/query_rules.h"
#include <QString>

// =========================================================================
// Language → Grammar DLL mapping
// =========================================================================

QString lexerToGrammarName(const QString &lexerName)
{
    if (lexerName == "c")            return "ts_c";
    if (lexerName == "cpp")          return "ts_cpp";
    if (lexerName == "python")       return "ts_python";
    if (lexerName == "javascript")   return "ts_javascript";
    if (lexerName == "bash")         return "ts_bash";
    if (lexerName == "java")         return "ts_java";
    if (lexerName == "csharp")       return "ts_c-sharp";  // hyphen matters for DLL name
    if (lexerName == "ruby")         return "ts_ruby";
    if (lexerName == "html")         return "ts_html";
    if (lexerName == "css")          return "ts_css";
    if (lexerName == "json")         return "ts_json";
    if (lexerName == "go")           return "ts_go";
    if (lexerName == "rust")         return "ts_rust";
    if (lexerName == "php")          return "ts_php";
    if (lexerName == "scala")        return "ts_scala";
    if (lexerName == "ocaml")        return "ts_ocaml";
    if (lexerName == "julia")        return "ts_julia";
    if (lexerName == "verilog")      return "ts_verilog";
    return QString();
}

// =========================================================================
// Grammar DLL → function name mapping
// =========================================================================

QString grammarToFuncName(const QString &grammarName)
{
    // grammarName is like "ts_cpp", "ts_c-sharp", etc.
    // Standard: "ts_XXX" → "tree_sitter_XXX"
    // Special: "ts_c-sharp" → "tree_sitter_c_sharp"
    if (grammarName == "ts_c-sharp") {
        return "tree_sitter_c_sharp";
    }
    return "tree_sitter_" + grammarName.mid(3);
}

// =========================================================================
// tags.scm queries for each language
// Based on official tree-sitter grammar repos
// =========================================================================

QString tagsQueryForLanguage(const QString &lexerName)
{
    // ---- C ----
    if (lexerName == "c") {
        return R"(
(struct_specifier (type_identifier) @name
  (field_declaration_list)) @definition.struct
(union_specifier (type_identifier) @name
  (field_declaration_list)) @definition.union
(function_definition
  declarator: (function_declarator
    declarator: (identifier) @name)) @definition.function
(type_definition (type_identifier) @name) @definition.type
(enum_specifier (type_identifier) @name
  (enumerator_list)) @definition.enum
(preproc_def (identifier) @name) @definition.macro
(preproc_function_def (identifier) @name) @definition.macro
)";
    }

    // ---- C++ ----
    if (lexerName == "cpp") {
        return R"(
(function_definition
  declarator: (function_declarator
    declarator: (identifier) @name)) @definition.function
(function_definition
  declarator: (function_declarator
    declarator: (qualified_identifier
      name: (identifier) @name))) @definition.function
(class_specifier (type_identifier) @name
  (field_declaration_list)) @definition.class
(struct_specifier (type_identifier) @name
  (field_declaration_list)) @definition.struct
(union_specifier (type_identifier) @name
  (field_declaration_list)) @definition.union
(enum_specifier (type_identifier) @name
  (enumerator_list)) @definition.enum
(type_definition (type_identifier) @name) @definition.type
(namespace_definition (namespace_identifier) @name) @definition.namespace
)";
    }

    // ---- Python ----
    if (lexerName == "python") {
        return R"(
(class_definition (identifier) @name) @definition.class
(function_definition (identifier) @name) @definition.function
(module (assignment left: (identifier) @name)) @definition.variable
)";
    }

    // ---- JavaScript ----
    if (lexerName == "javascript" || lexerName == "js") {
        return R"(
(method_definition (property_identifier) @name) @definition.method
(class_declaration name: (_) @name) @definition.class
(function_declaration (identifier) @name) @definition.function
(generator_function_declaration (identifier) @name) @definition.function
(program (lexical_declaration (variable_declarator (identifier) @name))) @definition.variable
(program (variable_declaration (variable_declarator (identifier) @name))) @definition.variable
)";
    }

    // ---- Bash ----
    if (lexerName == "bash") {
        return R"(
; Functions: function name() {...} or name() {...}
(function_definition name: (word) @name) @definition.function
)";
    }

    // ---- Java ----
    if (lexerName == "java") {
        return R"(
; Class/interface/enum definitions
(class_declaration (identifier) @name) @definition.class
(interface_declaration (identifier) @name) @definition.interface
(enum_declaration (identifier) @name) @definition.enum
; Method definitions
(method_declaration (identifier) @name) @definition.method
; Field definitions (public/private static final etc)
(field_declaration (variable_declarator (identifier) @name)) @definition.variable
; Constructor
(constructor_declaration (identifier) @name) @definition.method
)";
    }

    // ---- C# ----
    if (lexerName == "csharp") {
        return R"(
; Classes, structs, interfaces, enums
(class_declaration (identifier) @name) @definition.class
(struct_declaration (identifier) @name) @definition.struct
(interface_declaration (identifier) @name) @definition.interface
(enum_declaration (identifier) @name) @definition.enum
; Methods
(method_declaration (identifier) @name) @definition.method
; Constructors
(constructor_declaration (identifier) @name) @definition.method
; Properties
(property_declaration (identifier) @name) @definition.variable
; Namespace
(namespace_declaration (identifier) @name) @definition.namespace
)";
    }

    // ---- Ruby ----
    if (lexerName == "ruby") {
        return R"(
; Classes and modules
(class (constant) @name) @definition.class
(module (constant) @name) @definition.namespace
(singleton_class (constant) @name) @definition.class
; Methods
(method (identifier) @name) @definition.function
(singleton_method (identifier) @name) @definition.function
)";
    }

    // ---- HTML (uses fallback - AST query TBD) ----
    // Query disabled: tree-sitter-html node types don't match our patterns

    // ---- CSS (uses fallback - AST query TBD) ----
    // Query disabled: tree-sitter-css node types don't match our patterns

    // ---- JSON ----
    if (lexerName == "json") {
        return R"(
; Top-level object keys as symbols
(document (object (pair key: (string (string_content) @name)))) @definition.variable
)";
    }

    // ---- Verilog ----
    if (lexerName == "verilog") {
        return R"(
; Module declarations (name extraction uses fallback first-line heuristic)
(module_declaration) @definition.class
)";
    }
    if (lexerName == "json") {
        return R"(
; Top-level object keys as symbols
(document (object (pair key: (string (string_content) @name)))) @definition.variable
)";
    }

    return QString();
}
