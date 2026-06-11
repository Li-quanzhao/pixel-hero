# ACCEPTANCE — 架构与代码规范整顿

## 验收结果

| 验收标准 | 结果 | 说明 |
|----------|------|------|
| 编译通过 | ✅ | `mingw32-make -j4` 零错误，`-Wall -Wextra` 无新增警告 |
| SurvivalScene < 250行 | ✅ | 204行 (从~450行缩减55%) |
| const getter 100% | ✅ | `totalKills()`/`elapsedTime()` 等全部加 const；消除 `const_cast` |
| items() 每帧1次 | ✅ | EnemyManager::update() 内单次扫描，autoAttackNearestEnemy/autoCastSkills 复用 aliveEnemies() |
| unique_ptr 所有权 | ✅ | m_effectManager, m_enemyManager 为 unique_ptr；EffectManager析构安全清理pending特效 |
| 命名空间 | ✅ | config + 3个新模块 均使用 `pixel_hero::survival` |
| 技能策略化 | ✅ | SkillExecutor 基类 + Fireball/Lightning/FrostNova 子类；新增技能=加子类+注册 |
| 魔法数字消除 | ✅ | 10+ 常量提取到 GameConfig.h |

## 改动清单

| 变更 | 文件 | 行数 |
|------|------|------|
| 新增 | `config/GameConfig.h` | 35 |
| 新增 | `EffectManager.h/.cpp` | 85 |
| 新增 | `EnemyManager.h/.cpp` | 81 |
| 新增 | `skills/SkillExecutor.h/.cpp` | 76 |
| 修改 | `SurvivalScene.h/.cpp` | ~204(原450) |
| 修改 | `pixel_hero.pro` | +3 SOURCES, +4 HEADERS, +2 INCLUDEPATH |
| 修改 | `Enemy.cpp/h` | 魔法数字替换 |
| 修改 | `Player.cpp` | 魔法数字替换 |
| 修改 | `WaveManager.h/cpp` | 魔法数字替换 |

## 架构改善对比

| 维度 | 改前 | 改后 |
|------|------|------|
| SurvivalScene 职责数 | 7种 | 3种(协调+升级+HUD) |
| 技能新增成本 | 改C++ if/else | 新增子类+注册 |
| 特效生命周期 | QTimer raw指针 → UAF风险 | QPointer + Scene析构自动清理 |
| 常量管理 | 散落各处魔法数字 | 集中 GameConfig.h |
| items() 扫描/帧 | 3次 | 1次 |

## 推送

`9e0fc9c` → origin/main (18 files, +928/-291)
