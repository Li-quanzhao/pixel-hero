#include "GameController.h"

GameController::GameController(GameScene *scene, QObject *parent)
    : QObject(parent), m_scene(scene), m_isRunning(false), m_isPaused(false)
{
}

GameController::~GameController()
{
}

void GameController::startGame()
{
    m_isRunning = true;
    m_isPaused = false;
    emit gameStarted();
}

void GameController::pauseGame()
{
    if (m_isRunning && !m_isPaused) {
        m_isPaused = true;
        emit gamePaused();
    }
}

void GameController::resumeGame()
{
    if (m_isRunning && m_isPaused) {
        m_isPaused = false;
        emit gameResumed();
    }
}

void GameController::gameOver()
{
    m_isRunning = false;
    emit gameOvered();
}

void GameController::handleKeyPress(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        if (m_isPaused) {
            resumeGame();
        } else {
            pauseGame();
        }
        return;
    }

    if (!m_isPaused && m_isRunning) {
        processPlayerMovement(event, true);
    }
}

void GameController::handleKeyRelease(QKeyEvent *event)
{
    if (!m_isPaused && m_isRunning) {
        processPlayerMovement(event, false);
    }
}

void GameController::handleMouseClick(QMouseEvent *event)
{
    Q_UNUSED(event);
}

void GameController::saveGame()
{
}

void GameController::loadGame()
{
}

void GameController::processPlayerMovement(QKeyEvent *event, bool isPressed)
{
    Player *player = m_scene->getPlayer();
    if (!player) return;

    switch (event->key()) {
    case Qt::Key_W:
    case Qt::Key_Up:
        player->setMoveUp(isPressed);
        break;
    case Qt::Key_S:
    case Qt::Key_Down:
        player->setMoveDown(isPressed);
        break;
    case Qt::Key_A:
    case Qt::Key_Left:
        player->setMoveLeft(isPressed);
        break;
    case Qt::Key_D:
    case Qt::Key_Right:
        player->setMoveRight(isPressed);
        break;
    case Qt::Key_Space:
        if (isPressed) {
            player->attack();
        }
        break;
    default:
        break;
    }
}