# 生存割草模式 — ALIGNMENT 文档 (修订版)

**项目**: 像素勇者 (Pixel Hero Adventure)
**文档版本**: ALIGNMENT V2.0
**修订日期**: 2026-06-10
**修订原因**: 删除故事模式、胜利改为死亡结算、删除装备系统
**阶段**: 阶段1 — 对齐阶段 (Align)

---

## 一、三次关键需求变更

| # | 变更 | 原方案 | 新方案 | 影响 |
|:--:|------|--------|--------|------|
| 1 | 删除故事模式 | 故事+生存双入口 | **纯生存模式，单一游戏循环** | 删除 GameScene/MapManager/NPC/Item/Inventory/Tile/CollisionLayer 等 |
| 2 | 胜利条件 | 存活15分钟或死亡 | **仅死亡结算，无时间限制** | 删除计时器胜利判断，HUD 改为正向计时 |
| 3 | 删除装备系统 | Player 有 Weapon/Armor/Inventory | **纯属性+技能系统** | 删除 Weapon/Armor 类，简化 Player |

---

## 二、项目上下文分析 (更新)

### 2.1 技术栈

| 组件 | 版本/说明 |
|------|----------|
| 语言 | C++17 |
| 框架 | Qt 6.11.1 (Widgets + Core + Gui) |
| 编译器 | MinGW 16.1.0 |
| 构建工具 | qmake 3.1 + mingw32-make |
| 图形架构 | QMainWindow → QGraphicsView → QGraphicsScene → QGraphicsPixmapItem |
| 平台 | Windows PC 横屏 800×600 |

### 2.2 代码资产 — 复用/删除决策

| 模块 | 文件 | 原决策 | 新决策 | 说明 |
|------|------|:------:|:------:|------|
| GameWindow | `src/GameWindow.h/cpp` | 复用 | ✅ **复用** | 简化为单一生存模式入口 |
| GameScene | `src/GameScene.h/cpp` | 复用(改造) | ❌ **删除** | 被 SurvivalScene 完全替代 |
| GameController | `src/GameController.h/cpp` | 复用 | ✅ **复用** | 简化，去掉E键交互 |
| Player | `src/entities/Player.h/cpp` | 复用 | ⚠️ **改造** | 删除 Weapon/Armor/Inventory 接口 |
| Enemy | `src/entities/Enemy.h/cpp` | 扩展 | ✅ **扩展** | 新增 Bat/Elite/Dragon 类型 |
| BattleSystem | `src/battle/BattleSystem.h/cpp` | 复用 | ❌ **删除** | 战斗逻辑合并到 SurvivalScene |
| Skill | `src/battle/Skill.h/cpp` | 扩大 | ⚠️ **改造** | 简化为 ActiveSkill 数据 |
| Weapon | `src/battle/Weapon.h/cpp` | 删除 | ❌ **删除** | 装备系统全删 |
| Armor | `src/entities/Armor.h/cpp` | 删除 | ❌ **删除** | 装备系统全删 |
| Item | `src/entities/Item.h/cpp` | 部分保留 | ❌ **删除** | 无道具系统 |
| NPC | `src/entities/NPC.h/cpp` | 删除 | ❌ **删除** | 无NPC |
| MapManager | `src/map/MapManager.h/cpp` | 改造 | ❌ **删除** | 无地图 |
| Tile | `src/map/Tile.h/cpp` | 删除 | ❌ **删除** | 无瓦片 |
| CollisionLayer | `src/map/CollisionLayer.h/cpp` | 保留 | ❌ **删除** | 边界碰撞在 SurvivalScene 实现 |
| HUD | `src/ui/HUD.h/cpp` | 改造 | ❌ **删除** | 被 SurvivalHUD 替代 |
| Menu | `src/ui/Menu.h/cpp` | 复用 | ✅ **复用** | 保留主菜单 |
| Inventory | `src/ui/Inventory.h/cpp` | 删除 | ❌ **删除** | 无背包 |
| GameData | `src/utils/GameData.h/cpp` | 扩展 | ✅ **扩展** | 加载 skills.json + waves.json |
| SaveManager | `src/utils/SaveManager.h/cpp` | 保留 | ✅ **复用** | 纪录存档 |
| ResourceManager | `src/utils/ResourceManager.h/cpp` | 复用 | ✅ **复用** | 图片缓存 |
| AnimationManager | `src/utils/AnimationManager.h/cpp` | 复用 | ✅ **复用** | 动画 |
| SaveManager 故事存档 | 保留 | ❌ **删除** | 仅保留生存纪录存档 |

### 2.3 文件变更统计

| 操作 | 数量 | 文件 |
|:----:|:----:|------|
| **删除** | 17个 .h/.cpp | Item, Armor, Weapon, NPC, Tile, CollisionLayer, BattleSystem, Inventory, GameScene, MapManager, HUD |
| **改造** | 2个 | Player (去装备), Skill (简化) |
| **新增** | 8个 .h/.cpp | SurvivalScene, WaveManager, UpgradeUI, SurvivalHUD, SurvivalPlayer, SurvivalStats, EnemyFactory |
| **修改** | 3个 | GameWindow (简化), GameController (简化), Menu (去故事模式选项) |
| **不动** | 4个 | GameData(扩展), SaveManager, ResourceManager, AnimationManager, main.cpp |

---

## 三、原始需求 (更新)

| # | 决策维度 | 选择 | 说明 |
|:--:|---------|:----:|------|
| 1 | 核心体验 | A | **爽快割草** |
| 2 | 平台画幅 | A | PC 横屏 800×600 |
| 3 | 差异化 | E | 先扎实基础 |
| 4 | 单局结构 | B | **生存模式** — 仅死亡结算 |
| 5 | 战斗节奏 | B | 适中（20-30敌人，普通/精英/Boss分层） |
| 6 | 成长体系 | A | 局内成长为主 |
| 7 | 技能系统 | B | Roguelike 三选一 |
| 8 | 敌人波次 | B+C | 渐进生成 + 每N波精英/Boss |
| 9 | 局外解锁 | D | 纯局内 |
| 10 | 单局时长 | - | ~~15分钟~~ → **无限，直至死亡** |
| 11 | 技能池 | C | 30+（MVP: 5个） |
| 12 | 敌人种类 | C | 25+（MVP: 6种） |
| 13 | 生存场景 | A | 单一场地，无障碍物 |
| 14 | ~~故事模式~~ | - | **已删除** |
| 15 | ~~装备系统~~ | - | **已删除** |

---

## 四、边界确认 (更新)

### 4.1 MVP 范围

| 范围 | 说明 |
|------|------|
| **包含** | 生存模式核心框架（波次系统 + 升级选技能 + 渐进敌人生成） |
| **包含** | MVP: 5个技能 + 6种敌人 |
| **包含** | SurvivalHUD（波次/存活时间/击杀数/等级/HP） |
| **包含** | 单一主菜单（开始/读档/退出） |
| **包含** | 局内跨局统计（最高波次/击杀数/存活时间） |
| **包含** | 中途退出存档 + 读档继续 |
| **不包含** | ~~30+技能池~~ 完整技能池 |
| **不包含** | ~~25+敌人~~ 完整敌人体系 |
| **不包含** | ~~音效~~ |
| **不包含** | ~~故事模式~~ |
| **不包含** | ~~装备/背包/护甲/道具~~ |
| **不包含** | ~~NPC/商店/任务/地图/关卡~~ |
| **不包含** | ~~局外成长/角色解锁~~ |

### 4.2 技术边界

| 边界 | 决定 |
|------|------|
| 架构方式 | **大幅简化** — 删除故事模式全部代码，仅保留生存模式 |
| 数据管理 | skills.json + survival_waves.json |
| 资源生成 | generate_assets.py 生成新敌人精灵 |
| 依赖限制 | Qt 6.11.1 core/gui/widgets，不引入新模块 |

### 4.3 验收标准 (更新)

| # | 标准 | 验证方式 |
|:--:|------|---------|
| AC1 | 启动进入主菜单（仅生存模式相关选项） | 点击开始/读档/退出 |
| AC2 | 玩家死亡触发结算 | HP=0 → 结算画面 |
| AC3 | 杀敌获得经验，升级弹出三选一 | 击杀后经验涨满→升级弹窗 |
| AC4 | 5个可选技能（3主动+2被动） | 升级时三选一包含 |
| AC5 | 6种敌人渐进生成 | 随时间/波次出现不同敌人 |
| AC6 | 每5波精英，每10波Boss | 固定波次触发 |
| AC7 | 结算画面显示存活时间/波次/击杀数 | 死亡后弹窗 |
| AC8 | 编译通过 | mingw32-make 零错误 |
| AC9 | ~~故事模式不受影响~~ → **故事模式已不存在** | 代码基准无冗余 |

---

## 五、核心循环 (更新)

```
开局 (Level 1, 无技能, 0波次)
    ↓
杀敌 → 涨经验 → 升级 (三选一技能)
    ↓
技能变强 → 杀敌更快 → 更多经验 → 更多升级
    ↓
敌人渐进变强 (新种类 + 更高密度 + 更快生成)
    ↓
         ↓ 玩家死亡
    结算画面 (存活时间/波次/击杀数)
```

---

## 六、需求理解确认

### 6.1 关键数值原型 (MVP 更新)

| 参数 | 值 | 说明 |
|------|:--:|------|
| 升级所需经验 | `level × 80` | ~5-8个敌人升1级 |
| 玩家初始HP | 100 | - |
| 初始敌人密度 | 5-8个 | 开局 |
| 最大敌人密度 | 25-30个 | 逐步达到 |
| 敌人生成间隔 | 3秒 → 0.8秒 | 随时间递减 |
| 精英波触发 | 每5波 | 精英 × 1-3 只 |
| Boss波触发 | 每10波 | Boss × 1 只 |
| HUD计时 | **正向计时** | 从 00:00 开始递增 |

### 6.2 简化后的代码架构

```
main.cpp
  └── GameWindow (简化为单入口)
        └── SurvivalScene
              ├── SurvivalPlayer (无装备的简化Player)
              ├── WaveManager → EnemyFactory → Enemy
              ├── UpgradeUI (三选一)
              ├── SurvivalHUD
              └── SurvivalStats (纪录+存档)
```

### 6.3 删除文件完整清单

| 目录 | 文件 |
|------|------|
| `src/` | `GameScene.h/cpp`, `GameWindow.h/cpp`(改造), `GameController.h/cpp`(改造) |
| `src/entities/` | `Item.h/cpp`, `Armor.h/cpp`, `NPC.h/cpp` |
| `src/map/` | `MapManager.h/cpp`, `Tile.h/cpp`, `CollisionLayer.h/cpp` |
| `src/battle/` | `BattleSystem.h/cpp`, `Weapon.h/cpp` |
| `src/ui/` | `HUD.h/cpp`, `Inventory.h/cpp` |

---

## 七、疑问全部解决

| # | 疑问 | 结论 |
|:--:|------|------|
| Q1 | 故事模式存留 | **删除** |
| Q2 | 胜利条件 | **仅死亡结算** |
| Q3 | 装备系统 | **删除** |
| Q4 | 音效 | 待P1 |
| Q5 | 界面语言 | 全中文 |
| Q6 | 存档 | 存最高纪录 + 中途退出可读档 |

---

**文档状态**: ✅ V2.0 已完成
**下一步**: 更新 CONSENSUS → DESIGN → TASK
