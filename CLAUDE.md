# CLAUDE.md

此文件为 Claude Code (claude.ai/code) 提供在此代码库中工作的指导。

## 项目概述

这是一个 UNIX/Linux 文件权限模拟器,实现了基于访问权限表的访问控制。项目模拟了 UNIX 文件权限模型,包括用户、用户组以及读/写/执行权限。

## 架构设计

项目遵循课程规范要求的双层架构:

### 内核项目 (`KernProject/`)
- **用途**: 文件权限系统的核心实现
- **输出类型**: 静态库 (.lib)
- **严格限制**:
  - 必须主要使用 C 语言(允许使用 C++ 但不推荐用于核心逻辑)
  - **禁止使用标准库函数**(不能用 `cin`/`cout`、`printf`/`scanf`)
  - **禁止 UI 或 I/O 操作** - 纯数据结构和算法
  - **禁止窗口操作**
  - 强调紧凑、节省空间的数据结构设计
  - 启用了预编译头 (pch.h)

- **核心组件**:
  - [framework.h](KernProject/framework.h) - 预编译头设置
  - [pch.h](KernProject/pch.h) - 预编译头定义
  - [KernProject.cpp](KernProject/KernProject.cpp) - 核心内核函数

### 演示项目 (`DemoProject/`)
- **用途**: 内核功能的演示和测试
- **输出类型**: 控制台应用程序 (.exe)
- **职责**:
  - 链接 KernProject.lib
  - 提供 UI(基于控制台)
  - 按顺序调用内核函数以演示功能
  - 可以使用标准 I/O (`iostream`、`printf` 等)
  - 验证内核实现

## 构建系统

### 平台和工具
- **IDE**: Microsoft Visual Studio 2019 或更高版本(Platform Toolset v145)
- **解决方案**: [UNIX_File_Permission_Simulator.slnx](UNIX_File_Permission_Simulator.slnx)
- **C++ 标准**: C++20 (stdcpp20)
- **配置类型**:
  - Debug/Release × Win32/x64 = 4 种配置

### 构建项目

**构建整个解决方案:**
```bash
# 使用 Visual Studio 开发者命令提示符或 MSBuild
msbuild UNIX_File_Permission_Simulator.slnx /p:Configuration=Debug /p:Platform=x64
```

**构建单个项目:**
```bash
# 先构建内核库(生成 .lib)
msbuild KernProject/KernProject.vcxproj /p:Configuration=Debug /p:Platform=x64

# 再构建演示应用程序(链接 KernProject.lib)
msbuild DemoProject/DemoProject.vcxproj /p:Configuration=Debug /p:Platform=x64
```

**构建并运行:**
- 在 Visual Studio 中打开解决方案
- 按 F5(调试)或 Ctrl+F5(开始执行不调试)
- 或从菜单选择 生成 > 生成解决方案

### 构建依赖
1. 必须先构建 KernProject,再构建 DemoProject(依赖顺序)
2. DemoProject 会自动链接生成的 KernProject.lib
3. 构建输出:
   - KernProject: `x64/Debug/KernProject.lib`
   - DemoProject: `x64/Debug/DemoProject.exe`

## 系统要求

模拟器必须实现:

1. **文件系统模型**
   - 带有权限属性的文件和目录
   - 层次结构(可选但推荐)

2. **用户和用户组模型**
   - 带有唯一 ID 的用户账户
   - 带有成员关系的用户组
   - Root/超级用户概念(可选)

3. **权限系统**
   - UNIX 风格权限: 读(r)、写(w)、执行(x)
   - 权限类别: 所有者、用户组、其他用户
   - 权限表示(例如八进制表示法如 755)

4. **访问控制操作**
   - 读操作 - 检查并执行文件读取
   - 写操作 - 检查并执行文件写入
   - 执行操作 - 检查并执行文件/目录执行
   - 带拒绝场景的权限检查

## 开发指南

### 对于 KernProject (内核):
- 使用 C 风格数据结构(struct、数组)
- 避免使用 C++ 特性、STL 容器和标准库
- 实现纯算法逻辑而不涉及 I/O
- 专注于内存效率和紧凑设计
- 所有函数返回状态码或数据结构(不执行打印)

### 对于 DemoProject (演示):
- 可自由使用 C++ 特性和标准 I/O
- 创建涵盖以下内容的测试场景:
  - 正常访问(权限授予)
  - 拒绝访问(权限不足)
  - 边界情况(所有者 vs 用户组 vs 其他用户)
  - 不同用户和用户组
- 显示清晰的输出,展示权限检查和结果

### 文件组织
- 将内核数据结构和算法放在 `KernProject/` 中
- 将演示代码和 UI 放在 `DemoProject/` 中
- 将内核头文件保留在 `KernProject/` 中供内部使用
- Demo 项目包含内核头文件以调用函数

## 项目参考

完整课程要求和限制请参阅 [docs/doc.md](docs/doc.md)。
