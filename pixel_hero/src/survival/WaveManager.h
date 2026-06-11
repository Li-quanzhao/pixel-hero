 #ifndef WAVEMANAGER_H
#define WAVEMANAGER_H

#include <QObject>
#include <QList>
#include "config/GameConfig.h"
#include "utils/GameData.h"
#include "EnemyFactory.h"

class SurvivalScene;

class WaveManager : public QObject
{
    Q_OBJECT

public:
    WaveManager(SurvivalScene* scene, QObject* parent = nullptr);
    ~WaveManager();

    void start();
    void update(qreal deltaTime);
    void restoreState(int wave, int kills, float elapsedTime = 0);
    void loadWaveConfig();

    int currentWave() const { return m_currentWave; }
    int totalKills() const { return m_totalKills; }
    float elapsedTime() const { return m_elapsedTime; }

    void addKill();

signals:
    void waveChanged(int wave);
    void bossSpawned(class Enemy* boss);
    void enemyKilled();

private:
    SurvivalScene* m_scene;
    EnemyFactory   m_enemyFactory;
    QList<WaveConfig> m_waveConfigs;
    int   m_currentWave;
    int   m_currentWaveIndex;
    int   m_totalKills;
    int   m_spawnedInWave;
    float m_elapsedTime;
    float m_spawnTimer;
    bool  m_isRunning;

    static const int MAX_ALIVE_ENEMIES = pixel_hero::config::MAX_ALIVE_ENEMIES;

    void advanceWave();
    void spawnEnemy();
    QString randomEnemyType() const;
    int countAliveEnemies() const;
};

#endif // WAVEMANAGER_H
