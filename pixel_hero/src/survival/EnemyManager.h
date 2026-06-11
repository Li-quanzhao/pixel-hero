#ifndef ENEMYMANAGER_H
#define ENEMYMANAGER_H

#include <QObject>
#include <QList>
#include <QPointF>
#include <QGraphicsScene>

class Enemy;
class SurvivalPlayer;
class WaveManager;
class SurvivalStats;

namespace pixel_hero {
namespace survival {

class EffectManager;

class EnemyManager : public QObject
{
    Q_OBJECT
public:
    EnemyManager(QGraphicsScene* scene, ::WaveManager* wm,
                 EffectManager* fx, QObject* parent = nullptr);

    void update(qreal dt, ::SurvivalPlayer* player);
    QList<::Enemy*> aliveEnemies() const;

    int totalKills() const { return m_totalKills; }
    void setTotalKills(int v) { m_totalKills = v; }
    void setStats(::SurvivalStats* s) { m_stats = s; }

    ::Enemy* findNearest(QPointF pos, qreal range) const;

private:
    QGraphicsScene*  m_scene;
    ::WaveManager*   m_waveManager;
    EffectManager*   m_effectManager;
    ::SurvivalStats* m_stats = nullptr;
    int              m_totalKills = 0;

    void cleanupDead(::SurvivalPlayer* player);
};

} // namespace survival
} // namespace pixel_hero

#endif // ENEMYMANAGER_H
