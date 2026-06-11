# 像素美术升级 — CONSENSUS 共识文档

**项目**: 像素勇者 (Pixel Hero Adventure)
**文档版本**: CONSENSUS V1.0
**日期**: 2026-06-11
**阶段**: 阶段1 — 对齐阶段 (Align)
**前置文档**: [ALIGNMENT_像素美术升级.md](./ALIGNMENT_像素美术升级.md)

---

## 一、最终需求

### 1.1 一句话需求

使用 Python PIL 程序化生成 SNES 16-bit JRPG 风格的像素精灵图(PNG)，替换游戏中所有纯色方块占位符。

### 1.2 验收标准

| # | 标准 | 验收方式 | 优先级 |
|:--:|------|---------|:------:|
| AC1 | 6种敌人各有独立像素精灵图 | 游戏中可见不同外观 | P0 |
| AC2 | 3种角色各有独立像素精灵图 | 角色选择界面 + 游戏中可见 | P0 |
| AC3 | 4种武器各有独立像素图标 | 武器选择界面可见 | P0 |
| AC4 | PNG 透明背景，无白边/黑边 | 叠在深色背景上无异常 | P0 |
| AC5 | 精灵风格统一为 SNES 16-bit | 视觉一致，无风格断裂 | P1 |
| AC6 | 精灵图尺寸符合规范 (角色48px, 普通怪32-48px, Boss 96px) | 游戏中尺寸合理 | P1 |
| AC7 | 所有精灵图由一个 generate_sprites.py 脚本生成 | 运行脚本→产出所有PNG | P0 |
| AC8 | 战斗中敌人精灵与配置数据对应 | slime 不是 goblin 的图 | P0 |
| AC9 | 编译通过，QRC 加载 PNG 资源 | mingw32-make 零错误 | P0 |

---

## 二、技术实现方案

### 2.1 架构方案

```
generate_sprites.py (Python + PIL)
  │
  ├── 逐像素绘制: 角色/敌人/武器
  │     ├── 定义调色板 → 定义像素矩阵 → 渲染 PNG
  │     └── 每个精灵一个绘制函数: draw_slime(), draw_goblin(), ...
  │
  └── 输出: pixel_hero/resources/png/
        ├── characters/  (warrior.png, archer.png, mage.png)
        ├── enemies/     (slime.png, goblin.png, skeleton.png, bat.png, goblin_elite.png, dragon.png)
        └── weapons/     (short_sword.png, long_sword.png, staff.png, dagger.png)

pixel_hero/resources.qrc (Qt资源文件)
  └── 添加 PNG 资源路径 → 编译时打包

C++ 代码集成:
  ├── SurvivalPlayer::applyCharacter() → setPixmap(QPixmap(":/...png"))
  ├── EnemyFactory::createEnemy()      → setPixmap(QPixmap(":/...png"))
  ├── CharacterSelectUI::paint()       → painter->drawPixmap(avatar, QPixmap(":/...png"))
  └── WeaponSelectUI::paint()          → painter->drawPixmap(icon, QPixmap(":/...png"))
```

### 2.2 PIL 绘制技术方案

```python
from PIL import Image, ImageDraw

def draw_character_pixel_by_pixel(pixel_matrix, palette, scale=1):
    """将像素矩阵渲染为 RGBA PNG"""
    img = Image.new('RGBA', (W, H), (0,0,0,0))
    for y, row in enumerate(pixel_matrix):
        for x, px in enumerate(row):
            if px >= 0:  # -1 = 透明
                img.putpixel((x, y), palette[px])
    if scale > 1:
        img = img.resize((W*scale, H*scale), Image.NEAREST)
    return img
```

**关键约束**:
- 颜色索引表示: `palette[0]`=主色, `palette[1]`=暗面, `palette[2]`=高光, `-1`=透明
- `Image.NEAREST` 放大保持像素锐利边缘
- 输出 PNG RGBA 透明背景

### 2.3 QRC 资源集成

```xml
<qresource prefix="/sprites">
    <file alias="slime">resources/png/enemies/slime.png</file>
    <file alias="goblin">resources/png/enemies/goblin.png</file>
    <!-- ... -->
</qresource>
```

C++ 使用: `QPixmap(":/sprites/slime")`

---

## 三、精灵规格明细

### 3.1 角色精灵 (48×48 → 3个)

| 角色 | 主色调 | 关键特征 |
|------|--------|---------|
| 战士 warrior | `#4A90A4` 蓝银 | 头盔+肩甲+剑盾 |
| 弓箭手 archer | `#228B22` 绿 | 皮甲+兜帽+弓 |
| 法师 mage | `#9932CC` 紫 | 长袍+尖帽+法杖 |

### 3.2 敌人精灵

| 敌人 | 尺寸 | 主色调 | 关键特征 |
|------|------|--------|---------|
| 史莱姆 slime | 32×32 | `#0080FF` 蓝 | 水滴形+微笑+高光 |
| 哥布林 goblin | 48×48 | `#4CAF50` 绿 | 驼背+尖耳+木棒 |
| 骷髅 skeleton | 48×48 | `#F5F5DC` 骨白 | 骨骼+空洞眼窝+骨剑 |
| 蝙蝠 bat | 32×48 | `#4A148C` 深紫 | 展开翼+红眼 |
| 精英哥布林 goblin_elite | 64×64 | `#D32F2F` 红 | 壮硕+金属肩甲+大斧 |
| 龙 dragon | 128×128 | `#37474F` 暗灰 | 双足站立+翅膀+红眼 |

### 3.3 武器图标 (32×32 → 4个)

| 武器 | 颜色 | 特征 |
|------|------|------|
| 短剑 short_sword | `#C0C0C0` 银白 | 短直剑 |
| 长剑 long_sword | `#FFD700` 金 | 长剑+交叉护手 |
| 法杖 staff | `#9932CC` 紫 | 长杖+宝珠 |
| 匕首 dagger | `#00FF7F` 绿 | 小弯刀 |

---

## 四、任务边界限制

| 包含 | 不包含 |
|------|--------|
| ✅ 一个 `generate_sprites.py` 脚本 | ❌ 多个分散脚本 |
| ✅ 13张 PNG 精灵图 | ❌ 动画帧序列 |
| ✅ QRC 资源集成 | ❌ 音效/BGM |
| ✅ C++ 代码中 QPixmap 替换 setPixmap | ❌ 背景/地图/场景 |
| ✅ 编译验证 | ❌ 特效粒子 |
| ❌ 游戏内手动绘制(保持 PIL 生成) | |

---

## 五、技术约束

| 约束 | 值 |
|------|-----|
| Python 版本 | 3.x + Pillow |
| PNG 格式 | RGBA 8-bit 透明 |
| 精灵放大方式 | NEAREST (无抗锯齿) |
| Qt 资源前缀 | `:/sprites/` |
| 代码改动范围 | EnemyFactory, SurvivalPlayer, CharacterSelectUI, WeaponSelectUI, resources.qrc |

---

## 六、所有决策已确认

| # | 决策 | 结论 |
|:--:|------|------|
| D1 | 生成方案 | Python PIL 程序化绘制 ✅ |
| D2 | 美术风格 | SNES 16-bit JRPG (DQ/FF IV-VI) ✅ |
| D3 | 精灵尺寸 | 角色48px, 普通怪32-48px, Boss 96-128px ✅ |
| D4 | 朝向 | 仅正面(top-down俯视) ✅ |
| D5 | 脚本数量 | 一个 generate_sprites.py ✅ |
| D6 | 图片格式 | PNG 透明背景 ✅ |

---

**文档状态**: ✅ CONSENSUS 完成 — 所有决策点已确认，可进入阶段2(Architect)
