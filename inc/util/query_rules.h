#ifndef QUERY_RULES_H
#define QUERY_RULES_H

#include <QString>

// Map QScintilla lexer name → tree-sitter grammar DLL basename (e.g. "cpp" → "ts_cpp")
// Return empty string if no grammar available
QString lexerToGrammarName(const QString &lexerName);

// Map grammar DLL basename → tree-sitter language function name (e.g. "ts_cpp" → "tree_sitter_cpp")
// Handles special cases like "ts_c-sharp" → "tree_sitter_c_sharp"
QString grammarToFuncName(const QString &grammarName);

// Get the tags.scm query string for a given lexer name
QString tagsQueryForLanguage(const QString &lexerName);

#endif // QUERY_RULES_H
