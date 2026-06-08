#!/usr/bin/env python3
"""
像素游戏音效生成器
生成简单的 WAV 音效文件（纯Python，无依赖）
"""
import struct
import math
import os

OUTPUT_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "pixel_hero", "resources", "audio")

def generate_wav(filename, duration_sec, frequency, wave_type="square", volume=0.3, sample_rate=22050):
    """生成WAV音效文件"""
    num_samples = int(duration_sec * sample_rate)
    samples = []

    for i in range(num_samples):
        t = i / sample_rate
        # 衰减包络
        env = max(0, 1.0 - i / num_samples * 0.99)
        # 频率微扫
        freq = frequency * (1.0 + 0.1 * (1 - i / num_samples)) if wave_type == "sweep" else frequency

        phase = (t * freq) % 1.0

        if wave_type == "square":
            val = 1.0 if phase < 0.5 else -1.0
        elif wave_type == "sine":
            val = math.sin(phase * 2 * math.pi)
        elif wave_type == "triangle":
            val = 2.0 * abs(2.0 * phase - 1.0) - 1.0
        elif wave_type == "noise":
            val = (hash(i) % 10000) / 5000.0 - 1.0
        elif wave_type == "sweep":
            val = math.sin(t * freq * 2 * math.pi)

        val = int(val * volume * env * 32767)
        val = max(-32767, min(32767, val))
        samples.append(val)

    # 写入WAV文件
    os.makedirs(os.path.dirname(filename), exist_ok=True)
    with open(filename, 'wb') as f:
        data_size = num_samples * 2
        # RIFF header
        f.write(b'RIFF')
        f.write(struct.pack('<I', 36 + data_size))
        f.write(b'WAVE')
        # fmt chunk
        f.write(b'fmt ')
        f.write(struct.pack('<IHHIIHH', 16, 1, 1, sample_rate, sample_rate * 2, 2, 16))
        # data chunk
        f.write(b'data')
        f.write(struct.pack('<I', data_size))
        for s in samples:
            f.write(struct.pack('<h', s))
    print(f"  Created: {os.path.basename(filename)}")

if __name__ == "__main__":
    print("Generating audio assets...\n")

    # 攻击音效
    generate_wav(os.path.join(OUTPUT_DIR, "attack.wav"), 0.15, 440, "square", 0.3)
    # 受伤音效
    generate_wav(os.path.join(OUTPUT_DIR, "hit.wav"), 0.2, 220, "noise", 0.25)
    # 技能音效
    generate_wav(os.path.join(OUTPUT_DIR, "skill.wav"), 0.3, 880, "sweep", 0.3)
    # 拾取道具音效
    generate_wav(os.path.join(OUTPUT_DIR, "pickup.wav"), 0.2, 660, "triangle", 0.3)
    # 升级音效
    generate_wav(os.path.join(OUTPUT_DIR, "levelup.wav"), 0.5, 523, "sine", 0.3)
    # 菜单确认音效
    generate_wav(os.path.join(OUTPUT_DIR, "confirm.wav"), 0.1, 880, "triangle", 0.25)
    # 菜单移动音效
    generate_wav(os.path.join(OUTPUT_DIR, "menu_move.wav"), 0.05, 660, "triangle", 0.2)
    # 敌人死亡音效
    generate_wav(os.path.join(OUTPUT_DIR, "enemy_death.wav"), 0.3, 330, "noise", 0.3)
    # 玩家死亡音效
    generate_wav(os.path.join(OUTPUT_DIR, "player_death.wav"), 0.8, 110, "sweep", 0.3)
    # 开门音效
    generate_wav(os.path.join(OUTPUT_DIR, "door_open.wav"), 0.3, 220, "sine", 0.2)

    print("\nDone! All audio generated.")
