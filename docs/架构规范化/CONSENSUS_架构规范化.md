# CONSENSUS — 架构与代码规范整顿

## 最终共识

### 范围
对 pixel_hero 项目进行系统性架构整顿和代码规范化，解决 P0/P1/P2 三个层级的9个问题。

### P0 — 安全隐患修复

| 问题 | 方案 |
|------|------|
| 裸指针无所有权 | WaveManager/SurvivalPlayer/UpgradeUI/SurvivalHUD: Scene持有所有权，用 `std::unique_ptr` |
| 特效Timer UAF | 创建 `EffectManager` 类，Scene持有其unique_ptr。所有特效延迟清理由EffectManager的QTimer驱动。Scene析构→EffectManager析构→自动清理pending特效，无UAF |

### P1 — 架构重构

| 问题 | 方案 |
|------|------|
| GOD对象 | 拆分SurvivalScene为: GameLoop(update调度) + EnemyManager(生成/清理/AI) + 保留SurvivalScene(协调+特效) |
| 技能硬编码 | 创建 `SkillExecutor` 基类 + `FireballSkill`/`LightningSkill`/`FrostNovaSkill` 子类。通过data/json的skillId查找executor。新增技能=新增子类+注册 |
| 敌人数据双源 | 统一由 EnemyFactory::loadStats(JSON) 加载，Enemy构造器接收EnemyStats参数。消除C++ hardcode |

### P2 — 代码规范

| 问题 | 方案 |
|------|------|
| const缺失 | 所有getter加const；消除const_cast |
| items()重复扫描 | 每帧只扫描1次，在updateGame开头收集aliveEnemies列表，传递给各子系统 |
| 魔法数字 | 提取到常量: `SceneBounds{0,0,800,600}` / `ExpTable`(升级经验) / 敌人属性通过JSON配置 |
| 无命名空间 | 全部放入 `pixel_hero` 命名空间，子目录对应子命名空间 |

## 验收标准

1. 编译通过，所有功能正常工作
2. `clang-tidy` 或 `-Wall -Wextra` 无新增警告
3. const getter 覆盖率 100%
4. SurvivalScene 行数 < 250行(vs 当前~450行)
5. 所有 `new` 出的对象有明确的 unique_ptr 或 QObject 父对象管理
6. `items()` 每帧只调用1次
7. 所有新类位于 `pixel_hero` 命名空间内
