#ifndef SURVIVALSCENE_H
#define SURVIVALSCENE_H

#include <QGraphicsScene>
#include <QTimer>
#include <memory>
#include "SurvivalPlayer.h"
#include "WaveManager.h"
#include "UpgradeUI.h"
#include "SurvivalHUD.h"
#include "EffectManager.h"
#include "EnemyManager.h"

struct SurvivalSaveData;
class SurvivalStats;

class SurvivalScene : public QGraphicsScene
{
    Q_OBJECT

public:
    SurvivalScene(QObject* parent = nullptr);
    ~SurvivalScene();

    void startGame();
    void startGame(const QString& characterId, const QString& weaponId);
    void startFromSave(const SurvivalSaveData& data);
    void pauseGame();
    void resumeGame();
    void endGame();

    void setStats(SurvivalStats* stats) { m_stats = stats; }
    SurvivalStats* stats() const { return m_stats; }

    SurvivalPlayer* player() const { return m_player; }
    WaveManager* waveManager() const { return m_waveManager; }
    int totalKills() const { return m_enemyManager->totalKills(); }
    float elapsedTime() const;

signals:
    void gameFinished(int wave, int kills, float time, bool isNewRecord);
    void levelUp(int newLevel);

protected:
    void drawBackground(QPainter* painter, const QRectF& rect) override;

private:
    SurvivalPlayer*  m_player = nullptr;
    WaveManager*     m_waveManager = nullptr;
    UpgradeUI*       m_upgradeUI = nullptr;
    SurvivalHUD*     m_hud = nullptr;
    SurvivalStats*   m_stats = nullptr;
    QTimer*          m_gameTimer = nullptr;

    std::unique_ptr<pixel_hero::survival::EffectManager> m_effectManager;
    std::unique_ptr<pixel_hero::survival::EnemyManager>   m_enemyManager;
    bool m_isPaused = false;
    bool m_isStarted = false;

    void initScene();
    void updateGame();
    void onSkillSelected(const QString& skillId);
    void onSkillSkipped();
    void checkPlayerDeath();
    void autoAttackNearestEnemy();
    void autoCastSkills();
};

#endif // SURVIVALSCENE_H
