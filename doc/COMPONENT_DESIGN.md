# 语言组件架构设计

## 目标

将语言高级功能（语法分析、结构解析、静态检查等）从主编辑器中解耦，改为可插拔的组件系统。核心编辑器只提供 QScintilla 基础功能（高亮、折叠），高级功能通过组件按需加载。

## 架构概览

```
┌──────────────────────────────────────────────────────┐
│                    Core Editor                        │
│  (QScintilla: 高亮/折叠/编辑/编码/文件管理)            │
│                                                      │
│  ComponentManager ── 扫描/加载/管理组件               │
│       │                                              │
│       ├── ILanguageComponent (接口)                   │
│       │    ├── querySymbols(source, lang) → Symbol[]  │
│       │    ├── getSegment(source, lang, pos) → Info   │
│       │    ├── getDiagnostics(source, lang) → Error[] │
│       │    └── supportsLanguage(lang) → bool          │
│       │                                              │
│       ├── ts_analyzer (tree-sitter 内置组件)           │
│       └── (未来) verilog_analyzer.dll                 │
│                    python_language_server.dll          │
│                    ...                                │
└──────────────────────────────────────────────────────┘
```

## 核心接口

### ILanguageComponent （抽象基类）

所有语言组件必须实现的接口：

```cpp
class ILanguageComponent {
public:
    virtual ~ILanguageComponent() = default;
    
    // 组件元信息
    virtual QString componentName() const = 0;
    virtual QString componentVersion() const = 0;
    virtual QString apiVersion() const = 0;       // 兼容的 API 版本
    
    // 能力查询
    virtual bool supportsLanguage(const QString &lexerName) const = 0;
    virtual QStringList supportedLanguages() const = 0;
    virtual Capabilities capabilities() const = 0;  // 位掩码能力标志
    
    // 核心功能
    virtual QList<SymbolInfo> parseSymbols(const QString &source,
                                            const QString &lexerName) = 0;
    virtual SegmentInfo getSegmentAt(const QString &source,
                                      const QString &lexerName,
                                      int byteOffset) = 0;
    virtual QList<SegmentInfo> getErrorNodes(const QString &source,
                                              const QString &lexerName) = 0;
};

// 能力标志
enum Capability {
    CapNone          = 0,
    CapSymbolOutline = 1 << 0,   // 符号大纲（Navigator）
    CapSegmentInfo   = 1 << 1,   // 段信息（Segment）
    CapDiagnostics   = 1 << 2,   // 静态检查
    CapCompletion    = 1 << 3,   // 自动补全
    CapHover         = 1 << 4,   // 悬停提示
};
```

### ComponentManager （单例）

```cpp
class ComponentManager {
public:
    static ComponentManager* instance();
    
    // 扫描 components/ 目录加载所有有效的组件
    void scanComponents();
    
    // 获取支持某语言的最佳组件
    ILanguageComponent* componentForLanguage(const QString &lexerName);
    
    // 获取所有已加载组件
    QList<ILanguageComponent*> loadedComponents() const;
    
    // 检查某语言是否有某能力
    bool hasCapability(const QString &lexerName, Capability cap);
};
```

## 组件发现与加载

1. 启动时扫描 `components/` 目录下的 .dll 文件
2. 每个 DLL 必须导出 `componentFactory()` 函数返回 `ILanguageComponent*`
3. 使用 Qt's QPluginLoader 加载
4. 验证 `apiVersion()` 是否与当前编辑器兼容
5. 注册到 ComponentManager 供查询

```
程序目录/
  CodeEditorLite.exe
  components/
    ts_cpp_analyzer.dll    -- C++ tree-sitter 组件
    ts_python_analyzer.dll -- Python tree-sitter 组件
    ...
  grammars/
    ts_cpp.dll             -- tree-sitter grammar DLL（由组件加载）
    ...
```

## 安全性

| 措施 | 说明 |
|------|------|
| API 版本检查 | 组件必须声明兼容的 `apiVersion()`，不匹配时拒绝加载 |
| 签名验证 | 可选：对组件 DLL 进行数字签名验证 |
| 崩溃隔离 | 组件运行在主进程内。考虑未来用 QProcess 做进程级隔离 |
| 资源限制 | 组件不应执行无限循环或大量内存分配（由 Qt 插件框架约束） |
| 日志追踪 | 所有组件调用记录到 debug 日志，便于问题追踪 |

## 当前实现状态

### 已完成
- `inc/component/language_component.h` -- `ILanguageComponent` 接口 + `SymbolInfo`/`SegmentInfo`/`Capability` 定义
- `inc/component/component_manager.h` -- `ComponentManager` 单例头文件
- `src/component/component_manager.cpp` -- 实现：`scanComponents()`、`loadPlugin()`、版本检查
- `CodeEditorLite.pro` -- 添加了新的源文件、头文件路径
- 构建通过 (Release + Debug)

### 下一步：内置组件适配

- 将 `TreeSitterManager` 包装为 `TreeSitterComponent` 实现 `ILanguageComponent`
- `ComponentManager` 将内置组件作为默认注册
- Navigator 和 Segment 改为通过 `ComponentManager` 查询

### 第三步：提取为外部 DLL
- 将 tree-sitter 相关代码编译为 `ts_analyzer.dll`
- 从主项目删除 TreeSitterManager 的源代码
- 主项目只保留接口头文件和 ComponentManager

### 第四步：第三方组件
- 定义组件开发 SDK（接口头文件 + 构建模板）
- 第三方可实现自定义语言分析器
- 社区可贡献更多语言支持

## 当前依赖关系（重构前）

```
TreeSitterManager (单例，直接编译进主程序)
    ├── Navigator —— #include "util/tree_sitter_manager.h"
    ├── Segment   —— #include "util/tree_sitter_manager.h"
    └── grammar DLLs —— 运行时动态加载
```

## 重构后依赖关系

```
ComponentManager (编译进主程序)
    ├── ts_analyzer.dll (内置/外部组件)
    │   ├── TreeSitterManager (组件内部)
    │   └── grammar DLLs (由组件加载)
    ├── Navigator —— 通过 ComponentManager 获取组件
    └── Segment   —— 通过 ComponentManager 获取组件
```
