#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QGraphicsScene>
#include <QTimer>
#include "Player.h"
#include "MapManager.h"

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

    Player* getPlayer() const;
    MapManager* getMapManager() const;

protected:
    void drawBackground(QPainter *painter, const QRectF &rect) override;

private:
    Player *m_player;
    MapManager *m_mapManager;
    QTimer *m_gameTimer;
    bool m_isPaused;

    void initScene();
    void updateGame(qreal deltaTime);
};

#endif // GAMESCENE_H