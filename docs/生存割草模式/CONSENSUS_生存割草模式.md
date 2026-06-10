# 生存割草模式 — CONSENSUS 文档 (修订版)

**项目**: 像素勇者 (Pixel Hero Adventure)
**文档版本**: CONSENSUS V2.0
**修订日期**: 2026-06-10
**修订原因**: 删除故事模式、胜利改为死亡结算、删除装备系统
**阶段**: 阶段1 — 对齐阶段 (Align)
**前置文档**: [ALIGNMENT_生存割草模式.md](./ALIGNMENT_生存割草模式.md)

---

## 一、最终需求

### 1.1 一句话需求

**将像素勇者重构为纯生存割草游戏**：删除故事模式/装备/背包/地图/NPC等系统，保留核心战斗引擎，新增波次渐进生成+Roguelike三选一技能升级，一局进行到玩家死亡结算为止。

### 1.2 验收标准 (9条)

| # | 标准 | 验收方式 | 优先级 |
|:--:|------|---------|:------:|
| AC1 | 启动→主菜单（开始/读档/退出），无故事模式入口 | 主菜单仅3选项 | P0 |
| AC2 | 玩家死亡(HP=0)触发结算 | 结算画面弹出 | P0 |
| AC3 | 杀敌→经验→升级→三选一技能 | 升级弹窗，3张卡片可选 | P0 |
| AC4 | 5个技能（3主动+2被动），可升级强化 | 火球/闪电/冰环/攻击强化/敏捷 | P0 |
| AC5 | 6种敌人渐进生成 | 史莱姆/哥布林/骷髅/蝙蝠/精英/Boss | P0 |
| AC6 | 每5波精英，每10波Boss | 固定波次触发 | P0 |
| AC7 | 结算画面：存活时间/波次/击杀数 | 死亡后弹窗 | P0 |
| AC8 | 编译零错误 | mingw32-make 通过 | P0 |
| AC9 | 代码无冗余—故事模式全删 | 不需的文件不存在 | P0 |

### 1.3 扩展验收标准

| # | 标准 | 验收方式 | 优先级 |
|:--:|------|---------|:------:|
| AC10 | 最高纪录存盘（波次/击杀/时间） | 结算显示"新纪录!" | P1 |
| AC11 | 中途退出→存档→读档继续 | 暂停→退出→主菜单→继续 | P1 |
| AC12 | 全中文界面 | 所有文字中文 | P1 |

---

## 二、技术实现方案 (更新)

### 2.1 架构：代码大幅简化

```
main.cpp
  └── GameWindow (简化为单入口)
        ├── Menu (新游戏/读档/退出)
        └── SurvivalScene
              ├── SurvivalPlayer (继承简化Player)
              ├── WaveManager (波次+渐进生成)
              │      └── EnemyFactory → Enemy (6种)
              ├── UpgradeUI (三选一弹窗)
              ├── SurvivalHUD (正向计时/波次/击杀/HP)
              └── SurvivalStats (纪录+存档)
```

### 2.2 新增文件 (8个)

| 文件 | 说明 |
|------|------|
| `src/survival/SurvivalScene.h/cpp` | 生存主场景（60fps循环） |
| `src/survival/WaveManager.h/cpp` | 波次管理+敌人生成 |
| `src/survival/UpgradeUI.h/cpp` | 升级三选一弹窗 |
| `src/survival/SurvivalHUD.h/cpp` | 正向计时+波次+击杀+HP |
| `src/survival/SurvivalPlayer.h/cpp` | 简化玩家（无装备） |
| `src/survival/SurvivalStats.h/cpp` | 统计+纪录+中途存档 |
| `src/survival/EnemyFactory.h/cpp` | 6种敌人工厂 |
| `resources/data/skills.json` | 5技能×5等级 |
| `resources/data/survival_waves.json` | 20波+敌人属性 |

### 2.3 修改文件 (4个)

| 文件 | 改动 |
|------|------|
| `src/GameWindow.h/cpp` | 简化为单生存模式入口，删除故事模式分支 |
| `src/GameController.h/cpp` | 删除E键交互/NPC相关 |
| `src/ui/Menu.h/cpp` | 删除"故事模式"选项 |
| `src/utils/GameData.h/cpp` | 新增 loadSkillData/loadWaveData |
| `pixel_hero.pro` | 删除旧文件 + 新增 survival 目录 |
| `resources.qrc` | 删除不再需要的资源 + 新增JSON |

### 2.4 删除文件 (14个 .h + 14个 .cpp = 28个文件)

```
src/GameScene.h/cpp
src/entities/Item.h/cpp
src/entities/Armor.h/cpp
src/entities/NPC.h/cpp
src/battle/BattleSystem.h/cpp
src/battle/Weapon.h/cpp
src/map/MapManager.h/cpp
src/map/Tile.h/cpp
src/map/CollisionLayer.h/cpp
src/ui/HUD.h/cpp
src/ui/Inventory.h/cpp
```

### 2.5 改造文件 (1个)

| 文件 | 改动 |
|------|------|
| `src/entities/Player.h/cpp` | 删除 Weapon/Armor/Inventory 成员和方法 |

### 2.6 不动文件

| 文件 | 说明 |
|------|------|
| `main.cpp` | 不动 |
| `src/entities/Enemy.h/cpp` | 扩展Bat/Elite/Dragon枚举 |
| `src/utils/ResourceManager.h/cpp` | 不动 |
| `src/utils/AnimationManager.h/cpp` | 不动 |
| `src/utils/SaveManager.h/cpp` | 复用 |

---

## 三、接口契约 (更新)

### 3.1 SurvivalScene

```cpp
class SurvivalScene : public QGraphicsScene {
    Q_OBJECT
public:
    void startGame();
    void pauseGame();
    void resumeGame();
    void endGame();  // 仅死亡触发，无胜利条件

    SurvivalPlayer* player() const;
    WaveManager* waveManager() const;
    SurvivalStats* stats() const;

signals:
    void gameFinished(SurvivalStats* stats);  // 死亡结算
    void levelUp(int newLevel);               // 升级→三选一
};
```

### 3.2 SurvivalPlayer (简化)

```cpp
class SurvivalPlayer : public Player {  // 继承简化后的Player
public:
    SurvivalPlayer(QGraphicsItem* parent = nullptr);

    void addSkill(const QString& skillId, int level);
    void upgradeSkill(const QString& skillId, int newLevel);
    
    int attack() const override;   // 基础值 + 被动加成
    int speed() const override;    // 基础值 + 被动加成

    void update(qreal deltaTime) override; // 技能冷却管理
};
```

### 3.3 WaveManager — 同 DESIGN 文档

### 3.4 UpgradeUI — 同 DESIGN 文档

### 3.5 SurvivalHUD

```cpp
class SurvivalHUD : public QObject, public QGraphicsItem {
    Q_OBJECT
public:
    void bind(SurvivalPlayer* player, WaveManager* waveManager);
    void updateHUD();
    // 显示: HP血条/等级/经验条/波次/击杀/正向计时
};
```

---

## 四、数据契约 (更新)

### 4.1 skills.json — 同原 DESIGN 文档 (5技能×5等级)

### 4.2 survival_waves.json — 同原 DESIGN 文档 (20波)

波次数据不变，但不再有15分钟限制。波次按时间递增，直到玩家死亡。

---

## 五、任务边界

### 5.1 包含

- 纯生存模式 + Roguelike技能升级
- 5技能 + 6敌人 MVP
- 死亡结算 + 最高纪录
- 中途存档+读档
- 删除故事模式所有代码

### 5.2 不包含

- 故事模式/装备/背包/NPC/地图/关卡
- 音效集成
- 30+技能池 / 25+敌人体系
- 局外成长

---

**文档状态**: ✅ V2.0 已完成
**下一步**: 更新 DESIGN 文档
