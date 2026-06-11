#ifndef GAMECONFIG_H
#define GAMECONFIG_H

#include <QRectF>

namespace pixel_hero {
namespace config {

// 场景
constexpr float SCREEN_W           = 800.0f;
constexpr float SCREEN_H           = 600.0f;
constexpr QRectF SCENE_RECT        = QRectF(0, 0, SCREEN_W, SCREEN_H);

// 游戏循环
constexpr int   GAME_TICK_MS       = 16;      // ~60fps
constexpr float DT                 = 0.016f;  // 帧间隔(秒)

// 玩家
constexpr float PLAYER_PADDING     = 24.0f;   // 边界碰撞内边距
constexpr float PLAYER_ATTACK_RANGE = 60.0f;  // 普攻范围

// 升级
constexpr int   EXP_PER_LEVEL_BASE = 80;      // 基础升级经验
constexpr int   LEVEL_HP_BONUS     = 20;      // 每级生命加成
constexpr int   LEVEL_ATK_BONUS    = 3;       // 每级攻击加成
constexpr int   LEVEL_DEF_BONUS    = 2;       // 每级防御加成

// 敌人
constexpr int   MAX_ALIVE_ENEMIES  = 30;      // 同时存活上限

// 敌人闪烁
constexpr int   FLASH_FRAMES       = 5;       // 受伤闪烁帧数(~80ms)
constexpr int   FLASH_ALPHA        = 180;     // 闪烁叠加透明度

// 拾取物
constexpr float PICKUP_MAGNET_RANGE   = 40.0f;   // 磁吸范围(px)
constexpr float PICKUP_LIFETIME       = 15.0f;   // 生命周期(秒)
constexpr float PICKUP_BLINK_START    = 3.0f;    // 闪烁开始(剩余秒)
constexpr float PICKUP_MAGNET_SPEED   = 200.0f;  // 磁吸移动速度(px/s)
constexpr int   MAX_PICKUPS           = 200;     // 同时存在上限
constexpr int   HP_DROP_RECOVERY      = 5;       // 血瓶固定恢复量

} // namespace config
} // namespace pixel_hero

#endif // GAMECONFIG_H
