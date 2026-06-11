"""
generate_sprites.py — Pixel Hero 像素精灵生成器
SNES 16-bit JRPG 风格, PIL 几何绘制 → PNG RGBA 透明背景

用法: python generate_sprites.py
输出: pixel_hero/resources/png/characters/ enemies/ weapons/
需要: pip install Pillow
"""

import os, math
from PIL import Image, ImageDraw

# ═══════════════════════════════════════════════
# 工具函数
# ═══════════════════════════════════════════════

SCALE = 1  # 输出放大倍数 (NEAREST, 保持像素边缘)

def new_canvas(w, h):
    """创建 RGBA 透明画布"""
    img = Image.new('RGBA', (w, h), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    return img, draw

def finalize(img, scale=SCALE):
    """放大并返回"""
    if scale > 1:
        img = img.resize((img.width * scale, img.height * scale), Image.NEAREST)
    return img

def save_png(img, path):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    img.save(path, 'PNG')
    print(f"  ✓ {os.path.basename(path):20s} {img.size[0]}×{img.size[1]}")

# ═══════════════════════════════════════════════
# 精灵绘制函数
# ═══════════════════════════════════════════════

def draw_slime():
    """32×32 DQ风格蓝色史莱姆"""
    img, d = new_canvas(32, 32)
    body = (0x00, 0x80, 0xFF, 255)
    dark = (0x00, 0x50, 0xAA, 255)
    hl   = (0x66, 0xCC, 0xFF, 255)
    w_eye= (0xFF, 0xFF, 0xFF, 255)
    pupil= (0x00, 0x00, 0x00, 255)

    # 身体(水滴形)
    d.ellipse([5, 6, 27, 29], fill=body)
    d.ellipse([5, 8, 27, 26], fill=dark)  # 底部暗面
    # 顶部高光椭圆
    d.ellipse([11, 8, 21, 15], fill=hl)
    # 眼睛
    d.ellipse([10, 14, 15, 20], fill=w_eye)
    d.ellipse([17, 14, 22, 20], fill=w_eye)
    d.ellipse([12, 16, 14, 18], fill=pupil)
    d.ellipse([19, 16, 21, 18], fill=pupil)
    # 微笑
    d.arc([12, 20, 21, 26], 10, 170, fill=pupil, width=1)
    return finalize(img)

def draw_goblin():
    """48×48 哥布林 — 驼背绿皮+木棒"""
    img, d = new_canvas(48, 48)
    grn = (0x4C, 0xAF, 0x50, 255)
    dgrn= (0x2E, 0x7D, 0x32, 255)
    bwn = (0x8B, 0x45, 0x13, 255)
    eye = (0xFF, 0x00, 0x00, 255)
    club= (0x5D, 0x40, 0x37, 255)

    # 身体(驼背)
    d.ellipse([12, 14, 34, 44], fill=grn)
    d.ellipse([14, 20, 32, 38], fill=dgrn)  # 暗面
    # 头部
    d.ellipse([14, 4, 32, 24], fill=grn)
    # 尖耳
    d.polygon([(14, 8), (6, 2), (18, 6)], fill=grn)
    d.polygon([(32, 8), (40, 2), (28, 6)], fill=grn)
    # 眼睛(红)
    d.ellipse([16, 11, 21, 15], fill=eye)
    d.ellipse([25, 11, 30, 15], fill=eye)
    d.ellipse([18, 12, 20, 14], fill=(0,0,0,255))
    d.ellipse([27, 12, 29, 14], fill=(0,0,0,255))
    # 皮甲
    d.rectangle([13, 22, 33, 34], fill=bwn)
    # 腿
    d.rectangle([16, 38, 21, 47], fill=grn)
    d.rectangle([25, 38, 30, 47], fill=grn)
    d.rectangle([17, 38, 20, 46], fill=dgrn)
    d.rectangle([26, 38, 29, 46], fill=dgrn)
    # 木棒(右手)
    d.line([(33, 26), (42, 20)], fill=club, width=3)
    d.ellipse([41, 16, 45, 22], fill=club)
    return finalize(img, 1)

def draw_skeleton():
    """48×48 骷髅 — 骨白骨骼+铁剑"""
    img, d = new_canvas(48, 48)
    bone = (0xF5, 0xF5, 0xDC, 255)
    dbone= (0xC0, 0xC0, 0xA0, 255)
    hlb  = (0xE0, 0xE0, 0xC0, 255)
    void = (0x00, 0x00, 0x00, 255)
    iron = (0xC0, 0xC0, 0xC0, 255)
    diron= (0x80, 0x80, 0x80, 255)

    # 头骨
    d.ellipse([14, 2, 34, 22], fill=bone)
    d.ellipse([16, 4, 32, 20], fill=hlb)
    # 眼窝
    d.ellipse([17, 9, 23, 15], fill=void)
    d.ellipse([25, 9, 31, 15], fill=void)
    # 鼻洞
    d.ellipse([21, 14, 25, 17], fill=dbone)
    # 牙齿
    for ix in range(18, 30, 3):
        d.line([(ix, 18), (ix, 21)], fill=bone, width=1)

    # 脊椎/肋骨
    d.line([(23, 21), (22, 38)], fill=bone, width=3)
    d.arc([14, 22, 26, 32], -90, 90, fill=bone, width=2)
    d.arc([20, 26, 32, 36], -90, 90, fill=bone, width=2)

    # 手臂
    d.line([(14, 26), (6, 24)], fill=bone, width=3)   # 左臂
    d.line([(24, 24), (38, 20)], fill=bone, width=3)   # 右臂(拿剑)

    # 剑(右手)
    d.line([(34, 18), (42, 8)], fill=iron, width=2)
    d.line([(36, 19), (44, 9)], fill=diron, width=1)
    d.line([(36, 22), (42, 30)], fill=iron, width=2)   # 剑柄
    d.rectangle([34, 20, 38, 23], fill=diron)          # 护手

    # 骨盆
    d.ellipse([16, 34, 30, 42], fill=bone)

    # 腿
    d.line([(19, 40), (16, 47)], fill=bone, width=3)
    d.line([(27, 40), (30, 47)], fill=bone, width=3)

    return finalize(img, 1)

def draw_bat():
    """48×48 蝙蝠 — 深紫+红眼"""
    img, d = new_canvas(48, 48)
    body= (0x4A, 0x14, 0x8C, 255)
    wing= (0x7B, 0x1F, 0xA2, 255)
    red = (0xFF, 0x00, 0x00, 255)
    bone= (0x31, 0x1B, 0x92, 255)

    # 左翼
    d.polygon([(4, 16), (24, 6), (4, 28)], fill=wing)
    d.polygon([(4, 20), (20, 12), (4, 24)], fill=body)
    # 翼骨
    d.line([(22, 12), (4, 16)], fill=bone, width=1)
    d.line([(20, 14), (4, 22)], fill=bone, width=1)

    # 右翼
    d.polygon([(44, 16), (24, 6), (44, 28)], fill=wing)
    d.polygon([(44, 20), (28, 12), (44, 24)], fill=body)
    d.line([(26, 12), (44, 16)], fill=bone, width=1)
    d.line([(28, 14), (44, 22)], fill=bone, width=1)

    # 身体
    d.ellipse([16, 16, 32, 34], fill=body)
    d.ellipse([18, 18, 30, 32], fill=wing)

    # 红眼
    d.ellipse([19, 19, 23, 23], fill=red)
    d.ellipse([25, 19, 29, 23], fill=red)
    d.point((21, 21), fill=(255, 255, 200, 255))
    d.point((27, 21), fill=(255, 255, 200, 255))

    # 尖耳
    d.polygon([(17, 16), (11, 8), (19, 14)], fill=body)
    d.polygon([(31, 16), (37, 8), (29, 14)], fill=body)

    # 尖牙
    for b in [(22, 28), (26, 28)]:
        d.line([b, (b[0], b[1]+3)], fill=(0xFF,0xFF,0xFF,255), width=1)

    return finalize(img, 1)

def draw_goblin_elite():
    """64×64 精英哥布林 — 红皮+金属甲+大斧"""
    img, d = new_canvas(64, 64)
    red = (0xD3, 0x2F, 0x2F, 255)
    dred= (0xB7, 0x1C, 0x1C, 255)
    mtl = (0x75, 0x75, 0x75, 255)
    gold= (0xFF, 0xD7, 0x00, 255)
    hmtl= (0x90, 0x90, 0x90, 255)
    eye = (0xFF, 0x44, 0x00, 255)

    # 身体(壮硕)
    d.ellipse([14, 18, 48, 56], fill=red)
    d.ellipse([18, 24, 44, 52], fill=dred)
    # 头部
    d.ellipse([18, 4, 44, 28], fill=red)
    # 耳
    d.polygon([(16, 10), (4, 2), (22, 6)], fill=red)
    d.polygon([(46, 10), (58, 2), (40, 6)], fill=red)
    # 眼
    d.ellipse([22, 13, 29, 19], fill=eye)
    d.ellipse([33, 13, 40, 19], fill=eye)
    d.ellipse([24, 14, 27, 17], fill=(0,0,0,255))
    d.ellipse([35, 14, 38, 17], fill=(0,0,0,255))
    # 肩甲(金属金色)
    d.rectangle([10, 20, 18, 34], fill=mtl)
    d.rectangle([44, 20, 52, 34], fill=mtl)
    d.rectangle([11, 22, 17, 33], fill=hmtl)
    d.rectangle([45, 22, 51, 33], fill=hmtl)
    # 金边
    d.line([(10, 20), (18, 20)], fill=gold, width=2)
    d.line([(44, 20), (52, 20)], fill=gold, width=2)
    # 腿
    d.rectangle([18, 50, 25, 63], fill=red)
    d.rectangle([37, 50, 44, 63], fill=red)
    d.rectangle([19, 50, 24, 62], fill=dred)
    d.rectangle([38, 50, 43, 62], fill=dred)
    # 大斧(右手)
    d.line([(48, 30), (62, 20)], fill=(0x4A,0x4A,0x4A,255), width=3)  # 柄
    d.polygon([(58, 12), (64, 8), (62, 22), (56, 18)], fill=mtl)
    d.polygon([(59, 13), (63, 10), (61, 20)], fill=hmtl)  # 刃高光

    return finalize(img, 1)

def draw_dragon():
    """128×128 龙Boss — 双足站立的黑色巨龙, 翅膀, 红眼, 口吐火焰"""
    img, d = new_canvas(128, 128)
    scl  = (0x37, 0x47, 0x4F, 255)  # 暗鳞
    scl2 = (0x54, 0x6E, 0x7A, 255)  # 亮鳞
    dscl = (0x26, 0x32, 0x38, 255)  # 最暗
    belly= (0xFF, 0xCC, 0x80, 255)  # 腹甲
    reye = (0xFF, 0x57, 0x22, 255)  # 红眼
    fire = (0xFF, 0x98, 0x00, 255)  # 火橙
    firc = (0xFF, 0xEB, 0x3B, 255)  # 火心
    claw = (0xBB, 0xBB, 0xBB, 255)  # 利爪
    wingm= (0x78, 0x90, 0x9C, 255)  # 翼膜

    # ── 身体 (双足站立, 巨大躯干) ──
    d.ellipse([28, 24, 98, 80], fill=scl)
    d.ellipse([32, 30, 94, 76], fill=scl2)
    d.ellipse([36, 46, 90, 72], fill=belly)  # 腹甲

    # ── 头部 ──
    d.ellipse([70, 4, 114, 40], fill=scl)
    d.ellipse([74, 8, 110, 36], fill=scl2)
    # 下颚
    d.ellipse([74, 18, 112, 44], fill=dscl)

    # 角
    d.polygon([(80, 6), (72, -2), (84, 4)], fill=dscl)
    d.polygon([(94, 4), (100, -4), (100, 6)], fill=dscl)

    # 红眼(发光)
    d.ellipse([78, 18, 86, 25], fill=reye)
    d.ellipse([94, 18, 102, 25], fill=reye)
    d.ellipse([80, 20, 84, 23], fill=firc)  # 高光
    d.ellipse([96, 20, 100, 23], fill=firc)

    # 鼻孔(火光)
    d.ellipse([104, 14, 108, 17], fill=firc)
    d.ellipse([106, 8, 110, 13], fill=fire)

    # ── 口吐火焰 ──
    d.ellipse([108, 12, 126, 22], fill=fire)
    d.ellipse([112, 16, 124, 20], fill=firc)
    d.polygon([(120, 8), (130, 4), (124, 16)], fill=fire)
    d.polygon([(118, 22), (132, 28), (124, 20)], fill=fire)

    # ── 左翼(展开) ──
    d.polygon([(4, 16), (30, 10), (14, 36), (26, 48), (14, 54)], fill=wingm)
    d.line([(30, 14), (4, 16)], fill=dscl, width=2)
    d.line([(28, 26), (14, 36)], fill=dscl, width=2)
    d.line([(28, 40), (26, 48)], fill=dscl, width=2)

    # ── 右翼(展开) ──
    d.polygon([(124, 16), (98, 10), (114, 36), (102, 48), (114, 54)], fill=wingm)
    d.line([(98, 14), (124, 16)], fill=dscl, width=2)
    d.line([(100, 26), (114, 36)], fill=dscl, width=2)
    d.line([(100, 40), (102, 48)], fill=dscl, width=2)

    # ── 腿(粗壮) ──
    # 左腿
    d.rectangle([38, 76, 50, 98], fill=scl)
    d.rectangle([40, 78, 48, 96], fill=scl2)
    # 左爪
    d.ellipse([34, 96, 54, 104], fill=claw)
    for cx in [38, 44, 50]:
        d.line([(cx, 100), (cx-2, 108)], fill=claw, width=2)

    # 右腿
    d.rectangle([70, 76, 82, 98], fill=scl)
    d.rectangle([72, 78, 80, 96], fill=scl2)
    # 右爪
    d.ellipse([66, 96, 86, 104], fill=claw)
    for cx in [70, 76, 82]:
        d.line([(cx, 100), (cx+2, 108)], fill=claw, width=2)

    # ── 尾巴 ──
    d.line([(36, 60), (12, 74)], fill=scl, width=5)
    d.line([(12, 74), (0, 68)], fill=scl, width=4)
    # 尾尖(刺)
    d.polygon([(0, 66), (-4, 64), (0, 70)], fill=claw)

    # ── 背刺(脊) ──
    for i, (x, y) in enumerate([(50, 20), (46, 28), (42, 36), (38, 46), (36, 56)]):
        d.polygon([(x, y), (x-6, y-6), (x+2, y-2)], fill=dscl)

    return finalize(img, 1)


# ─── 角色 ───

def draw_warrior():
    """48×48 战士 — 蓝银甲+剑盾+头盔"""
    img, d = new_canvas(48, 48)
    arm = (0x4A, 0x90, 0xA4, 255)
    dark= (0x33, 0x68, 0x7A, 255)
    hll = (0x80, 0xC8, 0xD8, 255)
    gld = (0xFF, 0xD7, 0x00, 255)
    bld = (0xC0, 0xC0, 0xC0, 255)
    bld2= (0xCC, 0xCC, 0xCC, 255)
    skn = (0x8B, 0x6B, 0x4A, 255)
    shd = (0x66, 0xAA, 0xBB, 255)

    # 身体(铠甲)
    d.ellipse([12, 14, 34, 36], fill=arm)
    d.ellipse([14, 16, 32, 34], fill=hll)
    d.rectangle([18, 20, 28, 30], fill=dark)  # 胸甲纹

    # 头盔
    d.ellipse([13, 2, 33, 18], fill=arm)
    d.ellipse([15, 4, 31, 16], fill=hll)
    d.rectangle([18, 6, 28, 8], fill=gld)  # 头盔金带

    # 脸部(皮肤)
    d.ellipse([16, 8, 28, 17], fill=skn)
    # 眼睛
    d.point((20, 12), fill=(0,0,0,255))
    d.point((26, 12), fill=(0,0,0,255))

    # 左臂+盾
    d.line([(12, 20), (2, 22)], fill=arm, width=4)  # 臂
    d.ellipse([0, 16, 14, 30], fill=shd)             # 盾
    d.ellipse([2, 18, 12, 28], fill=(0x88,0xCC,0xDD,255))  # 盾高光
    d.line([(7, 20), (7, 26)], fill=gld, width=1)    # 盾纹

    # 右臂+剑
    d.line([(34, 18), (42, 14)], fill=arm, width=4)
    d.line([(42, 10), (44, 32)], fill=bld, width=2)   # 剑刃
    d.line([(43, 12), (45, 30)], fill=bld2, width=1)   # 剑刃高光
    d.rectangle([40, 14, 44, 17], fill=gld)            # 护手
    d.line([(41, 17), (43, 20)], fill=dark, width=2)   # 剑柄

    # 腿
    d.rectangle([16, 34, 21, 47], fill=arm)
    d.rectangle([25, 34, 30, 47], fill=arm)
    d.rectangle([17, 34, 20, 46], fill=dark)
    d.rectangle([26, 34, 29, 46], fill=dark)
    # 靴
    d.rectangle([15, 44, 22, 47], fill=dark)
    d.rectangle([24, 44, 31, 47], fill=dark)

    return finalize(img, 1)

def draw_archer():
    """48×48 弓箭手 — 绿斗篷+皮甲+弓"""
    img, d = new_canvas(48, 48)
    grn = (0x22, 0x8B, 0x22, 255)
    dgrn= (0x1B, 0x6B, 0x1B, 255)
    skn = (0x8B, 0x6B, 0x4A, 255)
    lgrn= (0x55, 0xBB, 0x55, 255)
    bow = (0xA0, 0x6B, 0x35, 255)
    strg= (0xCC, 0xCC, 0xCC, 255)

    # 身体
    d.ellipse([14, 16, 34, 38], fill=grn)
    d.ellipse([16, 18, 32, 36], fill=lgrn)
    # 交叉皮带
    d.line([(16, 22), (32, 32)], fill=dgrn, width=2)
    d.line([(32, 22), (16, 32)], fill=dgrn, width=2)

    # 头部
    d.ellipse([15, 4, 33, 20], fill=grn)
    # 兜帽
    d.pieslice([13, 2, 35, 18], 180, 360, fill=grn)
    d.arc([14, 4, 32, 16], 180, 360, fill=dgrn, width=2)
    # 脸
    d.ellipse([17, 10, 29, 19], fill=skn)
    d.point((21, 14), fill=(0,0,0,255))
    d.point((27, 14), fill=(0,0,0,255))

    # 箭袋(背上)
    d.rectangle([32, 14, 38, 28], fill=(0x6B, 0x42, 0x2A, 255))
    d.line([(34, 14), (34, 8)], fill=strg, width=1)   # 箭羽
    d.line([(36, 14), (36, 8)], fill=strg, width=1)

    # 弓(左手)
    d.arc([0, 8, 22, 36], 290, 70, fill=bow, width=3)
    # 弦
    d.line([(10, 8), (10, 36)], fill=strg, width=1)

    # 右臂(拉弓)
    d.line([(32, 20), (10, 22)], fill=grn, width=4)
    d.ellipse([8, 20, 14, 25], fill=skn)  # 手

    # 腿
    d.rectangle([16, 36, 22, 47], fill=grn)
    d.rectangle([26, 36, 32, 47], fill=grn)
    d.rectangle([17, 36, 21, 46], fill=dgrn)
    d.rectangle([27, 36, 31, 46], fill=dgrn)
    # 靴
    d.rectangle([15, 44, 23, 47], fill=dgrn)
    d.rectangle([25, 44, 33, 47], fill=dgrn)

    return finalize(img, 1)

def draw_mage():
    """48×48 法师 — 紫袍+尖帽+法杖"""
    img, d = new_canvas(48, 48)
    pur = (0x99, 0x32, 0xCC, 255)
    dpur= (0x7B, 0x1F, 0xA2, 255)
    lp  = (0xBB, 0x77, 0xFF, 255)
    skn = (0x8B, 0x6B, 0x4A, 255)
    orb = (0xFF, 0xD7, 0x00, 255)
    stk = (0x8B, 0x45, 0x13, 255)

    # 身体(长袍)
    d.polygon([(12, 22), (34, 22), (36, 46), (10, 46)], fill=pur)
    d.polygon([(14, 24), (32, 24), (33, 44), (13, 44)], fill=lp)

    # 头部
    d.ellipse([14, 10, 32, 24], fill=skn)
    # 尖帽
    d.polygon([(12, 12), (34, 12), (24, -2)], fill=pur)
    d.polygon([(14, 11), (32, 11), (24, 0)], fill=lp)
    # 帽子卷边
    d.ellipse([12, 8, 34, 16], fill=dpur)
    # 眼睛
    d.point((20, 16), fill=(0,0,0,255))
    d.point((28, 16), fill=(0,0,0,255))
    # 胡子(白)
    d.ellipse([16, 20, 30, 26], fill=(0xFF,0xFF,0xFF,200))
    d.arc([18, 20, 28, 26], 10, 170, fill=(0xCC,0xCC,0xCC,200), width=1)

    # 法杖(左手)
    d.line([(4, 18), (10, 46)], fill=stk, width=3)   # 杖身
    # 宝珠
    d.ellipse([0, 10, 12, 22], fill=orb)
    d.ellipse([2, 12, 10, 20], fill=(0xFF,0xFF,0x80,255))
    d.point((4, 14), fill=(0xFF,0xFF,0xFF,255))  # 高光

    # 右臂
    d.line([(32, 24), (40, 28)], fill=pur, width=4)
    d.ellipse([38, 26, 43, 31], fill=skn)  # 手

    # 袍底
    d.ellipse([10, 44, 36, 47], fill=dpur)

    return finalize(img, 1)


# ─── 武器图标 ───

def draw_short_sword():
    """32×32 短剑"""
    img, d = new_canvas(32, 32)
    bld = (0xC0, 0xC0, 0xC0, 255)
    dk  = (0x80, 0x80, 0x80, 255)
    gld = (0xFF, 0xD7, 0x00, 255)
    brn = (0x8B, 0x6B, 0x4A, 255)
    hl  = (0xFF, 0xFF, 0xFF, 255)

    # 剑刃
    d.line([(16, 2), (16, 22)], fill=bld, width=3)
    d.line([(17, 3), (17, 21)], fill=hl, width=1)  # 高光
    d.point((16, 2), fill=bld)

    # 护手
    d.line([(10, 20), (22, 20)], fill=gld, width=3)

    # 剑柄
    d.line([(16, 22), (16, 28)], fill=brn, width=2)
    d.rectangle([15, 24, 17, 26], fill=dk)

    # 柄尾
    d.ellipse([14, 28, 18, 31], fill=gld)

    return finalize(img, 1)

def draw_long_sword():
    """32×32 长剑"""
    img, d = new_canvas(32, 32)
    bld = (0xC0, 0xC0, 0xC0, 255)
    gld = (0xFF, 0xD7, 0x00, 255)
    brn = (0x8B, 0x4A, 0x30, 255)
    hl  = (0xFF, 0xFF, 0xFF, 255)

    # 长剑刃(长)
    d.line([(16, 0), (16, 20)], fill=bld, width=4)
    d.line([(17, 2), (17, 19)], fill=hl, width=2)
    # 剑尖
    d.polygon([(16, 0), (14, 3), (18, 3)], fill=bld)

    # 大护手
    d.line([(8, 18), (24, 22)], fill=gld, width=4)
    d.line([(9, 19), (23, 21)], fill=(0xFF,0xFF,0x80,255), width=1)

    # 剑柄
    d.line([(16, 22), (16, 29)], fill=brn, width=3)
    # 缠绕纹
    for y in range(23, 29, 2):
        d.line([(14, y), (18, y)], fill=gld, width=1)

    # 柄尾宝珠
    d.ellipse([13, 28, 19, 31], fill=gld)

    return finalize(img, 1)

def draw_staff():
    """32×32 法杖"""
    img, d = new_canvas(32, 32)
    orb = (0xFF, 0xD7, 0x00, 255)
    ohl = (0xFF, 0xFF, 0x80, 255)
    stk = (0x8B, 0x45, 0x13, 255)
    dstk= (0x6B, 0x35, 0x08, 255)
    pur = (0x99, 0x32, 0xCC, 255)
    glow= (0xBB, 0x77, 0xFF, 120)

    # 光晕
    d.ellipse([4, 0, 20, 16], fill=glow)

    # 宝珠
    d.ellipse([8, 4, 16, 12], fill=orb)
    d.ellipse([10, 5, 14, 10], fill=ohl)
    d.point((11, 6), fill=(0xFF,0xFF,0xFF,255))

    # 杖身
    d.line([(12, 12), (12, 30)], fill=stk, width=2)
    d.line([(13, 12), (13, 30)], fill=dstk, width=1)

    # 杖环
    d.rectangle([10, 20, 14, 22], fill=pur)
    d.rectangle([10, 26, 14, 28], fill=pur)

    return finalize(img, 1)

def draw_dagger():
    """32×32 匕首"""
    img, d = new_canvas(32, 32)
    bld = (0x00, 0xFF, 0x7F, 255)  # 绿毒刃
    dkl = (0x00, 0xAA, 0x55, 255)
    brn = (0x8B, 0x50, 0x30, 255)
    gem = (0xFF, 0x00, 0x00, 255)  # 红宝石

    # 弯刃
    d.line([(6, 4), (20, 8)], fill=bld, width=4)
    d.line([(7, 5), (19, 7)], fill=(0x66,0xFF,0xAA,255), width=1)
    # 刃尖
    d.polygon([(6, 4), (2, 2), (8, 3)], fill=bld)

    # 柄
    d.line([(20, 8), (28, 14)], fill=brn, width=3)
    d.line([(20, 10), (26, 16)], fill=dkl, width=2)

    # 护手
    d.line([(18, 5), (22, 12)], fill=(0xC0,0xC0,0xC0,255), width=2)

    # 红宝石(柄尾)
    d.ellipse([24, 12, 30, 18], fill=gem)
    d.point((26, 14), fill=(0xFF,0xFF,0xFF,200))

    return finalize(img, 1)


# ═══════════════════════════════════════════════
# 主流程
# ═══════════════════════════════════════════════

def main():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    base = os.path.join(script_dir, '..', 'resources', 'png')

    enemies = os.path.join(base, 'enemies')
    chars   = os.path.join(base, 'characters')
    weapons = os.path.join(base, 'weapons')

    print("Generating sprites...")

    print("\n[Enemies]")
    save_png(draw_slime(),           os.path.join(enemies, 'slime.png'))
    save_png(draw_goblin(),          os.path.join(enemies, 'goblin.png'))
    save_png(draw_skeleton(),        os.path.join(enemies, 'skeleton.png'))
    save_png(draw_bat(),             os.path.join(enemies, 'bat.png'))
    save_png(draw_goblin_elite(),    os.path.join(enemies, 'goblin_elite.png'))
    save_png(draw_dragon(),          os.path.join(enemies, 'dragon.png'))

    print("\n[Characters]")
    save_png(draw_warrior(),  os.path.join(chars, 'warrior.png'))
    save_png(draw_archer(),   os.path.join(chars, 'archer.png'))
    save_png(draw_mage(),     os.path.join(chars, 'mage.png'))

    print("\n[Weapons]")
    save_png(draw_short_sword(), os.path.join(weapons, 'short_sword.png'))
    save_png(draw_long_sword(),  os.path.join(weapons, 'long_sword.png'))
    save_png(draw_staff(),       os.path.join(weapons, 'staff.png'))
    save_png(draw_dagger(),      os.path.join(weapons, 'dagger.png'))

    print(f"\n✓ Done! {6+3+4} sprites generated.")

if __name__ == '__main__':
    main()
