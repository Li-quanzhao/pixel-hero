#include "EnemyManager.h"
#include "Enemy.h"
#include "SurvivalPlayer.h"
#include "WaveManager.h"
#include "EffectManager.h"
#include "SurvivalStats.h"
#include "config/GameConfig.h"
#include <QLineF>
#include <QGraphicsScene>
#include <algorithm>

namespace pixel_hero {
namespace survival {

using ::Enemy;
using ::SurvivalPlayer;
using ::WaveManager;
using ::SurvivalStats;

EnemyManager::EnemyManager(QGraphicsScene* scene, WaveManager* wm,
                           EffectManager* fx, QObject* parent)
    : QObject(parent), m_scene(scene), m_waveManager(wm), m_effectManager(fx)
{
}

void EnemyManager::update(qreal dt, SurvivalPlayer* player)
{
    if (!m_scene || !player) return;

    // 单次扫描收集存活敌人
    QList<Enemy*> enemies;
    QList<QGraphicsItem*> items = m_scene->items();
    for (QGraphicsItem* item : items) {
        Enemy* e = dynamic_cast<Enemy*>(item);
        if (e && e->isAlive()) enemies.append(e);
    }

    // AI更新
    for (Enemy* e : enemies) {
        e->update(dt);
        e->updateAI(player, enemies, dt);
    }

    // 清理死亡
    cleanupDead(player);
}

QList<Enemy*> EnemyManager::aliveEnemies() const
{
    QList<Enemy*> result;
    if (!m_scene) return result;
    QList<QGraphicsItem*> items = m_scene->items();
    for (QGraphicsItem* item : items) {
        Enemy* e = dynamic_cast<Enemy*>(item);
        if (e && e->isAlive()) result.append(e);
    }
    return result;
}

Enemy* EnemyManager::findNearest(QPointF pos, qreal range) const
{
    if (!m_scene) return nullptr;
    QList<QGraphicsItem*> items = m_scene->items();
    Enemy* nearest = nullptr;
    qreal minDist = range;

    for (QGraphicsItem* item : items) {
        Enemy* e = dynamic_cast<Enemy*>(item);
        if (!e || !e->isAlive()) continue;
        qreal d = QLineF(pos, e->pos()).length();
        if (d < minDist) { minDist = d; nearest = e; }
    }
    return nearest;
}

void EnemyManager::cleanupDead(SurvivalPlayer* player)
{
    if (!m_scene) return;
    QList<QGraphicsItem*> items = m_scene->items();
    for (QGraphicsItem* item : items) {
        Enemy* enemy = dynamic_cast<Enemy*>(item);
        if (!enemy || enemy->isAlive()) continue;

        player->addExp(enemy->expReward());
        m_totalKills++;
        m_waveManager->addKill();
        if (m_stats) m_stats->addKill();
        m_scene->removeItem(enemy);
        delete enemy;
    }
}

} // namespace survival
} // namespace pixel_hero
