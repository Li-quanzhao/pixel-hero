#include "SkillExecutor.h"
#include "Enemy.h"
#include "EnemyManager.h"
#include "EffectManager.h"
#include "SurvivalPlayer.h"
#include "config/GameConfig.h"
#include <QLineF>
#include <algorithm>

namespace pixel_hero {
namespace survival {

using ::Enemy;

// ==================== 工厂 ====================

std::unique_ptr<SkillExecutor> SkillExecutor::create(const QString& skillId)
{
    if (skillId == "fireball")   return std::make_unique<FireballExecutor>();
    if (skillId == "lightning")  return std::make_unique<LightningExecutor>();
    if (skillId == "frost_nova") return std::make_unique<FrostNovaExecutor>();
    return nullptr;
}

// ==================== 火球术 ====================

void FireballExecutor::execute(const SurvivalPlayer::ActiveSkill& skill,
                               QPointF playerPos,
                               EnemyManager* enemyMgr,
                               EffectManager* fx)
{
    QList<Enemy*> enemies = enemyMgr->aliveEnemies();
    if (enemies.isEmpty()) return;

    Enemy* nearest = nullptr;
    qreal minDist = 99999;
    for (Enemy* e : enemies) {
        qreal d = QLineF(playerPos, e->pos()).length();
        if (d < minDist) { minDist = d; nearest = e; }
    }
    if (nearest) {
        fx->showFireball(playerPos, nearest->pos());
        nearest->takeDamage(skill.damage);
    }
}

// ==================== 闪电链 ====================

void LightningExecutor::execute(const SurvivalPlayer::ActiveSkill& skill,
                                QPointF playerPos,
                                EnemyManager* enemyMgr,
                                EffectManager* fx)
{
    QList<Enemy*> enemies = enemyMgr->aliveEnemies();
    if (enemies.isEmpty()) return;

    struct ED { Enemy* e; qreal d; };
    QList<ED> sorted;
    for (Enemy* e : enemies) {
        sorted.append({e, QLineF(playerPos, e->pos()).length()});
    }
    std::sort(sorted.begin(), sorted.end(),
              [](const ED& a, const ED& b) { return a.d < b.d; });

    int chain = qMin(skill.extra, sorted.size());
    QPointF prevPos = playerPos;
    for (int i = 0; i < chain; i++) {
        sorted[i].e->takeDamage(skill.damage);
        fx->showLightning(prevPos, sorted[i].e->pos());
        prevPos = sorted[i].e->pos();
    }
}

// ==================== 冰霜新星 ====================

void FrostNovaExecutor::execute(const SurvivalPlayer::ActiveSkill& skill,
                                QPointF playerPos,
                                EnemyManager* enemyMgr,
                                EffectManager* fx)
{
    QList<Enemy*> enemies = enemyMgr->aliveEnemies();
    if (enemies.isEmpty()) return;

    // 找最近敌人
    Enemy* nearest = nullptr;
    qreal minDist = 99999;
    for (Enemy* e : enemies) {
        qreal d = QLineF(playerPos, e->pos()).length();
        if (d < minDist) { minDist = d; nearest = e; }
    }
    if (!nearest) return;

    // 冰弹飞行特效
    fx->showFrostProjectile(playerPos, nearest->pos());

    // 命中伤害 + 溅射伤害
    QPointF hitPos = nearest->pos();
    float radius = static_cast<float>(skill.extra);
    nearest->takeDamage(skill.damage);

    for (Enemy* e : enemies) {
        if (e == nearest) continue;
        qreal d = QLineF(hitPos, e->pos()).length();
        if (d <= radius) e->takeDamage(skill.damage);
    }

    // 爆炸特效
    fx->showFrostExplosion(hitPos, radius);
}

} // namespace survival
} // namespace pixel_hero
