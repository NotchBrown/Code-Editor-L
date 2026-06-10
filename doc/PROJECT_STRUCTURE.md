# CodeEditorLite - 项目实现结构文档

## 1. 项目概述

CodeEditorLite 是一款基于 Qt5 和 QScintilla 开发的轻量级代码编辑器，支持多语言语法高亮、多标签页管理、IPC 通信等功能。

### 技术栈
- **框架**: Qt 5.14.2 (MinGW 7.3.0)
- **编辑器引擎**: QScintilla 2.x
- **构建工具**: qmake + mingw32-make
- **脚本语言**: PowerShell 5.x

---

## 2. 目录结构

```
CodeEditorLite/
├── inc/                    # 头文件目录
│   ├── widget/
│   │   ├── main_window/    # 主窗口相关
│   │   └── editor/         # 编辑器相关
│   ├── ipc/                # IPC 通信模块
│   └── project/            # 项目管理模块
├── src/                    # 源文件目录
│   ├── main.cpp            # 应用入口
│   ├── widget/
│   │   ├── main_window/    # 主窗口实现
│   │   └── editor/         # 编辑器实现
│   ├── ipc/                # IPC 实现
│   └── project/            # 项目管理实现
├── lib/                    # 第三方库
│   └── QScintilla/         # QScintilla 子模块
├── resources/              # 资源文件
├── script/                 # 构建脚本
├── doc/                    # 文档
├── dist/                   # 构建输出
└── CodeEditorLite.pro      # qmake 项目文件
```

---

## 3. 核心模块说明

### 3.1 CodeEditor (代码编辑器)

**文件位置**:
- `inc/widget/editor/code_editor.h`
- `src/widget/editor/code_editor.cpp`

**功能特性**:
| 功能 | 说明 |
|------|------|
| 语法高亮 | 支持 C++、Python、JavaScript、HTML、JSON、SQL、Lua、Bash |
| 行号显示 | 左侧边距显示行号 |
| 代码折叠 | BoxedTree 风格折叠 |
| 括号匹配 | Sloppy 模式括号高亮 |
| 自动补全 | 基于 QScintilla 的自动补全机制 |
| 撤销/重做 | 通过 Scintilla API 设置撤销深度 |
| 查找/替换 | 支持大小写敏感和整词匹配 |

**类结构**:
```
CodeEditor : QsciScintilla
├── m_filePath            // 当前文件路径
├── m_extensionToLexer    // 扩展名到语法分析器映射
├── m_currentLexer        // 当前语法分析器指针
├── setupEditor()         // 初始化编辑器
├── setupLexer()          // 初始化语法分析器
├── setLexerByExtension() // 根据扩展名切换语法
├── setUndoDepth()        // 设置撤销深度
├── findText()            // 查找文本
└── replaceText()         // 替换文本
```

### 3.2 MainWindow (主窗口)

**文件位置**:
- `inc/widget/main_window/main_window.h`
- `src/widget/main_window/main_window.cpp`

**功能特性**:
| 功能 | 说明 |
|------|------|
| 多标签页 | QTabWidget 管理多个编辑器 |
| 菜单系统 | 文件、编辑、视图、导航菜单 |
| 工具栏 | 常用操作快捷按钮 |
| 状态栏 | 显示光标位置、文件状态 |
| IPC 集成 | 支持外部程序通信 |

**类结构**:
```
MainWindow : QMainWindow
├── m_tabWidget           // 标签页容器
├── m_fileToTabIndex      // 文件路径到标签索引映射
├── m_ipcServer           // IPC 服务器实例
├── m_projectManager      // 项目管理器实例
├── setupMenuBar()        // 初始化菜单栏
├── setupToolBar()        // 初始化工具栏
├── setupStatusBar()      // 初始化状态栏
├── createNewEditor()     // 创建新编辑器
├── currentEditor()       // 获取当前编辑器
└── initIpcServer()       // 初始化 IPC 服务器
```

### 3.3 IpcServer (IPC 服务器)

**文件位置**:
- `inc/ipc/ipc_server.h`
- `src/ipc/ipc_server.cpp`

**通信协议**:
基于 JSON 的简单协议，每行一条消息：
```json
{
    "id": 1,
    "method": "openFile",
    "params": {
        "filePath": "/path/to/file.cpp"
    }
}
```

**支持的命令**:
| 方法 | 参数 | 说明 |
|------|------|------|
| `ping` | 无 | 心跳检测 |
| `openFile` | `filePath` | 打开文件 |
| `closeFile` | `filePath` | 关闭文件 |
| `newFile` | 无 | 新建文件 |
| `saveFile` | `filePath`, `content` | 保存文件 |
| `getFileContent` | `filePath` | 获取文件内容 |
| `exit` | 无 | 退出程序 |

### 3.4 ProjectManager (项目管理器)

**文件位置**:
- `inc/project/project_manager.h`
- `src/project/project_manager.cpp`

**项目文件格式** (.celproj):
```ini
[Project]
Name=MyProject

[Files]
D:\Projects\src\main.cpp
D:\Projects\src\utils.h
```

---

## 4. 构建脚本

### 4.1 build_mingw64.ps1

**参数说明**:
| 参数 | 类型 | 说明 |
|------|------|------|
| `-Debug` | Switch | Debug 模式构建 |
| `-AddTimestamp` | Switch | 输出目录添加时间戳 |

**构建流程**:
1. 初始化 Qt 和 MinGW 环境变量
2. 创建构建目录 (`dist/build_release` 或 `dist/build_debug`)
3. 运行 qmake 生成 Makefile
4. 执行 mingw32-make 编译
5. 使用 windeployqt 复制依赖
6. 复制 QScintilla DLL (第三方库，windeployqt 无法检测)
7. 复制 Qt5Network.dll (有时 windeployqt 会遗漏)
8. 清理中间产物

---

## 5. 命令行参数

| 参数 | 说明 |
|------|------|
| `-mp <port>` | 启动 IPC 服务器，指定端口 |
| `-f <file>` | 启动时打开指定文件 |
| `-debug` | 启用调试模式 |
| `-h` / `--help` | 显示帮助信息 |
| `-v` / `--version` | 显示版本信息 |

---

## 6. 扩展机制

### 6.1 自定义语法支持

在 `CodeEditor::setupExtensionMap()` 中添加新的扩展名映射：
```cpp
m_extensionToLexer[".rs"] = "rust";  // 需要添加 QsciLexerRust
```

### 6.2 自定义快捷键

在 `MainWindow::setupActions()` 中添加新的菜单项和快捷键。

### 6.3 IPC 命令扩展

在 `IpcServer::processMessage()` 中添加新的命令处理逻辑。

---

## 7. 开发注意事项

### 7.1 Qt5.14.2 兼容性

- `QKeySequence::GoToLine` 在 Qt 5.14.2 中不存在，需使用字符串形式
- `QsciScintilla` 的枚举值需使用完整命名空间限定
- Scintilla 常量 (SCI_*) 不是 QsciScintilla 的成员，需直接使用数值

### 7.2 QScintilla 集成

- QScintilla DLL 需要手动复制（windeployqt 无法检测）
- 需要在 .pro 文件中正确配置 QSCINTILLA_PATH

### 7.3 编码规范

- 使用 UTF-8 编码
- 类名采用 PascalCase
- 函数名和变量名采用 camelCase
- 常量采用 UPPER_CASE

---

## 8. 版本历史

| 版本 | 日期 | 变更说明 |
|------|------|----------|
| 1.0.0 | 2026-06-10 | 初始版本，支持基础编辑功能 |

---

## 9. 许可证

MIT License

---

*文档版本: 1.0*  
*生成日期: 2026-06-10*