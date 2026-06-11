#ifndef SURVIVALSCENE_H
#define SURVIVALSCENE_H

#include <QGraphicsScene>
#include <QTimer>
#include "SurvivalPlayer.h"
#include "WaveManager.h"
#include "UpgradeUI.h"
#include "SurvivalHUD.h"

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
    int totalKills() const { return m_totalKills; }
    float elapsedTime() const;

signals:
    void gameFinished(int wave, int kills, float time, bool isNewRecord);
    void levelUp(int newLevel);

protected:
    void drawBackground(QPainter* painter, const QRectF& rect) override;

private:
    SurvivalPlayer*  m_player;
    WaveManager*     m_waveManager;
    UpgradeUI*       m_upgradeUI;
    SurvivalHUD*     m_hud;
    SurvivalStats*   m_stats;
    QTimer*          m_gameTimer;
    bool             m_isPaused;
    bool             m_isStarted;
    int              m_totalKills;

    void initScene();
    void updateGame();
    void cleanupDeadEnemies();
    void onSkillSelected(const QString& skillId);
    void onSkillSkipped();  // 跳过升级
    void checkPlayerDeath();
    void autoAttackNearestEnemy();
    void autoCastSkills();
    QList<class Enemy*> aliveEnemies() const;
    Enemy* findNearestEnemy() const;

    // 战斗特效
    void showSlashEffect(QPointF pos);
    void showFireballEffect(QPointF from, QPointF to);
    void showLightningEffect(QPointF from, QPointF to);
    void showFrostNovaEffect(QPointF center, float radius);
};

#endif // SURVIVALSCENE_H
