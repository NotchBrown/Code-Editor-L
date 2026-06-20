# CodeEditorLite

轻量级代码编辑器。

---

## 概述

CodeEditorLite 是一个轻量级代码编辑器，专注于提供快速响应的编辑体验和深度的代码结构分析能力。支持通过 addon 组件扩展语言高级功能。

## 语言支持性

| 语言 | 高亮 | 静态补全 | 折叠 | 结构解析(Navigator) | 段属性面板(Segment) | 静态实时检查 | 自动补全 | 辅助例化器 |
|------|------|---------|------|-------------------|-------------------|------------|---------|----------|
| C | 是 | QScintilla | 是 | tree-sitter (14类) | tree-sitter | 规划中 | 规划中 | - |
| C++ | 是 | QScintilla | 是 | tree-sitter (17类) | tree-sitter | 规划中 | 规划中 | - |
| Python | 是 | QScintilla | 是 | tree-sitter (18类) | tree-sitter | 规划中 | 规划中 | - |
| JavaScript | 是 | QScintilla | 是 | tree-sitter (19类) | tree-sitter | 规划中 | 规划中 | - |
| Bash | 是 | - | 是 | tree-sitter | - | - | - | - |
| Java | 是 | - | 是 | tree-sitter | - | - | - | - |
| C# | 是 | - | 是 | tree-sitter | - | - | - | - |
| Ruby | 是 | - | 是 | tree-sitter | - | - | - | - |
| JSON | 是 | - | 是 | tree-sitter | - | - | - | - |
| Verilog | 是 | - | 是 | tree-sitter (module) / 启发式 | - | - | - | 规划中 |
| HTML | 是 | - | 是 | 启发式 | - | - | - | - |
| CSS | 是 | - | 是 | 启发式 | - | - | - | - |
| XML | 是 | - | 是 | 启发式 | - | - | - | - |
| SQL | 是 | - | 是 | 启发式 | - | - | - | - |
| Lua | 是 | - | 是 | 启发式 | - | - | - | - |
| Fortran | 是 | - | 是 | 启发式 | - | - | - | - |
| Makefile | 是 | - | 是 | 启发式 | - | - | - | - |
| CMake | 是 | - | 是 | 启发式 | - | - | - | - |
| YAML | 是 | - | 是 | 启发式 | - | - | - | - |
| Markdown | 是 | - | 是 | - | - | - | - | - |
| TCL | 是 | - | 是 | 启发式 | - | - | - | - |
| Batch | 是 | - | 是 | 启发式 | - | - | - | - |
| Pascal | 是 | - | 是 | 启发式 | - | - | - | - |
| VHDL | 是 | - | 是 | 启发式 | - | - | - | - |
| CoffeeScript | 是 | - | 是 | 启发式 | - | - | - | - |
| 其他(QScintilla) | 是 | - | 是 | 启发式 | - | - | - | - |

说明：
- "tree-sitter" 表示使用增量式 AST 解析引擎精确提取结构信息
- "启发式" 表示使用基于正则表达式的语言特定规则提取结构
- 符号数表示 Navigator 面板可识别的 symbol 类别数量
- "规划中" 的功能将在后续版本中实现

---

## 计划

已完成：
1. 基本编辑器功能（打开/保存/编码/只读）
2. 多标签页管理
3. 语法高亮（41 种 QScintilla lexer）
4. 代码折叠
5. 查找替换面板
6. 书签管理（添加/删除/跳转）
7. 断点管理（添加/删除/清除）
8. 快捷键绑定与管理
9. 编码切换（UTF-8/GBK/GB2312/Big5 等）
10. 打印向导
11. 最近文件管理
12. 导航面板 Navigator（tree-sitter + 启发式结构解析）
13. 段属性面板 Segment（光标上下文信息）
14. tree-sitter 集成（12 种 grammar DLL）
15. 命令行打开文件支持

计划中：
1. 组件化重构 -- 将 tree-sitter 等语言分析功能抽取为独立 addon 组件
2. Segment 面板增强 -- 显示更丰富的上下文信息（函数参数、变量类型、作用域链）
3. IPC 通信 -- 实现 JSON over TCP 的请求/响应 IPC 协议，支持外部进程接入
4. 静态实时检查 -- 基于 tree-sitter 查询的代码问题检测（未使用变量、类型不匹配等）
5. 自动补全 -- 基于 tree-sitter AST 的符号感知补全
6. 辅助例化器 -- Verilog 模块例化代码生成（Alt+Tab）
7. 更多 tree-sitter grammar 构建（Go/Rust/PHP/Scala/TypeScript 等）
8. (伪)项目管理器增强 -- 经由Socket Message控制的文件树浏览、工程构建

---

## 构建

前置条件：Qt 5.14.2 + MinGW 7.3.0

```powershell
# Debug 构建
.\script\build_mingw64.ps1 -Debug

# Release 构建
.\script\build_mingw64.ps1
```

构建产物位于 `dist/build_debug/bin/` 或 `dist/build_release/bin/`。

---

## 测试

```powershell
# 打开文件（支持命令行参数）
CodeEditorLite.exe path/to/file.c

# 查看调试日志
# 日志位于 %APPDATA%\WaveIn\Code Editor\log\debug.log
```

测试文件位于 `test/test_file/`，按语言分类。

详细测试流程见 `.github/skills/tree-sitter-test/SKILL.md`。

---

## 目录结构

```
CodeEditorLite/
  inc/          -- 头文件
  src/          -- 源文件
    function/   -- 菜单/工具栏动作
    widget/     -- UI 组件
    ipc/        -- IPC 通信
    util/       -- 工具类
  lib/          -- 第三方库
    QScintilla/          -- QScintilla 源码
    qscintilla_mingw64/  -- 预编译库
    tree_sitter_mingw64/ -- tree-sitter 库 + grammars
    grammar_repos/       -- grammar 源码
  addons/       -- 语言分析组件（可选加载）
  doc/          -- 文档
  script/       -- 构建脚本
  test/         -- 测试文件
```

详细架构见 `doc/PROJECT_STRUCTURE.md`。

---

## License

MIT
