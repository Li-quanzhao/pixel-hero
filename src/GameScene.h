#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QGraphicsScene>
#include <QTimer>
#include <QList>
#include "Player.h"
#include "MapManager.h"
#include "Enemy.h"
#include "NPC.h"
#include "BattleSystem.h"

class GameScene : public QGraphicsScene
{
    Q_OBJECT

public:
    GameScene(QObject *parent = nullptr);
    ~GameScene();

    void startGame();
    void pauseGame();
    void resumeGame();
    void gameOver();
    void tryInteractNPC();

    Player* getPlayer() const;
    MapManager* getMapManager() const;
    QList<Enemy*> getEnemies() const;

signals:
    void gameFinished();
    void npcInteracted(NPC* npc);
    void playerDied();

protected:
    void drawBackground(QPainter *painter, const QRectF &rect) override;

private:
    Player *m_player;
    MapManager *m_mapManager;
    BattleSystem *m_battleSystem;
    QTimer *m_gameTimer;
    bool m_isPaused;
    bool m_isStarted;

    void initScene();
    void updateGame(qreal deltaTime);
    void cleanupBattle();
};

#endif // GAMESCENE_H
