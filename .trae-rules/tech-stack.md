# 像素勇者 — 技术栈规范

## 一、硬约束（不可违背）

### 1.1 构建链

| 项 | 值 | 备注 |
|:--|------|------|
| 编译器 | **MinGW 16.1.0** (`E:\MinGW-w64\mingw64\bin`) | 不可更换为 MSVC |
| Qt 版本 | **Qt 6.11.1** (`E:\QT\6.11.1\mingw_64\bin`) | 不可升级/降级 |
| C++ 标准 | **C++17** | `CONFIG += c++17` |
| 构建工具 | **qmake 3.1 + mingw32-make** | 不可改为 CMake |
| 目标平台 | **Windows 64-bit** | 横屏 800×600 |

### 1.2 Qt 模块白名单

```pro
QT += core gui widgets
```

**仅允许这三个模块。** 禁止引入：

- ❌ `QT += sql network multimedia webengine qml quick`
- ❌ 任何第三方 C++ 库（通过 .pro 的 LIBS 引入）

### 1.3 标准库白名单

| 头文件 | 用途 | 状态 |
|------|------|:--:|
| `<cmath>` | 数学计算 | ✅ 允许 |
| `<algorithm>` | 排序/洗牌 | ✅ 允许 |
| `<random>` | 随机数生成 | ✅ 允许 |
| `<ctime>`, `<cstdlib>` | 时间种子 | ✅ 允许 |
| `<windows.h>` | WinMain 兼容 | ⚠️ 仅 main.cpp |

**禁止引入** `<iostream>`, `<fstream>`, `<thread>`, `<mutex>`, `<filesystem>`, `<regex>`, `<any>` 等任何未在白名单的标准库头。

### 1.4 图形架构

```
QMainWindow → QGraphicsView → QGraphicsScene → QGraphicsPixmapItem
```

- 精灵用 `QPixmap.fill()` 纯色矩形，**不加载外部PNG/JPG**
- 禁止 QML、QPainter 自由绘制替代 QGraphicsItem

### 1.5 禁止引入

- ❌ 任何 `.py` / `.js` / `.vbs` 脚本（含资源生成脚本）
- ❌ 任何音效文件（wav/mp3）
- ❌ 任何 JSON 库（用 Qt 内置 QJsonDocument）
- ❌ 任何第三方库

---

## 二、软约束（推荐遵守）

### 2.1 命名规范

| 类型 | 规则 | 示例 |
|------|------|------|
| 类名 | PascalCase | `SurvivalScene`, `WaveManager` |
| 成员变量 | `m_camelCase` | `m_player`, `m_totalKills` |
| 方法名 | camelCase | `startGame()`, `updateHUD()` |
| 文件名 | 与类名一致 | `SurvivalScene.h/cpp` |
| JSON key | camelCase | `"maxHealth"`, `"spawnInterval"` |
| include 路径 | 以 `src/` 为根 | `#include "survival/WaveManager.h"` |

### 2.2 目录约定

```
pixel_hero/
├── src/
│   ├── config/       # 集中常量 (GameConfig.h)
│   ├── entities/     # 实体基类 (Player, Enemy)
│   ├── survival/     # 生存模式所有类
│   │   └── skills/   # 技能策略子类 (SkillExecutor)
│   ├── ui/           # UI (Menu)
│   └── utils/        # 单例工具 (GameData, SaveManager...)
├── resources/
│   ├── data/         # JSON 配置 (skills.json, survival_waves.json)
│   └── png/          # 精灵图资源
├── scripts/
│   └── build.bat     # 构建脚本
└── resources.qrc     # Qt资源文件
```

### 2.3 单例模式

`GameData`, `ResourceManager`, `AnimationManager`, `SaveManager` 使用 `static instance()` 单例模式。

### 2.4 头文件保护

统一使用 `#ifndef` / `#define` / `#endif`，**不使用 `#pragma once`**。

### 2.5 编码规范

- 字符串用 `QString`，禁止 `std::string`
- 容器用 `QList` / `QMap` / `QPair`，禁止 `std::vector` / `std::map` / `std::pair`
- 文件IO用 `QFile`，禁止 `std::fstream`
- 随机数用 `QRandomGenerator::global()`，不由 AI 实现 `rand()` 包装
- JSON 用 `QJsonDocument` / `QJsonObject` / `QJsonArray`
- 存档路径用 `QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)`

---

## 三、编译流程

### 3.1 必须使用的编译命令

```powershell
qmake.exe pixel_hero.pro
# 然后修补 Makefile.Release 移除 QT_NEEDS_QMAIN 和 libQt6EntryPoint.a
mingw32-make.exe -f Makefile.Release -j4
```

### 3.2 .pro 文件必须保留项

```
DEFINES -= QT_NEEDS_QMAIN        # MinGW 16.1.0 兼容
QMAKE_LIBS_QT_ENTRY =            # 移除 entry point
```

### 3.3 测试策略

- **不引入** Google Test / Catch2 等外部测试框架
- 不编写单元测试文件（此项目纯手工验证）

---

## 四、数据文件

### 4.1 JSON 只能通过 qrc 加载

```cpp
GameData::instance()->loadSkillData(":/resources/data/skills.json");
GameData::instance()->loadWaveData(":/resources/data/survival_waves.json");
```

### 4.2 JSON 异常回退

所有 JSON 解析失败必须回退到硬编码默认值（`initDefaultSkills()` / `initDefaultWaves()`）。

---

## 五、禁止事项速查

| 禁止 | 替代方案 |
|------|---------|
| `#pragma once` | `#ifndef ... #define ... #endif` |
| `std::string` | `QString` |
| `std::vector/map/pair` | `QList/QMap/QPair` |
| `std::fstream` | `QFile` |
| `std::cout/cerr` | `qDebug()` (Qt 内置) |
| CMake | qmake |
| 任何第三方库 | Qt 6 内置功能 |
| PNG/JPG 精灵加载 | `QPixmap.fill(QColor(...))` |
| 音效文件 | 无音效 |
| Python 脚本 | 无额外脚本 |
| 外部测试框架 | 手动验证 |

---

## 六、配置性路径

以下路径为环境相关，不应硬编码在代码中：

| 路径 | 来源 |
|------|------|
| `E:\MinGW-w64\mingw64\bin` | build.bat / 系统 PATH |
| `E:\QT\6.11.1\mingw_64\bin` | build.bat / 系统 PATH |
| 存档目录 | `QStandardPaths::AppDataLocation` |

代码中**绝不硬编码** `E:\` 等绝对路径。
