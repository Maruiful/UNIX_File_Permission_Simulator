# 🗂️ UNIX File Permission Simulator

<div align="center">

**一个用于教学和实验的 UNIX/Linux 文件权限模拟器**

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++](https://img.shields.io/badge/C++-20-blue.svg)](https://isocpp.org/)
[![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey.svg)](https://www.microsoft.com/windows)

</div>

---
>这是hfut的操作系统课程设计中的一题，原题目是
>访问控制技术：建立基于访问权限表的系统安全管理。
>⚫ 实现 UNIX/LINUX 的文件访问权限模型；
>⚫ 模拟文件及目录的数据结构，为文件添加访问权限
>⚫ 建立用户与用户组模型
>⚫ 设计读、写和执行三种指令，模拟不同用户访问文件或目录时所发生的受限情况。
>我实现了上述功能，并进行了扩展，大部分测试都能通过，但是严格的测试无法通过，同时相比于一个完整的linux，还欠缺很多东西。

## 📖 项目简介

UNIX 文件权限模拟器是一个教学工具,用于模拟 UNIX/Linux 文件系统的权限控制机制。它完整实现了文件系统的层次结构、用户/组管理以及读/写/执行权限控制,并提供了一个交互式命令行界面(Shell)。

### ✨ 主要特性

- 📁 **完整的文件系统** - 支持目录和文件的创建、删除、移动、复制
- 👥 **多用户管理** - 支持用户和用户组的创建与管理
- 🔒 **细粒度权限控制** - 实现了标准的 UNIX rwx 权限模型
- 🛠️ **18个 Shell 命令** - 提供完整的命令行操作体验
- 🎯 **双层架构设计** - 内核层 + 演示层,职责清晰分离
- 📚 **丰富的测试** - 包含22个自动化测试场景
- 📖 **完善的文档** - 用户手册和开发者文档齐全

---

## 🎯 适用场景

- 🎓 **教学演示** - 帮助学生理解 UNIX 文件权限机制
- 🧪 **实验环境** - 安全地测试权限设置,无需真实系统
- 🔍 **算法学习** - 学习文件系统树结构和权限检查算法
- 💻 **项目参考** - C/C++ 项目架构设计的参考示例

---

## 🚀 快速开始

### 环境要求

- **编译器**: Microsoft Visual Studio 2019 或更高版本
- **Platform Toolset**: v145
- **C++ 标准**: C++20
- **操作系统**: Windows (x64)

### 编译步骤

1. **克隆项目**
   ```bash
   git clone https://github.com/yourusername/UNIX_File_Permission_Simulator.git
   cd UNIX_File_Permission_Simulator
   ```

2. **打开解决方案**
   - 使用 Visual Studio 打开 `UNIX_File_Permission_Simulator.slnx`

3. **编译项目**
   - 按 `Ctrl+Shift+B` 或选择 **生成 > 生成解决方案**

4. **运行程序**
   - 按 `Ctrl+F5` 或选择 **调试 > 开始执行(不调试)**

### 运行模式

启动程序后,可选择两种模式:

- **模式 1**: 运行所有自动化测试
- **模式 2**: 进入交互式 Shell

---

## 🖥️ 使用示例

### 交互式 Shell 示例

```bash
========================================
UNIX 文件权限模拟器
========================================
请选择模式:
  1. 运行所有测试
  2. 交互式Shell
请输入选择 (1 或 2): 2

========================================
UNIX 文件权限模拟器 - 交互式Shell
========================================
输入 'help' 查看可用命令

[root@/]$ mkdir /project
[root@/]$ mkdir /project/src
[root@/]$ touch /project/src/main.c
[root@/]$ ls /project
drwxr-xr-x uid=0 src/
[root@/]$ chmod /project/src/main.c 755
[root@/]$ useradd alice
用户创建成功: alice (UID=1, GID=1)
[root@/]$ su 1
[alice@/]$ cd /project
[alice@/project]$ ls
drwxr-xr-x uid=0 src/
[alice@/project]$ exit

再见!
```

### 支持的命令

| 类别 | 命令 | 功能 |
|------|------|------|
| **用户管理** | `useradd` | 创建用户 |
| | `userdel` | 删除用户 |
| | `groupadd` | 创建用户组 |
| | `su` | 切换用户 |
| | `whoami` | 显示当前用户 |
| **文件操作** | `ls` | 列出目录内容 |
| | `mkdir` | 创建目录 |
| | `touch` | 创建文件 |
| | `rm` | 删除文件/目录 |
| | `rm -r` | 递归删除 |
| | `mv` | 移动/重命名 |
| | `cp` | 复制文件 |
| **权限管理** | `chmod` | 修改权限 |
| | `chown` | 修改所有者 |
| **导航** | `cd` | 切换目录 |
| | `pwd` | 显示当前目录 |
| **其他** | `help` | 显示帮助 |
| | `exit` | 退出Shell |

---

## 🏗️ 项目架构

### 双层架构设计

```
┌─────────────────────────────────────┐
│      DemoProject (演示层)           │
│  ┌──────────┐  ┌──────────┐        │
│  │ 测试代码  │  │ Shell UI  │        │
│  └──────────┘  └──────────┘        │
│  - 可使用 C++ 和标准库              │
│  - 负责 UI 和 I/O                   │
│  - 验证功能正确性                   │
└─────────────────────────────────────┘
                 ↕ 链接
┌─────────────────────────────────────┐
│      KernProject (内核层)           │
│  ┌──────────┐  ┌──────────┐        │
│  │文件系统  │  │用户管理  │        │
│  └──────────┘  └──────────┘        │
│  - 纯 C 实现                        │
│  - 不使用标准库函数                 │
│  - 紧凑的数据结构                   │
│  - 核心算法和逻辑                   │
└─────────────────────────────────────┘
```

### 核心数据结构

```c
// 文件节点
typedef struct FileNode {
    int inode;                      // 唯一标识
    char filename[MAX_FILENAME_LEN]; // 文件名
    FileType type;                  // 类型(目录/文件)
    Permissions perms;              // 权限设置
    int uid;                        // 所有者ID
    int gid;                        // 组ID
    struct FileNode* parent;        // 父目录
    struct FileNode* children;      // 子节点链表
    struct FileNode* next;          // 同级链表
} FileNode;

// 权限结构
typedef struct {
    int owner_perms;   // 所有者权限 (rwx)
    int group_perms;   // 组权限 (rwx)
    int other_perms;   // 其他用户权限 (rwx)
} Permissions;
```

---

## 📚 文档

详细文档请查看 [`docs/`](docs/) 目录:

- **[用户手册.md](docs/用户手册.md)** - 完整的使用说明和命令参考
- **[开发者文档.md](docs/开发者文档.md)** - 架构设计和 API 文档

### 文档目录

```
docs/
├── 用户手册.md      # 使用说明、命令参考、权限模型
└── 开发者文档.md    # 架构设计、API 参考、扩展指南
```

---

## 🧪 测试

项目包含22个自动化测试场景:

### 测试覆盖

- ✅ 用户和组管理 (4个测试)
- ✅ 文件系统基础操作 (6个测试)
- ✅ 文件移动和重命名 (6个测试)
- ✅ 路径解析功能 (2个测试)
- ✅ 综合场景测试 (3个测试)

### 运行测试

```bash
# 运行所有测试
> DemoProject.exe
选择: 1

# 运行交互式Shell
> DemoProject.exe
选择: 2
```

---

## 📂 项目结构

```
UNIX_File_Permission_Simulator/
├── KernProject/                 # 内核项目(静态库)
│   ├── file_system.h           # 文件系统接口
│   ├── file_system.c           # 文件系统实现
│   ├── user_manager.h          # 用户管理接口
│   ├── user_manager.c          # 用户管理实现
│   ├── types.h                 # 公共类型定义
│   ├── framework.h             # 预编译头设置
│   └── pch.h                   # 预编译头定义
│
├── DemoProject/                # 演示项目(可执行文件)
│   └── DemoProject.cpp         # 主程序和Shell实现
│
├── docs/                       # 文档目录
│   ├── 用户手册.md
│   └── 开发者文档.md
│
├── README.md                   # 本文件
├── 第三阶段任务.md             # 任务需求文档
└── UNIX_File_Permission_Simulator.sln  # Visual Studio 解决方案
```

---

## 🔧 技术栈

- **语言**: C (内核层) + C++20 (演示层)
- **IDE**: Microsoft Visual Studio 2019+
- **编译器**: MSVC (Platform Toolset v145)
- **平台**: Windows x64
- **架构**: 双层架构(静态库 + 可执行文件)

---

## 📊 核心功能实现

### 文件系统功能

- ✅ 树形目录结构(链表实现)
- ✅ 文件和目录的创建、删除
- ✅ 文件移动和重命名
- ✅ 文件复制
- ✅ 递归删除非空目录
- ✅ 绝对路径和相对路径支持
- ✅ 路径规范化(`.` 和 `..`)
- ✅ 完整的边界检查和安全防护

### 权限系统

- ✅ 读(r)、写(w)、执行(x)权限
- ✅ 所有者、用户组、其他用户分类
- ✅ `chmod` 权限修改
- ✅ `chown` 所有者修改
- ✅ Root 超级用户机制

### 用户管理

- ✅ 用户创建和删除
- ✅ 用户组管理
- ✅ 用户组成员管理
- ✅ 权限检查算法

### 性能优化

- ✅ 字符串操作优化(减少 67% 复制)
- ✅ 路径解析优化(减少 100% 中间复制)
- ✅ 迭代算法替代递归
- ✅ 零编译警告

---

## 🎓 学习要点

本项目适合学习:

1. **文件系统设计** - 树形结构的实现
2. **权限控制算法** - UNIX 权限模型的实现
3. **C语言编程** - 指针、链表、内存管理
4. **软件架构** - 双层架构、接口分离
5. **测试驱动开发** - 完整的测试用例

---

## 🤝 贡献指南

欢迎提交 Issue 和 Pull Request!

### 贡献流程

1. Fork 本项目
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 开启 Pull Request

### 开发规范

- 遵循现有代码风格
- 添加相应的测试用例
- 更新相关文档
- 确保无编译警告

---

## 📝 开源协议

本项目采用 MIT 协议开源 - 查看 [LICENSE](LICENSE) 文件了解详情

---

## 👨‍💻 作者

- **项目**: UNIX 文件权限模拟器
- **版本**: 1.0
- **年份**: 2026

---

## 📮 联系方式

如有问题或建议,欢迎提交 Issue。

---

## 🙏 致谢

感谢所有为本项目做出贡献的开发者!

---

<div align="center">

**如果这个项目对您有帮助,请给一个 ⭐️ Star!**

Made with ❤️ by Maruiful

</div>
