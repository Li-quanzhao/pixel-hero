#include "WaveManager.h"
#include "SurvivalScene.h"
#include <QRandomGenerator>
#include <cmath>

WaveManager::WaveManager(SurvivalScene* scene, QObject* parent)
    : QObject(parent), m_scene(scene)
    , m_currentWave(0), m_currentWaveIndex(0)
    , m_totalKills(0), m_spawnedInWave(0)
    , m_elapsedTime(0), m_spawnTimer(0)
    , m_isRunning(false)
{
}

WaveManager::~WaveManager() {}

void WaveManager::start()
{
    m_isRunning = true;
    m_elapsedTime = 0;
    m_currentWave = 0;
    m_currentWaveIndex = -1;
    m_totalKills = 0;
    m_spawnedInWave = 0;
    m_spawnTimer = 0;
    loadWaveConfig();
    m_enemyFactory.loadStats(GameData::instance()->enemyBaseStats());
    advanceWave();
}

void WaveManager::loadWaveConfig()
{
    m_waveConfigs = GameData::instance()->waveConfigs();
}

void WaveManager::advanceWave()
{
    m_currentWaveIndex++;
    if (m_currentWaveIndex >= m_waveConfigs.size()) return;

    WaveConfig& wc = m_waveConfigs[m_currentWaveIndex];
    m_currentWave = wc.waveNumber;
    m_spawnedInWave = 0;
    m_spawnTimer = 0;

    emit waveChanged(m_currentWave);
}

void WaveManager::update(qreal deltaTime)
{
    if (!m_isRunning) return;

    m_elapsedTime += deltaTime;
    m_spawnTimer -= deltaTime;

    // 检查波次切换
    if (m_currentWaveIndex >= 0 && m_currentWaveIndex < m_waveConfigs.size()) {
        WaveConfig& wc = m_waveConfigs[m_currentWaveIndex];

        // 已生成完毕，检查下一波时间
        if (m_spawnedInWave >= wc.totalCount) {
            int nextIdx = m_currentWaveIndex + 1;
            if (nextIdx < m_waveConfigs.size()) {
                WaveConfig& nextWc = m_waveConfigs[nextIdx];
                if (m_elapsedTime >= nextWc.startTime) {
                    advanceWave();
                }
            }
        }
    }

    // 生成敌人
    if (m_currentWaveIndex >= 0 && m_currentWaveIndex < m_waveConfigs.size()) {
        WaveConfig& wc = m_waveConfigs[m_currentWaveIndex];
        if (m_spawnedInWave < wc.totalCount) {
            // Boss/精英波一次性生成
            if (wc.isBossWave || wc.isEliteWave) {
                if (m_spawnedInWave == 0) {
                    for (int i = 0; i < wc.totalCount; i++) {
                        spawnEnemy();
                    }
                }
            } else if (m_spawnTimer <= 0) {
                spawnEnemy();
                m_spawnTimer = wc.spawnInterval;
            }
        }
    }
}

void WaveManager::spawnEnemy()
{
    if (countAliveEnemies() >= MAX_ALIVE_ENEMIES) return;

    if (m_currentWaveIndex < 0 || m_currentWaveIndex >= m_waveConfigs.size()) return;
    WaveConfig& wc = m_waveConfigs[m_currentWaveIndex];
    if (m_spawnedInWave >= wc.totalCount) return;

    // 随机选一个敌人类型
    int idx = QRandomGenerator::global()->bounded(wc.enemies.size());
    QString enemyId = wc.enemies[idx];

    // 生成位置：场景边缘随机点（距玩家 150-400px）
    qreal x, y;
    int edge = QRandomGenerator::global()->bounded(4);
    switch (edge) {
    case 0: x = 0;                     y = QRandomGenerator::global()->bounded(pixel_hero::config::SCREEN_H); break;
    case 1: x = pixel_hero::config::SCREEN_W;  y = QRandomGenerator::global()->bounded(pixel_hero::config::SCREEN_H); break;
    case 2: x = QRandomGenerator::global()->bounded(pixel_hero::config::SCREEN_W); y = 0;                     break;
    case 3: x = QRandomGenerator::global()->bounded(pixel_hero::config::SCREEN_W); y = pixel_hero::config::SCREEN_H; break;
    }

    Enemy* enemy = m_enemyFactory.createEnemy(enemyId, x, y);
    m_scene->addItem(enemy);

    if (wc.isBossWave) {
        emit bossSpawned(enemy);
    }

    m_spawnedInWave++;
}

int WaveManager::countAliveEnemies() const
{
    // 通过 scene 遍历找 Enemy 实例
    int count = 0;
    QList<QGraphicsItem*> items = m_scene->items();
    for (QGraphicsItem* item : items) {
        Enemy* e = dynamic_cast<Enemy*>(item);
        if (e && e->isAlive()) count++;
    }
    return count;
}

void WaveManager::restoreState(int wave, int kills, float elapsedTime)
{
    m_totalKills = kills;
    m_elapsedTime = elapsedTime;
    m_currentWave = 1;
    m_currentWaveIndex = -1;
    m_spawnedInWave = 0;
    m_spawnTimer = 0;
    // 快进到指定波次
    for (int i = 0; i < m_waveConfigs.size(); i++) {
        if (m_waveConfigs[i].waveNumber <= wave) {
            m_currentWave = m_waveConfigs[i].waveNumber;
            m_currentWaveIndex = i;
            m_spawnedInWave = 0;
            m_spawnTimer = 0;
        }
    }
    m_isRunning = true;
    emit waveChanged(m_currentWave);
}

void WaveManager::addKill()
{
    m_totalKills++;
    emit enemyKilled();
}
