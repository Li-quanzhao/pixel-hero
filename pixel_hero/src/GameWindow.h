#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QShowEvent>
#include "ui/Menu.h"
#include "ui/CharacterSelectUI.h"
#include "ui/WeaponSelectUI.h"
#include "ui/SaveLoadUI.h"
#include "survival/SurvivalScene.h"
#include "survival/SurvivalStats.h"
#include "survival/SurvivalSaveData.h"
#include "entities/Weapon.h"

enum class GameState {
    MainMenu,
    CharacterSelect,
    WeaponSelect,
    SaveLoad,
    Playing,
    Paused,
    GameOver
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
    QGraphicsView*      m_view;
    SurvivalScene*      m_survivalScene;
    Menu*               m_menu;
    CharacterSelectUI*  m_charSelectUI;
    WeaponSelectUI*     m_wpnSelectUI;
    SaveLoadUI*         m_saveLoadUI;
    SurvivalStats*      m_stats;
    GameState           m_state;
    QString             m_selectedCharacter;
    QString             m_selectedWeapon;

    void initUI();
    void setupConnections();
    void showEvent(QShowEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

    // 菜单
    void showMainMenu();
    void showPauseMenu();
    void showGameOverMenu(int wave, int kills, float time, bool isNewRecord);
    void hideMenu();
    void hideCharSelectUI();
    void hideWeaponSelectUI();
    void hideSaveLoadUI();

    // 选择流程
    void showCharacterSelect();
    void showWeaponSelect();

    // 存档界面
    void showSaveLoadUI(SaveLoadUI::Mode mode);

    // 游戏流程
    void startNewGame();
    void resumeGame();
    void pauseGame();
    void gameOver(int wave, int kills, float time, bool isNewRecord);

    // 存档
    void saveCurrentGame(int slot);
    void loadSavedGame(int slot);
    void resumeSavedGame(int slot);
};

#endif // GAMEWINDOW_H
