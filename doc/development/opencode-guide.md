# 使用 OpenCode 开发指南 / OpenCode Development Guide

> **OpenCode** 是一款开源 AI 编程代理，运行在终端中，能够理解代码库、编辑文件、执行命令，大幅提升开发效率。
>
> **OpenCode** is an open-source AI coding agent that runs in the terminal. It understands your codebase, edits files, runs commands, and dramatically improves development efficiency.

---

## 目录 / Table of Contents

1. [安装 / Install](#安装--install)
2. [配置 Provider / Configure Provider](#配置-provider--configure-provider)
3. [初始化项目 / Initialize Project](#初始化项目--initialize-project)
4. [基本用法 / Basic Usage](#基本用法--basic-usage)
5. [开发工作流 / Development Workflow](#开发工作流--development-workflow)
6. [为 Nezha Privacy 项目配置 / Configure for Nezha Privacy](#为-nezha-privacy-项目配置--configure-for-nezha-privacy)
7. [常用命令速查 / Command Cheat Sheet](#常用命令速查--command-cheat-sheet)
8. [最佳实践 / Best Practices](#最佳实践--best-practices)

---

## 安装 / Install

### 一键安装 / Quick Install

```bash
curl -fsSL https://opencode.ai/install | bash
```

### 其他方式 / Alternative

```bash
# npm
npm install -g opencode-ai

# Homebrew (macOS / Linux)
brew install anomalyco/tap/opencode
```

安装完成后验证 / Verify installation:

```bash
opencode --version
```

---

## 配置 Provider / Configure Provider

OpenCode 需要 LLM Provider 的 API Key 才能工作。推荐以下两种方式：

OpenCode requires an LLM provider API key to work. Two recommended approaches:

### 方式一：使用 OpenCode Zen（推荐新手）/ Option 1: OpenCode Zen (Recommended for beginners)

在 OpenCode TUI 中运行 / Run in OpenCode TUI:

```
/connect
```

选择 `opencode`，前往 [opencode.ai/auth](https://opencode.ai/auth) 获取 API Key。

Select `opencode`, go to [opencode.ai/auth](https://opencode.ai/auth) to get your API key.

### 方式二：使用 Anthropic / OpenAI 等 Provider / Option 2: Use Anthropic / OpenAI etc.

设置环境变量 / Set environment variable:

```bash
# Anthropic Claude（推荐 / Recommended）
export ANTHROPIC_API_KEY="sk-ant-..."

# 或 OpenAI / Or OpenAI
export OPENAI_API_KEY="sk-..."
```

建议将上述行添加到 `~/.bashrc` 或 `~/.zshrc` 中。

Add the above to your `~/.bashrc` or `~/.zshrc` for persistence.

---

## 初始化项目 / Initialize Project

进入项目目录并启动 OpenCode / Navigate to project and start OpenCode:

```bash
cd /path/to/nezha-privacy/PPPU
opencode
```

首次使用时，运行初始化命令 / On first use, run the init command:

```
/init
```

这会扫描项目结构，生成 `AGENTS.md` 文件。该文件帮助 OpenCode 理解项目的代码结构、编码规范和技术栈。

This scans the project structure and generates an `AGENTS.md` file. This file helps OpenCode understand the codebase structure, coding conventions, and tech stack.

> **建议将 `AGENTS.md` 提交到 Git。/ Commit `AGENTS.md` to Git.**

---

## 基本用法 / Basic Usage

### 提问 / Ask Questions

直接用自然语言提问 / Ask in natural language:

```
How is the Semi2K protocol implemented? The operator dispatch chain?
```

### 引用文件 / Reference Files

使用 `@` 引用项目中的文件 / Use `@` to reference files:

```
Explain the operator dispatch in @src/context/basic/dispatch.h
```

### 执行命令 / Run Commands

使用 `!` 前缀执行 shell 命令 / Prefix with `!` to run shell commands:

```
!mkdir build && cd build && cmake .. && make -j$(nproc)
```

### 模式切换 / Mode Switching

按 **Tab** 键在两种模式间切换 / Press **Tab** to switch between modes:

| 模式 / Mode | 说明 / Description |
|:------------|:-------------------|
| **Build** | 允许修改文件，执行开发任务 / Can modify files, execute dev tasks |
| **Plan** | 仅规划，不修改文件。适合先讨论方案 / Plan only, no file changes. Good for discussing approaches first |

**推荐工作流 / Recommended workflow**: 先 Plan → 确认方案 → 切 Build → 执行。

---

## 开发工作流 / Development Workflow

### 1. 添加新功能 / Add New Features

```
# 先切到 Plan 模式讨论方案
<Tab>

I want to add a secure matrix multiplication operator to PPPU.
It should support Semi2K protocol and follow the existing operator dispatch pattern.
Please analyze the existing operators and propose an implementation plan.

# 确认方案后切回 Build 模式
<Tab>

Looks good! Go ahead and implement it.
```

### 2. 修 Bug / Fix Bugs

```
The comparison operator produces incorrect results when inputs have different
bit widths. Look at @src/context/basic/compare.h and fix the issue.
```

### 3. 代码审查 / Code Review

```
Review the changes in @src/context/basic/new_operator.h for potential
security issues, memory leaks, and compliance with our coding standards.
```

### 4. 撤销与重做 / Undo & Redo

如果对修改不满意 / If you're not happy with changes:

```
/undo    # 撤销上一次修改 / Undo last change
/redo    # 重做 / Redo
```

> 注意：需要项目是 Git 仓库。/ Note: Requires the project to be a Git repository.

---

## 为 Nezha Privacy 项目配置 / Configure for Nezha Privacy

### 项目级配置 / Project-Level Config

在项目根目录创建 `opencode.json` / Create `opencode.json` in project root:

```json
{
  "$schema": "https://opencode.ai/config.json",
  "model": "anthropic/claude-sonnet-4-5",
  "instructions": [
    "AGENTS.md",
    "doc/development/mpx.md",
    "doc/development/ndarray.md",
    "doc/development/network.md",
    "doc/development/serialization.md"
  ]
}
```

这样 OpenCode 会自动加载项目的开发文档作为上下文。

This makes OpenCode automatically load project dev docs as context.

### AGENTS.md 示例 / AGENTS.md Example

以下是适用于 PPPU 的 `AGENTS.md` 示例 / Example for PPPU:

```markdown
# PPPU — Privacy Processing Unit

## Project Overview
This is a C++20 MPC (Secure Multi-Party Computation) framework implementing
the Semi2K protocol. It provides secure arithmetic, comparison, math, and
shape operations.

## Project Structure
- `src/context/` — Core operator implementations (dispatch chain)
- `src/datatypes/` — Fixed-point, Z2k ring types
- `src/network/` — Boost.Asio networking layer
- `src/serialization/` — Serialization utilities
- `doc/` — Developer documentation

## Code Standards
- C++20 with GCC 13+
- Header-heavy design; most implementations are in `.h` files
- 5-layer operator dispatch chain: Player → Preprocessing → Protocol → Basic → Raw
- Use the existing dispatch pattern when adding new operators
- All MPC operations must go through the context layer

## Build
- CMake-based build system
- Dependencies: GMP, Boost, OpenMP, Eigen

## Important Notes
- Security-critical code — all changes must be reviewed
- Follow Semi2K protocol semantics strictly
- Test with at least 2-party and 3-party configurations
```

### 全局配置 / Global Config

如需在所有项目中使用统一设置 / For global settings across all projects:

```bash
# 创建全局配置目录 / Create global config directory
mkdir -p ~/.config/opencode

# 全局 AGENTS.md（个人偏好）/ Global AGENTS.md (personal preferences)
cat > ~/.config/opencode/AGENTS.md << 'EOF'
## Personal Preferences
- Use Chinese comments for documentation, English for code comments
- Always run `cmake --build build` after making changes to verify compilation
- Commit messages should follow Conventional Commits format
EOF
```

---

## 常用命令速查 / Command Cheat Sheet

### 斜杠命令 / Slash Commands

| 命令 / Command | 快捷键 / Keybind | 说明 / Description |
|:---------------|:-----------------|:-------------------|
| `/init` | `ctrl+x i` | 初始化 AGENTS.md / Initialize AGENTS.md |
| `/new` | `ctrl+x n` | 新建会话 / New session |
| `/sessions` | `ctrl+x l` | 切换会话 / Switch sessions |
| `/undo` | `ctrl+x u` | 撤销 / Undo |
| `/redo` | `ctrl+x r` | 重做 / Redo |
| `/compact` | `ctrl+x c` | 压缩上下文 / Compact context |
| `/models` | `ctrl+x m` | 切换模型 / Switch models |
| `/share` | `ctrl+x s` | 分享会话 / Share session |
| `/editor` | `ctrl+x e` | 外部编辑器 / Open external editor |
| `/help` | `ctrl+x h` | 帮助 / Help |
| `/exit` | `ctrl+x q` | 退出 / Exit |

### 快捷操作 / Quick Actions

| 操作 / Action | 方法 / Method |
|:-------------|:-------------|
| 引用文件 / Reference file | `@filename` |
| 执行命令 / Run command | `!command` |
| 切换模式 / Switch mode | `Tab` |
| 切换推理 / Toggle reasoning | `ctrl+t` |
| 命令面板 / Command palette | `ctrl+p` |

---

## 最佳实践 / Best Practices

### 1. 提供充分上下文 / Provide Sufficient Context

```
# 好的提示 / Good prompt
Look at the operator dispatch chain in @src/context/basic/dispatch.h
and @src/context/protocol/semi2k.h. I need to add a secure comparison
operator that handles mixed bit-width inputs. Follow the same pattern
as the existing arithmetic operators.

# 不好的提示 / Bad prompt
Add a comparison operator.
```

### 2. 先规划后执行 / Plan Before Build

对于复杂任务，先用 Plan 模式讨论方案，确认后再切换到 Build 模式执行。

For complex tasks, discuss the approach in Plan mode first, then switch to Build mode.

### 3. 善用 Undo / Use Undo Liberally

如果 AI 生成的代码不符合预期，直接 `/undo` 撤销，修改提示后重试。

If AI-generated code isn't what you expected, `/undo` and retry with a refined prompt.

### 4. 利用 AGENTS.md / Leverage AGENTS.md

在 `AGENTS.md` 中写清楚项目结构和编码规范，可以显著提高 AI 输出质量。

Document project structure and coding standards in `AGENTS.md` to significantly improve AI output quality.

### 5. 提交前审查 / Review Before Committing

OpenCode 修改文件后，务必用 `git diff` 检查变更，确认无误后再提交。

After OpenCode modifies files, always review changes with `git diff` before committing.

```
!git diff
```

---

## 参考链接 / References

- [OpenCode 官方文档 / Official Docs](https://opencode.ai/docs)
- [OpenCode GitHub](https://github.com/anomalyco/opencode)
- [Nezha Privacy 组织 / Organization](https://github.com/nezha-privacy)
- [PPPU 仓库 / Repository](https://github.com/nezha-privacy/PPPU)
