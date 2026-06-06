#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QShowEvent>
#include "GameScene.h"
#include "GameController.h"
#include "ui/Menu.h"
#include "ui/HUD.h"
#include "ui/Inventory.h"

enum class GameState {
    MainMenu,
    Playing,
    Paused,
    GameOver,
    Inventory
};

class GameWindow : public QMainWindow
{
    Q_OBJECT

public:
    GameWindow(QWidget *parent = nullptr);
    ~GameWindow();
    GameState currentState() const;
    void setState(GameState state);

private:
    QGraphicsView *m_view;
    GameScene *m_scene;
    GameController *m_controller;
    Menu *m_menu;
    HUD *m_hud;
    Inventory *m_inventory;
    GameState m_state;

    void initUI();
    void initHUD();
    void setupConnections();
    void showEvent(QShowEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

    // 菜单
    void showMainMenu();
    void showPauseMenu();
    void showGameOverMenu();
    void hideMenu();

    // 游戏流程
    void startNewGame();
    void resumeGame();
    void pauseGame();
    void gameOver();

    // 存档
    void saveCurrentGame();
    void loadSavedGame();

    // 背包
    void openInventory();
    void closeInventory();

    // NPC
    void handleNPCInteraction(NPC* npc);
    void openShop();
    void openQuestDialog(NPC* npc);
};

#endif // GAMEWINDOW_H
