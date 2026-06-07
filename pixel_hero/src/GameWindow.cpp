#include "GameWindow.h"
#include "GameData.h"
#include "SaveManager.h"
#include "Weapon.h"
#include "Armor.h"
#include <QKeyEvent>
#include <QMessageBox>
#include <QApplication>
#include <QPainter>
#include <QInputDialog>

GameWindow::GameWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_menu(nullptr), m_hud(nullptr)
    , m_inventory(nullptr)
    , m_state(GameState::MainMenu)
{
    GameData::instance()->loadLevelData(":/resources/data/levels.json");
    GameData::instance()->loadItemData(":/resources/data/items.json");

    initUI();
    initHUD();
    setupConnections();

    qApp->installEventFilter(this);
    setFocusPolicy(Qt::StrongFocus);
    showMainMenu();
}

GameWindow::~GameWindow()
{
    qApp->removeEventFilter(this);
    delete m_controller; delete m_scene; delete m_view;
}

GameState GameWindow::currentState() const { return m_state; }
void GameWindow::setState(GameState state) { m_state = state; }

void GameWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    setFocus(); activateWindow();
}

void GameWindow::initUI()
{
    setWindowTitle("像素勇者 - Pixel Hero Adventure");
    setFixedSize(800, 600);
    setStyleSheet("background-color: #1a1a2e;");

    m_scene = new GameScene(this);
    m_view = new QGraphicsView(m_scene, this);
    m_view->setGeometry(0, 0, 800, 600);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setRenderHint(QPainter::SmoothPixmapTransform, false);
    m_view->setRenderHint(QPainter::Antialiasing, false);
    m_view->setFocusPolicy(Qt::NoFocus);
    m_view->viewport()->setFocusPolicy(Qt::NoFocus);
    m_controller = new GameController(m_scene, this);
    setCentralWidget(m_view);
}

void GameWindow::initHUD()
{
    m_hud = new HUD();
    m_hud->setZValue(100);
    m_scene->addItem(m_hud);
    m_hud->hide();

    m_inventory = new Inventory();
    m_inventory->setZValue(200);
    m_scene->addItem(m_inventory);
    m_inventory->hide();

    connect(m_inventory, &Inventory::closed, this, [this]() {
        if (m_state == GameState::Inventory) {
            m_state = GameState::Playing;
            if (m_hud) m_hud->show();
            setFocus();
        }
    });
}

void GameWindow::setupConnections()
{
    connect(m_controller, &GameController::gamePaused,  this, [this]() { pauseGame(); });
    connect(m_controller, &GameController::gameResumed, this, [this]() { resumeGame(); });
    connect(m_controller, &GameController::gameOvered,  this, [this]() { gameOver(); });
    connect(m_scene, &GameScene::gameFinished,           this, [this]() { gameOver(); });

    connect(m_scene, &GameScene::npcInteracted, this, [this](NPC* npc) {
        handleNPCInteraction(npc);
    });

    connect(m_scene, &GameScene::playerDied, this, [this]() { gameOver(); });
}

// ========================= 键盘 =========================
bool GameWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent*>(event);

        switch (m_state) {
        case GameState::MainMenu:
        case GameState::Paused:
        case GameState::GameOver:
            if (m_menu) {
                int k = ke->key();
                if (k == Qt::Key_Up    || k == Qt::Key_W) m_menu->selectPrevious();
                if (k == Qt::Key_Down  || k == Qt::Key_S) m_menu->selectNext();
                if (k == Qt::Key_Return || k == Qt::Key_Enter || k == Qt::Key_Space)
                    m_menu->confirmSelection();
            }
            break;

        case GameState::Playing:
            if (ke->key() == Qt::Key_Escape)
                m_controller->pauseGame();
            else if (ke->key() == Qt::Key_I) {
                openInventory();
            } else
                m_controller->handleKeyPress(ke);
            break;

        case GameState::Inventory:
            if (m_inventory) {
                if (ke->key() == Qt::Key_Escape || ke->key() == Qt::Key_I)
                    closeInventory();
                else if (ke->key() == Qt::Key_Up || ke->key() == Qt::Key_W)
                    m_inventory->selectPrevious();
                else if (ke->key() == Qt::Key_Down || ke->key() == Qt::Key_S)
                    m_inventory->selectNext();
                else if (ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Enter || ke->key() == Qt::Key_Space)
                    m_inventory->confirmItem();
            }
            break;
        }
    }
    else if (event->type() == QEvent::KeyRelease && m_state == GameState::Playing) {
        m_controller->handleKeyRelease(static_cast<QKeyEvent*>(event));
    }
    return QMainWindow::eventFilter(obj, event);
}

// ========================= NPC交互 =========================
void GameWindow::handleNPCInteraction(NPC* npc)
{
    switch (npc->npcType()) {
    case NPC::SHOPKEEPER:
        openShop();
        break;
    case NPC::QUEST_GIVER:
        openQuestDialog(npc);
        break;
    default:
        npc->interact();
        QMessageBox::information(this, npc->name(),
            npc->name() + "：「" + npc->getCurrentDialogue() + "」");
        setFocus();
        break;
    }
}

void GameWindow::openShop()
{
    // 简单商店：从 items.json 读取前几个可购买的物品
    auto itemDataList = GameData::instance()->items();
    QStringList itemNames;
    for (const auto& d : itemDataList) {
        if (d.price > 0) itemNames << QString("%1 (%2金币)").arg(d.name).arg(d.price);
    }

    if (itemNames.isEmpty()) {
        QMessageBox::information(this, "商店", "暂时没有可购买的物品");
        setFocus();
        return;
    }

    bool ok;
    QString choice = QInputDialog::getItem(this, "商店 - 购买物品",
        "选择要购买的物品:", itemNames, 0, false, &ok);

    if (ok && !choice.isEmpty()) {
        int idx = itemNames.indexOf(choice);
        if (idx >= 0 && idx < itemDataList.size()) {
            auto& d = itemDataList[idx];
            Player* player = m_scene->getPlayer();
            if (!player) return;

            if (player->gold() < d.price) {
                QMessageBox::warning(this, "商店", "金币不足！");
            } else {
                player->setGold(player->gold() - d.price);
                if (d.type == "weapon")
                    player->addItemToInventory(new Weapon(d.id, d.name));
                else if (d.type == "armor")
                    player->addItemToInventory(new Armor(d.id, d.name));
                else
                    player->addItemToInventory(new Item(d.id, d.name, Item::CONSUMABLE));

                if (m_inventory) m_inventory->refresh();
                QMessageBox::information(this, "商店", "购买成功！获得 " + d.name);
            }
        }
    }
    setFocus();
}

void GameWindow::openQuestDialog(NPC* npc)
{
    npc->interact();
    QString text = npc->name() + "：「" + npc->getCurrentDialogue() + "」\n\n接受此任务？";
    int ret = QMessageBox::question(this, "任务", text,
        QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        // 简单奖励
        Player* player = m_scene->getPlayer();
        if (player) {
            player->addExp(50);
            player->setGold(player->gold() + 30);
            QMessageBox::information(this, "任务完成", "获得经验50，金币30！");
        }
    }
    setFocus();
}

void GameWindow::openInventory()
{
    if (!m_inventory) return;
    m_state = GameState::Inventory;
    m_inventory->setPlayer(m_scene->getPlayer());
    m_inventory->show();
    m_hud->hide();
}

void GameWindow::closeInventory()
{
    if (m_inventory) m_inventory->hide();
    m_state = GameState::Playing;
    m_hud->show();
    setFocus();
}

// ========================= 菜单 =========================
void GameWindow::showMainMenu()
{
    hideMenu();
    m_menu = new Menu(Menu::MAIN_MENU);
    m_menu->setPos(300, 200);
    m_scene->addItem(m_menu);
    m_menu->show();
    m_state = GameState::MainMenu;
    m_hud->hide();
    if (m_inventory) m_inventory->hide();

    connect(m_menu, &Menu::startGame, this, &GameWindow::startNewGame);
    connect(m_menu, &Menu::loadGame, this, [this]() { 
        loadSavedGame();
    });
    connect(m_menu, &Menu::quitGame, this, &QMainWindow::close);
    setFocus();
}

void GameWindow::showPauseMenu()
{
    hideMenu();
    m_menu = new Menu(Menu::PAUSE_MENU);
    m_menu->setPos(300, 200);
    m_scene->addItem(m_menu);
    m_menu->show();
    m_state = GameState::Paused;

    connect(m_menu, &Menu::resumeGame, this, &GameWindow::resumeGame);
    connect(m_menu, &Menu::saveGame, this, [this]() { saveCurrentGame(); });
    connect(m_menu, &Menu::quitGame, this, [this]() { showMainMenu(); });
    setFocus();
}

void GameWindow::showGameOverMenu()
{
    hideMenu();
    m_menu = new Menu(Menu::GAME_OVER_MENU);
    m_menu->setPos(300, 200);
    m_scene->addItem(m_menu);
    m_menu->show();
    m_state = GameState::GameOver;

    connect(m_menu, &Menu::startGame, this, &GameWindow::startNewGame);
    connect(m_menu, &Menu::quitGame, this, [this]() { showMainMenu(); });
    setFocus();
}

void GameWindow::hideMenu()
{
    if (m_menu) {
        Menu* menu = m_menu;
        m_menu = nullptr;
        m_scene->removeItem(menu);
        menu->deleteLater();
    }
}

// ========================= 存档 =========================
void GameWindow::saveCurrentGame()
{
    Player* player = m_scene->getPlayer();
    if (!player) return;

    SaveData data;
    data.playerName = "勇者";
    data.level = player->level();
    data.exp = player->exp();
    data.health = player->health();
    data.maxHealth = player->maxHealth();
    data.attack = player->attack();
    data.defense = player->defense();
    data.gold = player->gold();
    data.currentMap = "level_1";
    data.posX = static_cast<int>(player->pos().x());
    data.posY = static_cast<int>(player->pos().y());

    for (Item* item : player->inventory()) {
        data.inventoryIds << item->id();
        if (item->type() == Item::WEAPON)
            data.weaponId = item->id();
        else if (item->type() == Item::ARMOR)
            data.armorId = item->id();
    }

    if (SaveManager::instance()->saveGame(data)) {
        QMessageBox::information(this, "存档", "游戏已保存！");
    } else {
        QMessageBox::warning(this, "存档", "存档失败！");
    }
    setFocus();
}

void GameWindow::loadSavedGame()
{
    SaveData data;
    if (!SaveManager::instance()->loadGame(data)) {
        QMessageBox::information(this, "读档", "没有找到存档文件");
        setFocus();
        return;
    }

    hideMenu();
    m_state = GameState::Playing;
    m_scene->startGame();
    m_controller->startGame();

    Player* player = m_scene->getPlayer();
    if (player) {
        player->applySaveData(data.level, data.exp, data.health,
            data.maxHealth, data.attack, data.defense, data.gold);
        player->setPos(data.posX, data.posY);

        // 恢复装备
        if (!data.weaponId.isEmpty()) {
            Weapon* w = new Weapon(data.weaponId, data.weaponId);
            player->equipWeapon(w);
        }
        if (!data.armorId.isEmpty()) {
            Armor* a = new Armor(data.armorId, data.armorId);
            player->equipArmor(a);
        }
    }

    m_hud->show();
    if (player) m_hud->setPlayer(player);
    if (m_inventory) m_inventory->setPlayer(player);
    setFocus(); activateWindow();

    QMessageBox::information(this, "读档", "存档已加载！");
    setFocus();
}

// ========================= 游戏流程 =========================
void GameWindow::startNewGame()
{
    hideMenu();
    m_state = GameState::Playing;
    m_scene->startGame();
    m_controller->startGame();
    m_hud->show();

    Player* player = m_scene->getPlayer();
    if (player) {
        m_hud->setPlayer(player);
        m_inventory->setPlayer(player);
    }
    setFocus(); activateWindow();
}

void GameWindow::resumeGame()
{
    hideMenu();
    m_state = GameState::Playing;
    m_scene->resumeGame();
    m_controller->resumeGame();
    m_hud->show();
    setFocus(); activateWindow();
}

void GameWindow::pauseGame()
{
    if (m_state == GameState::Playing) {
        m_scene->pauseGame();
        showPauseMenu();
        m_hud->hide();
    }
}

void GameWindow::gameOver()
{
    if (m_state == GameState::Playing) {
        m_scene->gameOver();
        showGameOverMenu();
        m_hud->hide();
    }
}
