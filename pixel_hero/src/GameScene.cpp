#include "GameScene.h"
#include "Weapon.h"
#include "Armor.h"
#include <QPainter>
#include <QDebug>
#include <cmath>

GameScene::GameScene(QObject *parent)
    : QGraphicsScene(parent)
    , m_battleSystem(nullptr)
    , m_isPaused(false)
    , m_isStarted(false)
{
    initScene();
}

GameScene::~GameScene()
{
    if (m_gameTimer) m_gameTimer->stop();
    delete m_battleSystem;
    delete m_player;
    delete m_mapManager;
}

void GameScene::initScene()
{
    setSceneRect(0, 0, 800, 600);

    m_mapManager = new MapManager(this);
    m_player = new Player(nullptr);
    m_battleSystem = new BattleSystem(this);

    m_gameTimer = new QTimer(this);
    connect(m_gameTimer, &QTimer::timeout, this, [this]() {
        if (!m_isPaused && m_isStarted) updateGame(1.0 / 60.0);
    });
}

void GameScene::startGame()
{
    m_mapManager->loadMap("level_1");
    m_mapManager->renderMap(this);

    QPointF spawnPoint = m_mapManager->getPlayerSpawnPoint();
    m_player->setPos(spawnPoint);
    addItem(m_player);
    m_player->setZValue(10);

    // 给玩家一些初始道具
    Weapon* starterWeapon = new Weapon("sword_001", "铁剑");
    starterWeapon->setAttackBonus(8);
    m_player->addItemToInventory(starterWeapon);

    Armor* starterArmor = new Armor("armor_001", "皮甲");
    starterArmor->setDefenseBonus(5);
    m_player->addItemToInventory(starterArmor);

    Item* potion = new Item("potion_hp", "生命药水", Item::CONSUMABLE);
    potion->setHealAmount(50);
    potion->setPrice(30);
    potion->setDescription("恢复50点生命值");
    m_player->addItemToInventory(potion);

    Item* potion2 = new Item("potion_hp", "生命药水", Item::CONSUMABLE);
    potion2->setHealAmount(50);
    potion2->setPrice(30);
    potion2->setDescription("恢复50点生命值");
    m_player->addItemToInventory(potion2);

    m_isPaused = false;
    m_isStarted = true;
    if (!m_gameTimer->isActive()) m_gameTimer->start(16);
}

void GameScene::pauseGame()  { m_isPaused = true; }
void GameScene::resumeGame() { m_isPaused = false; }

void GameScene::gameOver()
{
    m_gameTimer->stop();
    m_isPaused = true;
    m_isStarted = false;
}

Player* GameScene::getPlayer() const { return m_player; }
MapManager* GameScene::getMapManager() const { return m_mapManager; }
QList<Enemy*> GameScene::getEnemies() const { return m_mapManager->getEnemies(); }

void GameScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    painter->fillRect(rect, QColor(0x0f, 0x0f, 0x23));
}

void GameScene::updateGame(qreal deltaTime)
{
    // 1. 玩家移动 + 碰撞检测
    Player::MoveState& ms = m_player->moveState();
    qreal speed = m_player->speed() * deltaTime * 60;
    qreal dx = 0, dy = 0;

    if (ms.moveUp)    dy -= speed;
    if (ms.moveDown)  dy += speed;
    if (ms.moveLeft)  dx -= speed;
    if (ms.moveRight) dx += speed;

    if (dx != 0 || dy != 0) {
        QRectF playerRect(0, 0, 32, 24);
        playerRect.moveCenter(m_player->pos());

        QRectF testX = playerRect.translated(dx, 0);
        if (!m_mapManager->checkCollision(testX))
            m_player->setX(m_player->x() + dx);

        QRectF testY = playerRect.translated(0, dy);
        if (!m_mapManager->checkCollision(testY))
            m_player->setY(m_player->y() + dy);
    }

    // 2. 玩家攻击冷却
    m_player->update(deltaTime);

    // 3. 敌人AI
    QList<Enemy*> enemies = m_mapManager->getEnemies();
    for (Enemy* enemy : enemies) {
        if (enemy->isAlive()) enemy->updateAI(m_player, deltaTime);
    }

    // 4. 检查敌人交互：Space攻击范围内敌人
    if (m_player->moveState().moveUp || m_player->moveState().moveDown ||
        m_player->moveState().moveLeft || m_player->moveState().moveRight) {
        // 移动时自动攻击旁边的敌人
        for (Enemy* enemy : enemies) {
            if (!enemy->isAlive()) continue;
            QRectF playerRect(0, 0, 40, 40);
            playerRect.moveCenter(m_player->pos());
            QRectF enemyRect(0, 0, 40, 40);
            enemyRect.moveCenter(enemy->pos());
            enemyRect.adjust(-5, -5, 5, 5);
            if (playerRect.intersects(enemyRect)) {
                m_player->attackEnemy(enemy);
                if (!enemy->isAlive()) {
                    cleanupBattle();
                }
                break;
            }
        }
    }

    // 5. 玩家死亡检测
    if (m_player->health() <= 0) {
        emit playerDied();
    }
}

void GameScene::tryInteractNPC()
{
    QList<NPC*> npcs = m_mapManager->getNPCs();
    QRectF playerRect(0, 0, 32, 30);
    playerRect.moveCenter(m_player->pos());

    for (NPC* npc : npcs) {
        QRectF npcRect(0, 0, 20, 30);
        npcRect.moveCenter(npc->pos());
        npcRect.adjust(-10, -10, 10, 10);
        if (playerRect.intersects(npcRect)) {
            emit npcInteracted(npc);
            return;  // 只交互最近的NPC
        }
    }
}

void GameScene::cleanupBattle()
{
    QList<Enemy*> enemies = m_mapManager->getEnemies();
    for (Enemy* enemy : enemies) {
        if (!enemy->isAlive() && enemy->isVisible()) {
            m_player->addExp(enemy->expReward());
            m_player->setGold(m_player->gold() + enemy->goldReward());
            enemy->hide();
        }
    }
}
