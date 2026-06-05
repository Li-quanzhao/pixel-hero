#include "GameScene.h"
#include <QPainter>

GameScene::GameScene(QObject *parent)
    : QGraphicsScene(parent), m_isPaused(false)
{
    initScene();
}

GameScene::~GameScene()
{
    delete m_gameTimer;
    delete m_player;
    delete m_mapManager;
}

void GameScene::initScene()
{
    setSceneRect(0, 0, 800, 600);

    m_mapManager = new MapManager(this);
    m_player = new Player(nullptr);

    m_gameTimer = new QTimer(this);
    connect(m_gameTimer, &QTimer::timeout, this, [this]() {
        if (!m_isPaused) {
            updateGame(1.0 / 60.0);
        }
    });
    m_gameTimer->start(16);
}

void GameScene::startGame()
{
    m_mapManager->loadMap("level_1");
    m_mapManager->renderMap(this);
    QPointF spawnPoint = m_mapManager->getPlayerSpawnPoint();
    m_player->setPos(spawnPoint);
    addItem(m_player);
    m_isPaused = false;
}

void GameScene::pauseGame()
{
    m_isPaused = true;
}

void GameScene::resumeGame()
{
    m_isPaused = false;
}

void GameScene::gameOver()
{
    m_gameTimer->stop();
    m_isPaused = true;
}

Player* GameScene::getPlayer() const
{
    return m_player;
}

MapManager* GameScene::getMapManager() const
{
    return m_mapManager;
}

void GameScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    painter->fillRect(rect, QColor(0x1a, 0x1a, 0x2e));
}

void GameScene::updateGame(qreal deltaTime)
{
    m_player->update(deltaTime);
    m_mapManager->update(deltaTime);
}