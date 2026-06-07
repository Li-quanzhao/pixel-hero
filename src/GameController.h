#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <QObject>
#include <QKeyEvent>
#include <QMouseEvent>
#include "GameScene.h"

class GameController : public QObject
{
    Q_OBJECT

public:
    GameController(GameScene *scene, QObject *parent = nullptr);
    ~GameController();

    void startGame();
    void pauseGame();
    void resumeGame();
    void gameOver();

    void handleKeyPress(QKeyEvent *event);
    void handleKeyRelease(QKeyEvent *event);
    void handleMouseClick(QMouseEvent *event);

    void saveGame();
    void loadGame();

signals:
    void gameStarted();
    void gamePaused();
    void gameResumed();
    void gameOvered();

private:
    GameScene *m_scene;
    bool m_isRunning;
    bool m_isPaused;

    void processPlayerMovement(QKeyEvent *event, bool isPressed);
};

#endif // GAMECONTROLLER_H