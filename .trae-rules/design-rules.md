# 像素勇者 — 设计规范

## 一、模块职责边界

### 1.1 SurvivalScene（协调层，≤250行）

**仅负责**：游戏流程协调、HUD更新、升级UI连接

**禁止**：
- 直接管理敌人生命周期（→ EnemyManager）
- 直接创建战斗特效（→ EffectManager）
- 硬编码技能逻辑（→ SkillExecutor 子类）
- 直接调用 `items()` 超过1次/帧

### 1.2 子模块职责

| 模块 | 位置 | 职责 | 行数上限 |
|------|------|------|----------|
| `EnemyManager` | `survival/EnemyManager.h/.cpp` | 敌人扫描、AI更新、死亡清理 | ≤120 |
| `EffectManager` | `survival/EffectManager.h/.cpp` | 战斗特效创建与自动清理 | ≤100 |
| `SkillExecutor` | `survival/skills/SkillExecutor.h/.cpp` | 技能策略基类 + 子类 | 按需 |
| `WaveManager` | `survival/WaveManager.h/.cpp` | 波次调度、生成计时 | ≤150 |

### 1.3 新增技能流程

1. 在 `skills/SkillExecutor.h` 新增子类，实现 `execute()`
2. 在 `SkillExecutor::create()` 工厂方法中注册 `skillId`
3. 在 `skills.json` 中添加技能数据

**禁止**：在 SurvivalScene 的 `autoCastSkills()` 中写 if/else。

---

## 二、所有权与生命周期

### 2.1 规则

| 对象类型 | 所有权 | 生命周期管理 |
|----------|--------|-------------|
| Scene 直接管理的子模块 | `std::unique_ptr` | Scene 析构自动释放 |
| QGraphicsItem（Player/Enemy/特效） | QGraphicsScene | `scene->removeItem()` + `delete` |
| QObject 子模块（Q_OBJECT） | QObject parent | Qt 父子树自动管理 |

### 2.2 禁止

- ❌ 裸指针跨模块传递所有权（必须明确谁 owner）
- ❌ QTimer::singleShot 捕获 raw `this` 用于跨生命周期回调
- ✅ 用 `QPointer<T>` 弱引用（EffectManager 的 m_scene）
- ✅ 用 QObject parent 参数传 `this`（Timer 回调）

---

## 三、性能约束

### 3.1 QGraphicsScene::items() 扫描

- **硬约束**：每帧最多调用 1 次
- **实现**：EnemyManager::update() 内扫描一次，结果传递给 `updateAI()`、`cleanupDead()`

### 3.2 敌人数量

- 同时存活上限：`config::MAX_ALIVE_ENEMIES` = 30
- `WaveManager::spawnEnemy()` 必须检查上限

### 3.3 帧率

- 固定 16ms/帧（~60fps），`config::GAME_TICK_MS`

---

## 四、常量化规则

### 4.1 必须提取到 `config/GameConfig.h`

满足以下任一条件，禁止硬编码：
- 在整个项目中出现 2 次以上
- 会影响游戏平衡的可调参数（HP、伤害、经验）
- 场景尺寸相关（800、600、边距）

### 4.2 命名约定

```cpp
namespace pixel_hero::config {
    constexpr float SCREEN_W = 800.0f;        // 大写_下划线
    constexpr int   MAX_ALIVE_ENEMIES = 30;
}
```

---

## 五、命名空间规则

### 5.1 新模块必须使用命名空间

```
pixel_hero::config::      // GameConfig.h
pixel_hero::survival::    // EffectManager, EnemyManager, SkillExecutor
pixel_hero::survival::skills::  // 预留
```

### 5.2 全局命名空间引用

在命名空间内部引用外层类型时，使用 `using ::` 声明：

```cpp
namespace pixel_hero::survival {
    using ::Enemy;
    using ::SurvivalPlayer;
    // 不污染全局，仅命名空间内可见
}
```

---

## 六、const 正确性

- 所有 getter 方法加 `const`：`int health() const;`
- 不修改对象状态的查询方法加 `const`
- 禁止 `const_cast`（消除已在重构中完成）
- 从 const 方法获取 mutable 容器用 `QList<T> aliveEnemies() const;`（返回拷贝）

---

## 七、特效系统规范

### 7.1 特效创建

- 统一通过 `EffectManager` 调用
- 特效 item 用 `QGraphicsScene::addEllipse/addPath` 创建
- 特效 item 通过 `QTimer::singleShot` 定时销毁，**Timer 的 parent 必须传 EffectManager 的 this**

### 7.2 特效销毁安全

```cpp
// ✅ 正确：EffectManager 作为 QObject parent，析构时自动取消 pending Timer
QTimer::singleShot(150, this, [scene, item]() {
    if (scene) { scene->removeItem(item); delete item; }
});

// ❌ 错误：捕获 raw this 到非 QObject 或被 delete 的对象
QTimer::singleShot(150, [this]() { ... });
```

---

## 八、目录结构（更新后）

```
pixel_hero/src/
├── config/              # 集中常量
│   └── GameConfig.h
├── entities/            # 实体基类
├── survival/            # 生存模式
│   ├── skills/          # 技能策略子类
│   ├── SurvivalScene.*  # 协调层
│   ├── EffectManager.*  # 特效
│   ├── EnemyManager.*   # 敌人管理
│   └── ...
├── ui/                  # UI 组件
└── utils/               # 工具
```

---

## 九、反模式（禁止清单）

| 反模式 | 说明 |
|--------|------|
| GOD 对象 | 单个类超过 250 行 + 5 种职责 → 拆分 |
| if/else 技能链 | 技能逻辑不能用 if/else — 用策略模式 |
| QTimer::singleShot + raw this | UAF 隐患 — 用 QPointer 或 QObject parent |
| 魔法数字 | 2次以上出现的数字必须提取常量 |
| 重复 items() 扫描 | 每帧1次，结果复用 |
| 裸 new + 无 owner | 必须明确 unique_ptr 或 QObject parent |
