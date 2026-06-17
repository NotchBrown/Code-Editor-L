# Tree-Sitter Grammar 集成状态

> 三阶段：**编译** (DLL 构建) → **适配** (代码映射 + Query 规则) → **测试** (符号提取验证)

---

## 第一阶段：已编译 ✓

所有语言都已构建 grammar DLL 放入 `grammars/` 目录。

| # | 语言 | DLL | 源码仓库 |
|---|------|-----|---------|
| 1 | C | `ts_c.dll` | [tree-sitter-c](https://github.com/tree-sitter/tree-sitter-c) |
| 2 | C++ | `ts_cpp.dll` | [tree-sitter-cpp](https://github.com/tree-sitter/tree-sitter-cpp) |
| 3 | Python | `ts_python.dll` | [tree-sitter-python](https://github.com/tree-sitter/tree-sitter-python) |
| 4 | JavaScript | `ts_javascript.dll` | [tree-sitter-javascript](https://github.com/tree-sitter/tree-sitter-javascript) |
| 5 | Bash | `ts_bash.dll` | [tree-sitter-bash](https://github.com/tree-sitter/tree-sitter-bash) |
| 6 | Java | `ts_java.dll` | [tree-sitter-java](https://github.com/tree-sitter/tree-sitter-java) |
| 7 | C# | `ts_c-sharp.dll` | [tree-sitter-c-sharp](https://github.com/tree-sitter/tree-sitter-c-sharp) |
| 8 | Ruby | `ts_ruby.dll` | [tree-sitter-ruby](https://github.com/tree-sitter/tree-sitter-ruby) |
| 9 | HTML | `ts_html.dll` | [tree-sitter-html](https://github.com/tree-sitter/tree-sitter-html) |
| 10 | CSS | `ts_css.dll` | [tree-sitter-css](https://github.com/tree-sitter/tree-sitter-css) |
| 11 | JSON | `ts_json.dll` | [tree-sitter-json](https://github.com/tree-sitter/tree-sitter-json) |
| 12 | Verilog | `ts_verilog.dll` | [tree-sitter-verilog](https://github.com/tree-sitter/tree-sitter-verilog) |

**未构建**（无官方 tree-sitter grammar 或无 QScintilla 支持）：
XML, SQL, Lua, Perl, YAML, Markdown, TeX, Diff, D, Fortran, MATLAB, Makefile, CMake, TCL, Batch, POV, PostScript, Spice, IDL, Pascal, IntelHex, VHDL, AVS, EDIFACT, Properties, PO, CoffeeScript, Octave

---

## 第二阶段：适配

| # | 语言 | 代码映射 | Query 规则 | 状态 |
|---|------|---------|-----------|------|
| 1 | C | `inc/util/query_rules.cpp` | ✅ `definition.struct/union/function/type/enum/macro` | ✅ |
| 2 | C++ | `inc/util/query_rules.cpp` | ✅ `definition.function/class/struct/union/enum/type/namespace` | ✅ |
| 3 | Python | `inc/util/query_rules.cpp` | ✅ `definition.class/function/variable` | ✅ |
| 4 | JavaScript | `inc/util/query_rules.cpp` | ✅ `definition.method/class/function/variable` | ✅ |
| 5 | Bash | `inc/util/query_rules.cpp` | ✅ `definition.function` | ✅ |
| 6 | Java | `inc/util/query_rules.cpp` | ✅ `definition.class/interface/enum/method/variable` | ✅ |
| 7 | C# | `inc/util/query_rules.cpp` | ✅ `definition.class/struct/interface/enum/method/variable/namespace` | ✅ |
| 8 | Ruby | `inc/util/query_rules.cpp` | ✅ `definition.class/namespace/function` | ✅ |
| 9 | HTML | `inc/util/query_rules.cpp` | ❌ 节点名未知，使用 fallback | ⏳ |
| 10 | CSS | `inc/util/query_rules.cpp` | ❌ 节点名未知，使用 fallback | ⏳ |
| 11 | JSON | `inc/util/query_rules.cpp` | ✅ `definition.variable` | ✅ |
| 12 | Verilog | `inc/util/query_rules.cpp` | ❌ 节点名未知，使用 fallback | ⏳ |

### Query 规则文件位置
所有 query 规则集中在：`src/util/query_rules.cpp` → `tagsQueryForLanguage()`

### 特殊处理
- **C#**: DLL 名含连字符 `ts_c-sharp.dll`，函数名用下划线 `tree_sitter_c_sharp()`
- **`supportsLanguage()`**: 同时检查 query 规则存在 + DLL 存在，避免空 query 误判

---

## 第三阶段：测试

测试方法：`.\CodeEditorLite.exe <test_file>` → 读 `debug.log` → 分析 `[TreeSitter]` 输出

| # | 语言 | 测试文件 | 符号数 | 提取类型 | 状态 |
|---|------|---------|--------|---------|------|
| 1 | C | `test/test_file/c/test.c` | **14** | Struct, Union, Enum, Typedef, Function | ✅ |
| 2 | C++ | `test/test_file/cpp/test.cpp` | **17** | Namespace, Class, Struct, Enum, Typedef, Function (含模板+成员函数) | ✅ |
| 3 | Python | `test/test_file/python/test.py` | **18** | Class, Function, Module-level Variable | ✅ |
| 4 | JavaScript | `test/test_file/javascript/test.js` | **19** | Method, Class, Function, Generator, Variable | ✅ |
| 5 | Bash | `test/test_file/bash/test.sh` | **6** | Function | ✅ |
| 6 | Java | `test/test_file/java/Test.java` | **30** | Interface, Enum, Class, Method, Constructor, Variable | ✅ |
| 7 | C# | `test/test_file/csharp/Test.cs` | **26** | Namespace, Interface, Enum, Struct, Method, Property, Constructor | ✅ |
| 8 | Ruby | `test/test_file/ruby/test.rb` | **21** | Module, Class, Method | ✅ |
| 9 | HTML | `test/test_file/html/test.html` | 0 (fallback) | - | ⏳ |
| 10 | CSS | `test/test_file/css/test.css` | 16 (fallback) | 折行/缩进启发式 | ⏳ |
| 11 | JSON | `test/test_file/json/test.json` | **6** | Top-level keys | ✅ |
| 12 | Verilog | `test/test_file/verilog/test.v` | 0 (fallback) | - | ⏳ |

### 已知问题
1. **HTML/CSS/Verilog Query** - 节点类型名与 tree-sitter 语法版本不匹配，需根据 AST dump 调整
2. **Fortran/Makefile** - 无官方 tree-sitter grammar，一直使用 fallback
3. **Go/Rust/PHP/Scala 等** - QScintilla 无对应 lexer，回退到 "cpp"，C++ parser 无法正确解析
4. **文件内含中文注释** - UTF-8 字节偏移 ≠ UTF-16 字符偏移，已用 `extractByteRange()` 修复
