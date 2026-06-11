# 生存割草模式 — FINAL 项目总结报告

**项目**: 像素勇者 (Pixel Hero Adventure)
**文档版本**: FINAL V3.2
**日期**: 2026-06-11
**阶段**: 阶段6 — 评估阶段 (Assess)

---

## 一、项目概述

将像素勇者从"故事+生存"双模游戏**重构为纯生存割草游戏**，新增角色/武器选择系统。

### 重构规模

| 指标 | 数值 |
|------|:--:|
| 删除文件 | ~32个（.h/.cpp/png/wav/json） |
| 新增文件 | 22个（16 .h/.cpp + 3 .json + 2 .bat + 1 .qrc） |
| 改造文件 | 6个（Player/GameWindow/Menu/SurvivalPlayer/SurvivalScene/pixel_hero.pro） |
| 净代码变化 | 减少约1500行 |
| 目标平台 | Windows 横屏 800×600 |

---

## 二、验收总览

| 验收标准 | 结果 |
|------|:--:|
| AC1 启动→主菜单3选项 | ✅ |
| AC2 HP=0触发结算 | ✅ |
| AC3 杀敌→经验→升级→三选一 | ✅ |
| AC4 5技能3主动+2被动 | ✅ |
| AC5 6种敌人渐进生成 | ✅ |
| AC6 每5波精英/每10波Boss | ✅ |
| AC7 结算：时间/波次/击杀 | ✅ |
| AC8 编译零错误 | ✅ |
| AC9 无故事模式代码 | ✅ |
| AC10 最高纪录存盘 | ✅ |
| AC11 中途存档+读档 | ✅ |
| AC12 全中文界面 | ✅ |
| AC13 角色选择→武器选择→游戏 | ✅ |

**通过率**: 13/13 ✅

---

## 三、新增系统 (V2.0)

### 武器系统

```
Weapon.h (数据结构)
  ├── WeaponData: id/name/attackBonus/speedMod/rangeBonus/skillBonus/color/desc
  │
resources/data/weapons.json → GameData::loadWeaponData()
  │
  ├── Player::applyWeapon() → attackEnemy() 包含武器加成
  └── SurvivalScene::startGame(character, weapon)
```

**4武器**: 短剑(均衡) / 长剑(高攻低速) / 法杖(最高攻+技能加成) / 匕首(高速)

### 角色系统

```
SurvivalPlayer::CharacterConfig
  ├── 战士 (HP 120/ATK 12/DEF 5/SPD 3) — 均衡
  ├── 弓箭手 (HP 80/ATK 15/DEF 2/SPD 4) — 高攻脆皮
  └── 法师 (HP 70/ATK 20/DEF 1/SPD 3) — 玻璃大炮
```

### 选择界面

```
CharacterSelectUI (角色选择)
  ├── 3卡片居中排列
  ├── 属性条形图 (HP红/ATK橙/DEF蓝/SPD绿)
  └── A/D ← → 切换  Enter 确认

WeaponSelectUI (武器选择)
  ├── 4卡片居中排列
  ├── 属性行 (攻击/攻速/范围/技能)
  └── A/D ← → 切换  Enter 确认
```

---

## 四、13个原子任务完成情况

| 任务 | 名称 | 状态 |
|:--:|------|:--:|
| T1 | JSON数据文件 | ✅ 完成 |
| T2 | GameData扩展 | ✅ 完成 |
| T3 | 删除旧代码+改造Player | ✅ 完成 |
| T4 | Enemy扩展(6类型) | ✅ 完成 |
| T5 | EnemyFactory | ✅ 完成 |
| T6 | WaveManager | ✅ 完成 |
| T7 | SurvivalPlayer | ✅ 完成 |
| T8 | SurvivalHUD | ✅ 完成 |
| T9 | UpgradeUI | ✅ 完成 |
| T10 | SurvivalScene | ✅ 完成 |
| T11 | SurvivalStats | ✅ 完成 |
| T12 | GameWindow+Menu简化 | ✅ 完成 |
| T13 | 编译配置 | ✅ 完成 |

**完成率**: 13/13 ✅

---

## 五、技术栈

| 组件 | 版本 |
|------|------|
| 语言 | C++17 |
| 框架 | Qt 6.11.1 (Widgets + Core + Gui) |
| 编译器 | MinGW 16.1.0 |
| 构建 | qmake 3.1 + mingw32-make |
| 图形 | QMainWindow → QGraphicsView → QGraphicsScene → QGraphicsPixmapItem |

### 依赖清单

- Qt 6.11.1 (core/gui/widgets) — 无其他外部库
- C++17 标准库: `<cmath>`, `<algorithm>`, `<random>`, `<ctime>`
- 无第三方脚本、无音效、无Python/JS依赖

---

## 六、代码架构

```
main.cpp
  └── GameWindow
        ├── Menu (新游戏/读档/退出)
        ├── CharacterSelectUI (角色选择 — 3卡片)
        ├── WeaponSelectUI (武器选择 — 4卡片)
        └── SurvivalScene
              ├── SurvivalPlayer (3角色 × 4武器)
              ├── WaveManager → EnemyFactory → Enemy (6种)
              ├── UpgradeUI (技能升级三选一)
              ├── SurvivalHUD (计时/波次/击杀/HP)
              └── SurvivalStats (统计+纪录+存档)

共享层: GameData / WeaponData / SaveManager / ResourceManager / AnimationManager / CharacterConfig
```

### 游戏流程

```
启动 → 主菜单
  └── "开始游戏" → CharacterSelectUI → WeaponSelectUI → 生存游戏
  └── "读取存档" → 直接进入生存游戏
  └── "退出游戏" → 关闭

游戏中:
  WASD 移动    ESC 暂停
  杀敌 → 经验 → 升级 → 三选一技能
  HP=0 → 结算(时间/波次/击杀/纪录)
```

---

## 七、已知问题

### V2.0 已修复
所有原始 P0/P1/P2 缺陷已在 V2.0 中解决。

### V3.0 grill-me 审查发现 (2026-06-11)

| 级别 | # | 问题 | 状态 |
|:--:|:--:|------|:--:|
| P0 | 1 | UpgradeUI 的 A/S/D 键映射与 WASD 移动冲突 | ✅ 已修复 |
| P0 | 2 | 各界面鼠标确认行为不一致 | ✅ 已修复 |
| P1 | 1 | GameOver 使用原生 QMessageBox | ✅ V3.2 自绘 Menu 替代 |
| P1 | 2 | UpgradeUI 无法跳过/取消 | ✅ V3.2 Esc 跳过 |
| P1 | 3 | 导航键方向不统一 | ✅ P0-1修复后自然解决 |
| P1 | 4 | SaveLoadUI 空槽静默失败 | ✅ V3.1 基类重构顺带修复 |

---

## 八、文件清单

### 源代码 (22个 .h/.cpp)

| 目录 | 文件 |
|------|------|
| `src/` | `main.cpp`, `GameWindow.h/cpp` |
| `src/entities/` | `Player.h/cpp`, `Enemy.h/cpp`, `Weapon.h` |
| `src/survival/` | `SurvivalScene.h/cpp`, `SurvivalPlayer.h/cpp`, `WaveManager.h/cpp`, `EnemyFactory.h/cpp`, `UpgradeUI.h/cpp`, `SurvivalHUD.h/cpp`, `SurvivalStats.h/cpp` |
| `src/ui/` | `Menu.h/cpp`, `CharacterSelectUI.h/cpp`, `WeaponSelectUI.h/cpp` |
| `src/utils/` | `GameData.h/cpp`, `SaveManager.h/cpp`, `ResourceManager.h/cpp`, `AnimationManager.h/cpp` |

### 数据文件

| 文件 | 说明 |
|------|------|
| `resources/data/skills.json` | 5技能×5等级 |
| `resources/data/survival_waves.json` | 20波+6敌人属性 |
| `resources/data/weapons.json` | 4武器（短剑/长剑/法杖/匕首） |

### 构建/部署文件

| 文件 | 说明 |
|------|------|
| `pixel_hero.pro` | qmake 项目文件 |
| `resources.qrc` | Qt 资源文件 |
| `scripts/dev.bat` | 一键编译+部署DLL+运行 (5步) |
| `scripts/build.bat` | 一键编译 |
| `scripts/run.bat` | 一键运行 |
| `qt.conf` | Qt 运行时路径配置 |
| `.trae/commands/dev.md` | Trae `/dev` 命令定义 |

---

## 九、V3.0 P0 修复详情 (2026-06-11)

### P0-1: UpgradeUI 移除 A/S/D 键映射冲突

**文件**: `src/survival/UpgradeUI.cpp`

**问题**: 游戏中 A/S/D = 移动，升级界面 A/S/D = 选择技能，认知冲突导致误操作。

**修复**: 删除 `Qt::Key_A`, `Qt::Key_S`, `Qt::Key_D` 映射，仅保留 `Qt::Key_1/2/3` 数字键和 `Qt::Key_Left/Right` 方向键。

### P0-2: 统一鼠标确认行为

**文件**: `src/survival/UpgradeUI.cpp`

**问题**: Menu 单击即确认、卡片选择 UI 点两次确认、存档 UI 点选后需 Enter——三种不同逻辑。

**统一规则**:
| 界面类型 | 鼠标行为 |
|---------|---------|
| 列表菜单 (Menu) | 单击 = 确认 (标准行为) |
| 卡片选择 (Character/Weapon/Upgrade) | 单击选中 + 再次点击或 Enter 确认 |
| 存档槽 (SaveLoad) | 单击选中 + Enter 确认 |

**改动**: `UpgradeUI::mousePressEvent` — 从"单击即触发 `selectOption()`"改为"首次点击仅高亮，再次点击同一卡片才触发"。

---

### V3.1 SelectableListBase 基类抽取 (2026-06-11)

**问题**: 5个UI组件(Menu/CharacterSelect/WeaponSelect/UpgradeUI/SaveLoadUI)各自实现了独立的交互逻辑，导致 ~500行重复代码 + 交互行为不一致。

**方案**: 抽取 `SelectableListBase` — `QObject + QGraphicsItem` 基类，统一管理公共交互逻辑。

**基类提供**:
| 功能 | 方法 |
|------|------|
| 选中状态 | `m_selectedIndex`, `selectedIndex()`, `setSelectedIndex()` |
| 可见性 | `m_visible`, `isVisible()`, `appear()`, `dismiss()` |
| 导航(跳过禁用项) | `selectPrev()`, `selectNext()`, 支持循环/非循环 |
| 禁用管理 | `setItemDisabled()`, `isItemDisabled()` |
| 确认机制 | `confirm()` → `onConfirm()` + `confirmed(int)` 信号 |
| 键盘事件 | W/↑/A/← = prev, S/↓/D/→ = next, Enter/Space = confirm |
| 鼠标事件 | 统一支持 `ClickToConfirm`(菜单) 和 `ClickToSelect`(卡片) |
| 滚轮事件 | 上下滚动 = prev/next |
| 扩展钩子 | `handleExtraKey(int)` 子类可处理额外按键 |

**子类只需实现**: `paint()` + `itemRect()` + `onConfirm()`

**消除重复代码**: 约 600行 → 200行基类 + 子类各 ~30行交互代码

**顺带修复**:
- SaveLoadUI LOAD模式: 空槽自动禁用(解决 P1-4 空槽静默失败)
- 所有卡片类统一为 ClickToSelect 模式

### 文件变更

| 操作 | 文件 |
|------|------|
| 新增 | `src/ui/SelectableListBase.h` (94行) |
| 新增 | `src/ui/SelectableListBase.cpp` (176行) |
| 重构 | `src/ui/Menu.h/cpp` (继承基类, 移除状态/事件) |
| 重构 | `src/ui/CharacterSelectUI.h/cpp` (继承基类) |
| 重构 | `src/ui/WeaponSelectUI.h/cpp` (继承基类) |
| 重构 | `src/ui/SaveLoadUI.h/cpp` (继承基类+空槽禁用) |
| 重构 | `src/survival/UpgradeUI.h/cpp` (继承基类, 1/2/3 via handleExtraKey) |
| 适配 | `src/GameWindow.cpp` (m_menu->show() → appear()) |
| 配置 | `pixel_hero.pro` (添加新文件) |

---

### V3.2 P1 修复 (2026-06-11)

#### P1-1: GameOver 自绘替代 QMessageBox

**文件**: `src/ui/Menu.h/cpp`, `src/GameWindow.cpp`

**问题**: GameOver 使用 Windows 原生 `QMessageBox::information`，撕裂像素风沉浸感。

**修复**: Menu 新增 `setGameOverStats()` + `m_statsText`，GAME_OVER 模式下在菜单上方绘制统计数据。GameWindow 改为创建 Menu(GAME_OVER_MENU) 替代 QMessageBox。

#### P1-2: UpgradeUI Esc 跳过

**文件**: `src/survival/UpgradeUI.h/cpp`, `src/survival/SurvivalScene.h/cpp`

**问题**: 升级UI弹出后必须选择技能，无法取消。

**修复**: UpgradeUI 新增 `skillSkipped()` 信号，Esc 键触发。SurvivalScene 连接后：隐藏UI → 仍应用属性加成 → 恢复游戏（不选技能也能获得升级基础属性）。

#### P1-4: 升级过渡动画

**文件**: `src/survival/SurvivalHUD.h/cpp`, `src/survival/SurvivalScene.cpp`

**问题**: 战斗中突然冻结弹出升级UI，玩家不知发生了什么。

**修复**: 升级时先在 HUD 中央显示 "LEVEL UP!" 金色横幅（带发光描边效果），0.6秒后再弹出 UpgradeUI。

---

## 十、构建命令

```powershell
# 一键构建+部署+运行
.\scripts\dev.bat

# 或使用 Trae 命令
/dev
```

`dev.bat` 自动执行:
1. Clean 清理
2. qmake 生成 Makefile
3. Patch 移除 Qt6EntryPoint
4. Compile 编译
5. Deploy 自动 windeployqt (首次) + 复制 qt.conf
6. Run 启动游戏

---

**文档状态**: ✅ V3.2 — P0 + P1 全部修复，界面交互统一
