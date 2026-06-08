#!/usr/bin/env python3
"""
像素精灵图片生成器
纯Python实现，无需外部依赖，使用struct+zlib生成PNG文件
"""
import struct
import zlib
import os

BASE_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "pixel_hero")

def create_png(width, height, pixels):
    """创建PNG文件，pixels为列表，每个元素为(R,G,B,A)"""
    def make_chunk(chunk_type, data):
        chunk = chunk_type + data
        return struct.pack('>I', len(data)) + chunk + struct.pack('>I', zlib.crc32(chunk) & 0xffffffff)

    header = b'\x89PNG\r\n\x1a\n'
    ihdr_data = struct.pack('>IIBBBBB', width, height, 8, 6, 0, 0, 0)  # 8bit RGBA
    ihdr = make_chunk(b'IHDR', ihdr_data)

    raw_data = b''
    for y in range(height):
        raw_data += b'\x00'  # filter byte
        for x in range(width):
            raw_data += bytes(pixels[y * width + x])

    idat = make_chunk(b'IDAT', zlib.compress(raw_data))
    iend = make_chunk(b'IEND', b'')
    return header + ihdr + idat + iend

def save_png(path, width, height, pixels):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, 'wb') as f:
        f.write(create_png(width, height, pixels))

# ============ 颜色调色板 ============
class Color:
    # 玩家
    PLAYER_SKIN     = (245, 205, 150, 255)
    PLAYER_HAIR     = (60, 40, 20, 255)
    PLAYER_SHIRT    = (74, 144, 164, 255)   # 蓝绿色
    PLAYER_PANTS    = (45, 55, 72, 255)     # 深灰蓝
    PLAYER_BOOTS    = (60, 40, 20, 255)
    PLAYER_SWORD    = (192, 192, 192, 255)
    PLAYER_SWORD_H  = (139, 69, 19, 255)

    # 哥布林
    GOBLIN_SKIN     = (139, 69, 19, 255)
    GOBLIN_EYE      = (255, 255, 0, 255)
    GOBLIN_CLOTH    = (100, 60, 40, 255)

    # 史莱姆
    SLIME_BODY      = (34, 139, 34, 200)
    SLIME_HIGHLIGHT = (80, 200, 80, 180)
    SLIME_EYE       = (255, 255, 255, 255)
    SLIME_PUPIL     = (0, 0, 0, 255)

    # 骷髅兵
    BONE            = (220, 220, 200, 255)
    BONE_DARK       = (180, 180, 160, 255)
    SKELETON_EYE    = (255, 50, 50, 120)

    # Boss
    BOSS_BODY       = (139, 0, 0, 255)
    BOSS_HIGHLIGHT  = (200, 30, 30, 255)
    BOSS_EYE        = (255, 200, 0, 255)

    # NPC
    VILLAGER_SHIRT  = (88, 101, 242, 255)
    VILLAGER_HAIR   = (180, 120, 60, 255)
    BLACKSMITH_BODY = (107, 114, 128, 255)
    MERCHANT_BODY   = (139, 92, 191, 255)
    QUEST_BODY      = (6, 95, 70, 255)

    # 瓦片
    FLOOR           = (74, 85, 104, 255)
    FLOOR_DARK      = (60, 70, 85, 255)
    WALL            = (45, 55, 72, 255)
    WALL_TOP         = (55, 65, 82, 255)
    GRASS           = (56, 161, 105, 255)
    GRASS_DARK      = (40, 140, 85, 255)
    WATER           = (49, 130, 206, 255)
    WATER_LIGHT     = (80, 160, 230, 255)
    LAVA            = (229, 62, 62, 255)
    LAVA_LIGHT      = (255, 100, 60, 255)
    DOOR            = (139, 69, 19, 255)
    DOOR_LIGHT      = (160, 90, 30, 255)

    # UI
    TRANSPARENT     = (0, 0, 0, 0)
    RED             = (229, 62, 62, 255)
    GREEN           = (56, 161, 105, 255)
    YELLOW          = (255, 204, 0, 255)
    BLUE            = (49, 130, 206, 255)
    WHITE           = (255, 255, 255, 255)
    BLACK           = (0, 0, 0, 255)
    DARK            = (26, 26, 46, 255)

# ============ 玩家精灵 64x64, 4帧x4方向 ============
def draw_player_frame(pixels, w, h, frame, direction):
    """绘制玩家帧 frame=0行走偏移, direction 0=下 1=左 2=右 3=上"""
    bounce = (0, 1, 2, 1)[frame % 4]  # 行走弹跳
    cx, cy = w // 2, h // 2
    leg_offset = (1, 2, 1, 0)[frame % 4]  # 腿部动画

    # 头部(带头发)
    head_y = 6 + bounce
    for dy in range(-4, 6):
        for dx in range(-4, 5):
            if dx*dx + dy*dy <= 20:
                i = (cy + head_y + dy) * w + (cx + dx)
                if 0 <= i < w*h:
                    pixels[i] = Color.PLAYER_SKIN

    # 头发顶
    for dy in range(-6, -2):
        for dx in range(-5, 6):
            if cx+dx >= 0 and cx+dx < w and cy+head_y+dy >= 0:
                i = (cy + head_y + dy) * w + (cx + dx)
                if 0 <= i < w*h:
                    pixels[i] = Color.PLAYER_HAIR

    # 身体
    body_top = head_y + 6
    for dy in range(8):
        for dx in range(-4, 5):
            py, px = cy + body_top + dy, cx + dx
            if 0 <= py < h and 0 <= px < w:
                pixels[py * w + px] = Color.PLAYER_SHIRT

    # 腰带
    for dx in range(-5, 6):
        py = cy + body_top + 6
        if 0 <= py < h and 0 <= cx+dx < w:
            pixels[py * w + cx + dx] = (80, 60, 40, 255)

    # 腿
    leg_top = body_top + 8
    left_shift = leg_offset if direction != 1 else -leg_offset
    right_shift = -leg_offset if direction != 1 else leg_offset
    for leg, shift in [(left_shift, -2), (right_shift, 2)]:
        for dy in range(0, 7):
            for dx in range(-2, 3):
                py = cy + leg_top + dy
                px = cx + dx + shift
                if 0 <= py < h and 0 <= px < w:
                    pixels[py * w + px] = Color.PLAYER_PANTS

    # 靴子
    boot_top = leg_top + 7
    for bx, shift in [(left_shift, -2), (right_shift, 2)]:
        for dx in range(-3, 4):
            py = cy + boot_top
            if 0 <= py < h and 0 <= cx+dx+shift < w:
                pixels[py * w + cx + dx + shift] = Color.PLAYER_BOOTS

    # 剑(右手)
    sword_x = 6
    if direction == 1: sword_x = -14  # 左朝向
    for l in range(8):
        py = cy + body_top + 2 + l
        px = cx + sword_x
        if 0 <= py < h and 0 <= px < w:
            pixels[py * w + px] = Color.PLAYER_SWORD
    # 剑柄
    for dx in range(-1, 2):
        py = cy + body_top + 1
        px = cx + sword_x + dx
        if 0 <= py < h and 0 <= px < w:
            pixels[py * w + px] = Color.PLAYER_SWORD_H

def create_player_sprite():
    """创建玩家精灵表 256x64 = 4方向x64"""
    w, h = 256, 64
    pixels = [Color.TRANSPARENT] * (w * h)
    for direction in range(4):
        for frame in range(4):
            # 每帧64x64，方向在frame里展示 简化：4帧都是同方向的不同帧
            pass
    # 简化：每个方向一帧，排成一行 4x64=256
    # 方向0=下, 1=左, 2=右, 3=上
    offsets = [(0,0), (0,-2), (0,2), (0,-4)]  # eye offset
    for d in range(4):
        ox = d * 64
        bounce = 0
        for dy in range(-4, 6):
            for dx in range(-4, 5):
                if dx*dx/(16) + dy*dy/(20) <= 1:
                    px, py = ox + 32 + dx, 28 + dy + d * 0
                    if 0 <= py < h: pixels[py * w + px] = Color.PLAYER_SKIN
        for dy in range(-7, -2):
            for dx in range(-5, 6):
                px, py = ox + 32 + dx, 28 + dy
                if 0 <= py < h:
                    pixels[py * w + px] = Color.PLAYER_HAIR
        # 眼睛
        eye_dx = offsets[d][1]
        for dy in (-2, -1):
            for dx in range(-1, 2):
                px, py = ox + 32 + dx + eye_dx, 28 + dy
                if 0 <= py < h: pixels[py * w + px] = Color.WHITE
        # 瞳孔
        px, py = ox + 32 + eye_dx, 28 - 1
        if 0 <= py < h: pixels[py * w + px] = Color.BLACK

        # 身体
        for dy in range(8):
            for dx in range(-5, 6):
                px, py = ox + 32 + dx, 36 + dy
                if 0 <= py < h: pixels[py * w + px] = Color.PLAYER_SHIRT
        # 腰带
        for dx in range(-6, 7):
            px, py = ox + 32 + dx, 42
            if 0 <= py < h: pixels[py * w + px] = (80, 60, 40, 255)
        # 腿
        for side, sx in [(-1, -3), (1, 3)]:
            for dy in range(6):
                for dx in range(-2, 3):
                    px, py = ox + 32 + dx + sx, 44 + dy
                    if 0 <= py < h: pixels[py * w + px] = Color.PLAYER_PANTS
        # 靴子
        for sx in [-3, 3]:
            for dx in range(-3, 4):
                px, py = ox + 32 + dx + sx, 50
                if 0 <= py < h: pixels[py * w + px] = Color.PLAYER_BOOTS
        # 剑
        sx = 8 if d != 1 else -14
        for l in range(8):
            px, py = ox + 32 + sx, 36 + l
            if 0 <= py < h: pixels[py * w + px] = Color.PLAYER_SWORD

    save_png(os.path.join(BASE_DIR, "resources", "images", "entities", "player.png"), w, h, pixels)
    print("  Created: player.png")

# ============ 敌人精灵 ============
def create_enemy_sprite(name, w, h, draw_fn):
    pixels = [Color.TRANSPARENT] * (w * h)
    draw_fn(pixels, w, h)
    save_png(os.path.join(BASE_DIR, "resources", "images", "entities", f"{name}.png"), w, h, pixels)
    print(f"  Created: {name}.png")

def draw_goblin(pixels, w, h):
    cx, cy = w//2, h//2
    # 身体
    for dy in range(8):
        for dx in range(-4, 5):
            i = (cy+dy+2)*w + cx+dx
            if 0 <= i < w*h: pixels[i] = Color.GOBLIN_SKIN
    # 头
    for dy in range(-5, 3):
        for dx in range(-4, 5):
            i = (cy+dy)*w + cx+dx
            if 0 <= i < w*h and dx*dx + dy*dy <= 20:
                pixels[i] = Color.GOBLIN_SKIN
    # 眼睛
    for dy in (-3,):
        for dx in (-3, 3):
            i = (cy+dy)*w + cx+dx
            if 0 <= i < w*h: pixels[i] = Color.GOBLIN_EYE
    # 瞳孔
    pixels[(cy-3)*w+cx-3] = Color.BLACK
    pixels[(cy-3)*w+cx+3] = Color.BLACK
    # 耳朵(尖耳)
    pixels[(cy-4)*w+cx-5] = Color.GOBLIN_SKIN
    pixels[(cy-4)*w+cx+5] = Color.GOBLIN_SKIN
    pixels[(cy-3)*w+cx-5] = Color.GOBLIN_SKIN
    pixels[(cy-3)*w+cx+5] = Color.GOBLIN_SKIN
    # 腿
    for dy in range(5):
        for dx in (-2, 2):
            i = (cy+10+dy)*w + cx+dx
            if 0 <= i < w*h: pixels[i] = Color.GOBLIN_CLOTH

def draw_slime(pixels, w, h):
    cx, cy = w//2, h//2
    # 半透明圆润身体
    for dy in range(-8, 6):
        for dx in range(-8, 9):
            dist = (dx*dx + dy*dy*1.5)
            if dist <= 64:
                i = (cy+dy+2)*w + cx+dx
                if 0 <= i < w*h:
                    a = max(0, min(200, int(220 - dist*2)))
                    pixels[i] = (34, 139, 34, a)
    # 高光
    for dy in range(-6, -2):
        for dx in range(-4, 5):
            i = (cy+dy)*w + cx+dx
            if 0 <= i < w*h:
                r, g, b, a = pixels[i]
                if a > 50:
                    pixels[i] = (min(255, r+50), min(255, g+50), min(255, b+30), a)
    # 眼睛
    for ex in (-4, 4):
        pixels[(cy-3)*w+cx+ex] = Color.SLIME_EYE
        pixels[(cy-3)*w+cx+ex+1] = Color.SLIME_EYE
        pixels[(cy-2)*w+cx+ex] = Color.SLIME_EYE
        pixels[(cy-2)*w+cx+ex+1] = Color.SLIME_EYE
        pixels[(cy-2)*w+cx+ex+2] = Color.SLIME_PUPIL

def draw_skeleton(pixels, w, h):
    cx, cy = w//2, h//2
    # 头骨
    for dy in range(-5, 3):
        for dx in range(-4, 5):
            i = (cy+dy)*w + cx+dx
            if 0 <= i < w*h and dx*dx*2 + dy*dy <= 50:
                pixels[i] = Color.BONE
    # 眼窝
    for ex in (-2, 2):
        for dy in (-2, 0):
            for dx in (-1, 1):
                i = (cy+dy)*w + cx+ex+dx
                if 0 <= i < w*h: pixels[i] = Color.SKELETON_EYE
    # 下颌
    pixels[(cy+3)*w+cx-1] = Color.BONE_DARK
    pixels[(cy+3)*w+cx] = Color.BONE_DARK
    pixels[(cy+3)*w+cx+1] = Color.BONE_DARK
    # 身体(肋骨)
    for dy in range(4, 11):
        for dx in range(-2, 3):
            i = (cy+dy)*w + cx+dx
            if 0 <= i < w*h: pixels[i] = Color.BONE
    # 肋骨横纹
    for ry in (5, 7, 9):
        for dx in range(-3, 4):
            i = (cy+ry)*w + cx+dx
            if 0 <= i < w*h and pixels[i] == Color.BONE:
                pixels[i] = Color.BONE_DARK
    # 手臂
    for arm_x in (-4, 4):
        for dy in range(5, 9):
            i = (cy+dy)*w + cx+arm_x
            if 0 <= i < w*h: pixels[i] = Color.BONE
    # 腿
    for leg_x in (-2, 2):
        for dy in range(15, 20):
            i = (cy+dy)*w + cx+leg_x
            if 0 <= i < w*h: pixels[i] = Color.BONE_DARK

def draw_boss(pixels, w, h):
    cx, cy = w//2, h//2
    # 大身体
    for dy in range(-12, 15):
        for dx in range(-12, 13):
            i = (cy+dy)*w + cx+dx
            if 0 <= i < w*h and dx*dx + dy*dy*1.2 <= 160:
                pixels[i] = Color.BOSS_BODY
    # 高光
    for dy in range(-10, -4):
        for dx in range(-6, 7):
            i = (cy+dy)*w + cx+dx
            if 0 <= i < w*h and dx*dx + dy*dy <= 80:
                pixels[i] = Color.BOSS_HIGHLIGHT
    # 眼睛(发光)
    for ey in (-5, -5):
        for ex in (-5, 1):
            i = (cy+ey)*w + cx+ex+4
            if 0 <= i < w*h: pixels[i] = Color.BOSS_EYE
        for ex in (1, 6):
            i = (cy+ey)*w + cx+ex
            if 0 <= i < w*h: pixels[i] = Color.BOSS_EYE
    # 瞳孔
    pixels[(cy-5)*w+cx-3] = Color.BLACK
    pixels[(cy-5)*w+cx+4] = Color.BLACK
    # 角
    for hx in (-6, 6):
        for hy in range(-14, -8):
            i = (cy+hy)*w + cx+hx
            if 0 <= i < w*h: pixels[i] = Color.BOSS_HIGHLIGHT

# ============ NPC精灵 ============
def create_npc_sprite(name, body_color, hair_color=None):
    w, h = 48, 48
    pixels = [Color.TRANSPARENT] * (w * h)
    cx, cy = w//2, h//2
    hair = hair_color or body_color

    # 头
    for dy in range(-5, 4):
        for dx in range(-4, 5):
            i = (cy+dy)*w + cx+dx
            if 0 <= i < w*h and dx*dx + dy*dy <= 18:
                pixels[i] = Color.PLAYER_SKIN
    # 头发
    for dy in range(-7, -3):
        for dx in range(-4, 5):
            i = (cy+dy)*w + cx+dx
            if 0 <= i < w*h: pixels[i] = hair
    # 身体
    for dy in range(4, 13):
        for dx in range(-4, 5):
            i = (cy+dy)*w + cx+dx
            if 0 <= i < w*h: pixels[i] = body_color
    # 腿
    for dy in range(13, 17):
        for dx in range(-2, 3):
            i = (cy+dy)*w + cx+dx
            if 0 <= i < w*h: pixels[i] = Color.PLAYER_PANTS

    save_png(os.path.join(BASE_DIR, "resources", "images", "entities", f"{name}.png"), w, h, pixels)
    print(f"  Created: {name}.png")

# ============ 瓦片精灵 ============
def create_tile_sprite(name, main_color, accent_color=None, pattern=False):
    w, h = 32, 32
    pixels = [main_color] * (w * h)
    accent = accent_color or main_color

    if pattern:
        for y in range(h):
            for x in range(w):
                p = ((x+y*2) // 4) % 2
                if p == 0:
                    pixels[y*w+x] = main_color
                else:
                    pixels[y*w+x] = accent

    # 边缘暗化
    for i in range(w):
        pixels[i] = (max(0,main_color[0]-15),max(0,main_color[1]-15),max(0,main_color[2]-15),255)  # 顶边
        pixels[(h-1)*w+i] = (max(0,main_color[0]-20),max(0,main_color[1]-20),max(0,main_color[2]-20),255)
    for i in range(h):
        pixels[i*w] = (max(0,main_color[0]-10),max(0,main_color[1]-10),max(0,main_color[2]-10),255)
        pixels[i*w+w-1] = (max(0,main_color[0]-10),max(0,main_color[1]-10),max(0,main_color[2]-10),255)

    save_png(os.path.join(BASE_DIR, "resources", "images", "tiles", f"{name}.png"), w, h, pixels)
    print(f"  Created: {name}.png")

def create_water_tile():
    w, h = 32, 32
    pixels = [Color.TRANSPARENT] * (w * h)
    for y in range(h):
        for x in range(w):
            wave = ((x + y*3) // 3) % 2
            color = Color.WATER if wave == 0 else Color.WATER_LIGHT
            pixels[y*w+x] = color
    save_png(os.path.join(BASE_DIR, "resources", "images", "tiles", "water.png"), w, h, pixels)
    print("  Created: water.png")

def create_lava_tile():
    w, h = 32, 32
    pixels = [Color.TRANSPARENT] * (w * h)
    for y in range(h):
        for x in range(w):
            hot = ((x + y + ((x*y)//7)) // 4) % 3
            if hot == 0:
                pixels[y*w+x] = Color.LAVA
            elif hot == 1:
                pixels[y*w+x] = Color.LAVA_LIGHT
            else:
                pixels[y*w+x] = (180, 40, 20, 255)
    save_png(os.path.join(BASE_DIR, "resources", "images", "tiles", "lava.png"), w, h, pixels)
    print("  Created: lava.png")

def create_door_tile():
    w, h = 32, 32
    pixels = [Color.TRANSPARENT] * (w * h)
    for y in range(h):
        for x in range(w):
            if y <= 2 or y >= h-3:
                pixels[y*w+x] = Color.DOOR
            elif x <= 2 or x >= w-3:
                pixels[y*w+x] = Color.DOOR
            else:
                pixels[y*w+x] = Color.DOOR_LIGHT
    # 门把手
    for dy in range(14, 18):
        for dx in range(w-6, w-3):
            if 0 <= dy < h:
                pixels[dy*w+dx] = Color.YELLOW
    save_png(os.path.join(BASE_DIR, "resources", "images", "tiles", "door.png"), w, h, pixels)
    print("  Created: door.png")

def create_grass_tile():
    w, h = 32, 32
    pixels = [Color.GRASS] * (w * h)
    for y in range(h):
        for x in range(w):
            if (x + y*3) % 5 == 0:
                pixels[y*w+x] = Color.GRASS_DARK
    # 随机小草点
    for _ in range(8):
        gx = (_ * 37 + 13) % w
        gy = (_ * 23 + 7) % h
        pixels[gy*w+gx] = (100, 200, 50, 255)
    save_png(os.path.join(BASE_DIR, "resources", "images", "tiles", "grass.png"), w, h, pixels)
    print("  Created: grass.png")

# ============ UI图标 ============
def create_ui_icons():
    w, h = 32, 32
    T = Color.TRANSPARENT

    # 剑图标
    pixels = [T] * (w * h)
    for x in range(14, 19):
        for y in range(4, 24):
            pixels[y*w+x] = Color.PLAYER_SWORD
    for x in range(12, 21):
        for y in range(24, 27):
            pixels[y*w+x] = Color.PLAYER_SWORD_H
    save_png(os.path.join(BASE_DIR, "resources", "images", "ui", "icon_sword.png"), w, h, pixels)
    print("  Created: icon_sword.png")

    # 护甲图标
    pixels = [T] * (w * h)
    for y in range(4, 18):
        for x in range(8, 24):
            if (y-4)*(y-4)/40 + (x-16)*(x-16)/64 <= 1:
                pixels[y*w+x] = Color.PLAYER_SHIRT
    for y in range(14, 26):
        for x in range(10, 22):
            pixels[y*w+x] = Color.GREEN
    save_png(os.path.join(BASE_DIR, "resources", "images", "ui", "icon_armor.png"), w, h, pixels)
    print("  Created: icon_armor.png")

    # 药水图标
    pixels = [T] * (w * h)
    for y in range(8, 26):
        for x in range(11, 21):
            pixels[y*w+x] = Color.RED
    for y in range(4, 8):
        for x in range(13, 19):
            pixels[y*w+x] = (255, 255, 255, 255)
    pixels[8*w+16] = (255, 255, 255, 255)
    pixels[8*w+17] = (255, 255, 255, 255)
    save_png(os.path.join(BASE_DIR, "resources", "images", "ui", "icon_potion.png"), w, h, pixels)
    print("  Created: icon_potion.png")

    # 钥匙图标
    pixels = [T] * (w * h)
    for x in range(14, 22):
        for y in range(8, 10):
            pixels[y*w+x] = Color.YELLOW
    for y in range(10, 24):
        pixels[y*w+18] = Color.YELLOW
        pixels[y*w+19] = Color.YELLOW
    for x in range(10, 20):
        for y in range(22, 25):
            pixels[y*w+x] = Color.YELLOW
    save_png(os.path.join(BASE_DIR, "resources", "images", "ui", "icon_key.png"), w, h, pixels)
    print("  Created: icon_key.png")

    # 宝石图标
    pixels = [T] * (w * h)
    cx, cy = 16, 16
    for dy in range(-8, 9):
        for dx in range(-6, 7):
            if abs(dx)+abs(dy) <= 8:
                i = (cy+dy)*w + cx+dx
                if 0 <= i < w*h:
                    pixels[i] = Color.RED
    pixels[14*w+14] = (255, 255, 255, 255)
    pixels[14*w+15] = (255, 255, 255, 255)
    pixels[15*w+14] = (255, 255, 255, 255)
    save_png(os.path.join(BASE_DIR, "resources", "images", "ui", "icon_gem.png"), w, h, pixels)
    print("  Created: icon_gem.png")

if __name__ == "__main__":
    print("Generating pixel art resources...")
    print("\n[Entities]")
    create_player_sprite()
    create_enemy_sprite("goblin", 48, 48, draw_goblin)
    create_enemy_sprite("slime", 48, 48, draw_slime)
    create_enemy_sprite("skeleton", 48, 48, draw_skeleton)
    create_enemy_sprite("boss", 64, 64, draw_boss)
    create_npc_sprite("villager", Color.VILLAGER_SHIRT, Color.VILLAGER_HAIR)
    create_npc_sprite("blacksmith", Color.BLACKSMITH_BODY, (60, 60, 70, 255))
    create_npc_sprite("shopkeeper", Color.MERCHANT_BODY, Color.VILLAGER_HAIR)
    create_npc_sprite("quest_giver", Color.QUEST_BODY, Color.VILLAGER_HAIR)

    print("\n[Tiles]")
    create_tile_sprite("floor", Color.FLOOR, Color.FLOOR_DARK, True)
    create_tile_sprite("wall", Color.WALL, Color.WALL_TOP, True)
    grass = create_grass_tile()
    water = create_water_tile()
    lava = create_lava_tile()
    door = create_door_tile()

    print("\n[UI Icons]")
    create_ui_icons()

    print("\nDone! All pixel art generated.")
