# ALIGNMENT — 架构与代码规范整顿

## 原始需求

"规范设计规则" — 系统性解决当前项目存在的设计问题，建立可长期维护的代码规范基础。

## 项目上下文

| 维度 | 现状 |
|------|------|
| 项目性质 | 学习/实验项目，单人开发 |
| 技术栈 | C++17 + Qt 6.11.1(QGraphicsScene框架) |
| 代码规模 | 19个.h + 19个.cpp，~3500行 |
| 架构 | 单Scene多层：Menu → CharacterSelectUI → SurvivalScene(核心) |
| 数据 | JSON驱动(技能/武器/波次)，QRC资源系统 |
| 构建 | QMake + MinGW-w64，手动make |

## 问题清单

### P0 - 安全隐患
1. **裸指针无所有权**：WaveManager/SurvivalPlayer/UpgradeUI 均裸指针传递，无明确owner
2. **特效Timer与Scene生命周期不同步**：showXxxEffect 中 QTimer::singleShot 捕获 this(Scene)，Scene先destroy则UAF

### P1 - 架构腐化
3. **SurvivalScene GOD对象**：~450行，包含游戏循环/清理/攻击/技能/波次/升级/特效7种职责
4. **技能硬编码**：if/else串，加新技能=改C++重编译
5. **敌人数据双源**：C++构造器hardcode + EnemyFactory JSON load，互不关联

### P2 - 代码规范
6. **const缺失**：getter不加const；aliveEnemies()内部const_cast
7. **QGraphicsScene::items()重复扫描**：每帧扫描3次全场景
8. **魔法数字**：800/600边界、经验表、冷却值散落各处
9. **无命名空间**：所有类型裸放在全局

## 边界确认

- 范围：P0+P1+P2 全部解决
- 不改：JSON数据文件格式、QRC资源、精灵图、UI布局
- 不改：游戏性数值(怪物属性、技能伤害等)
- 允许：拆分文件、新增类、修改include关系

## 疑问澄清

1. **P0-特效Timer**: 当前用QTimer::singleShot + raw指针。改用smart guard(shared_ptr+weak_ptr)还是直接在Scene析构时取消所有pending特效？
   → 决策：特效统一由Scene管理生命周期，Scene析构时自动清理。创建一个EffectManager管理所有临时特效item，Scene持有其所有权。

2. **P1-技能系统**: 是否要支持热加载(不改C++就能加技能)？还是仅解决数据驱动问题？
   → 当前技能(火球/闪电/冰霜)逻辑够用，先做数据驱动化(消除if/else)，未来加技能改JSON即可，不需要完全热加载。

3. **P2-命名空间**: 使用什么结构？
   → 使用 `pixel_hero::` 根命名空间，子命名空间 `survival/`、`entities/`、`ui/`、`utils/` 对应目录结构。
