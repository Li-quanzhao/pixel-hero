#include "GameWindow.h"
#include "utils/GameData.h"
#include "survival/SurvivalStats.h"
#include "survival/UpgradeUI.h"
#include <QKeyEvent>
#include <QMessageBox>
#include <QPainter>
#include <QApplication>
#include <QDateTime>
#include <algorithm>
#include <random>

GameWindow::GameWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_menu(nullptr), m_charSelectUI(nullptr), m_wpnSelectUI(nullptr)
    , m_saveLoadUI(nullptr)
    , m_stats(new SurvivalStats(this))
    , m_state(GameState::MainMenu)
    , m_selectedCharacter("warrior"), m_selectedWeapon("short_sword")
{
    GameData::instance()->loadSkillData(":/resources/data/skills.json");
    GameData::instance()->loadWaveData(":/resources/data/survival_waves.json");
    GameData::instance()->loadWeaponData(":/resources/data/weapons.json");

    initUI();
    setupConnections();

    qApp->installEventFilter(this);
    setFocusPolicy(Qt::StrongFocus);
    showMainMenu();
}

GameWindow::~GameWindow()
{
    qApp->removeEventFilter(this);
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
    setWindowTitle("像素勇者 - 生存割草");
    setFixedSize(800, 600);
    setStyleSheet("background-color: #1a1a2e;");

    m_survivalScene = new SurvivalScene(this);
    m_survivalScene->setStats(m_stats);
    m_view = new QGraphicsView(m_survivalScene, this);
    m_view->setGeometry(0, 0, 800, 600);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setRenderHint(QPainter::SmoothPixmapTransform, false);
    m_view->setRenderHint(QPainter::Antialiasing, false);
    m_view->setFocusPolicy(Qt::NoFocus);
    setCentralWidget(m_view);
}

void GameWindow::setupConnections()
{
    connect(m_survivalScene, &SurvivalScene::gameFinished, this,
            [this](int wave, int kills, float time, bool isNewRecord) {
                gameOver(wave, kills, time, isNewRecord);
            });

    connect(m_survivalScene, &SurvivalScene::levelUp, this,
            [this](int newLevel) {
                Q_UNUSED(newLevel);
                m_survivalScene->pauseGame();
                QList<SkillData> allSkills = GameData::instance()->allSkills();
                QList<SkillData> options;
                SurvivalPlayer* sp = m_survivalScene->player();

                QList<SkillData> ownedOptions;
                for (const auto& as : sp->activeSkills()) {
                    const SkillData* sd = GameData::instance()->getSkillById(as.skillId);
                    if (sd && as.level < sd->maxLevel) {
                        SkillData opt = *sd;
                        opt.levels.clear();
                        opt.levels.append(sd->levels[qMin(as.level, sd->maxLevel-1)]);
                        ownedOptions.append(opt);
                    }
                }

                for (const auto& s : allSkills) {
                    if (sp->skillLevel(s.id) == 0) {
                        ownedOptions.append(s);
                    }
                }

                std::random_device rd;
                std::mt19937 g(rd());
                std::shuffle(ownedOptions.begin(), ownedOptions.end(), g);
                for (int i = 0; i < qMin(3, ownedOptions.size()); i++) {
                    options.append(ownedOptions[i]);
                }

                QList<QGraphicsItem*> items = m_survivalScene->items();
                for (auto* item : items) {
                    auto* ui = dynamic_cast<UpgradeUI*>(item);
                    if (ui) {
                        ui->showUpgrade(options);
                        return;
                    }
                }
            });
}

// ========================= 键盘 =========================
bool GameWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent*>(event);
        int key = ke->key();

        switch (m_state) {
        case GameState::MainMenu:
        case GameState::Paused:
        case GameState::GameOver:
            if (QApplication::activeModalWidget()) break; // 模态框激活时放过
            if (m_menu) {
                if (key == Qt::Key_Up    || key == Qt::Key_W)    { m_menu->selectPrevious();   return true; }
                if (key == Qt::Key_Down  || key == Qt::Key_S)    { m_menu->selectNext();       return true; }
                if (key == Qt::Key_Return || key == Qt::Key_Enter || key == Qt::Key_Space)
                                                                 { m_menu->confirmSelection(); return true; }
            }
            break;

        case GameState::CharacterSelect:
            if (m_charSelectUI) {
                if (key == Qt::Key_Left  || key == Qt::Key_A)    { m_charSelectUI->prev();    return true; }
                if (key == Qt::Key_Right || key == Qt::Key_D)    { m_charSelectUI->next();    return true; }
                if (key == Qt::Key_Return || key == Qt::Key_Enter || key == Qt::Key_Space)
                                                                 { m_charSelectUI->confirm(); return true; }
                if (key == Qt::Key_Escape || key == Qt::Key_Backspace)
                                                                 { hideCharSelectUI(); showMainMenu(); return true; }
            }
            break;

        case GameState::WeaponSelect:
            if (m_wpnSelectUI) {
                if (key == Qt::Key_Left  || key == Qt::Key_A)    { m_wpnSelectUI->prev();    return true; }
                if (key == Qt::Key_Right || key == Qt::Key_D)    { m_wpnSelectUI->next();    return true; }
                if (key == Qt::Key_Return || key == Qt::Key_Enter || key == Qt::Key_Space)
                                                                 { m_wpnSelectUI->confirm(); return true; }
                if (key == Qt::Key_Escape || key == Qt::Key_Backspace)
                                                                 { hideWeaponSelectUI(); showCharacterSelect(); return true; }
            }
            break;

        case GameState::SaveLoad:
            if (QApplication::activeModalWidget()) break;
            if (m_saveLoadUI) {
                if (key == Qt::Key_Up    || key == Qt::Key_W)    { m_saveLoadUI->prev();    return true; }
                if (key == Qt::Key_Down  || key == Qt::Key_S)    { m_saveLoadUI->next();    return true; }
                if (key == Qt::Key_Return || key == Qt::Key_Enter || key == Qt::Key_Space)
                                                                 { m_saveLoadUI->confirm(); return true; }
                if (key == Qt::Key_Delete)
                                                                 { m_saveLoadUI->del();     return true; }
                if (key == Qt::Key_Escape)
                                                                 { m_saveLoadUI->dismiss(); return true; }
            }
            break;

        case GameState::Playing:
            if (key == Qt::Key_Escape) {
                pauseGame(); return true;
            } else if (key == Qt::Key_W || key == Qt::Key_Up) {
                m_survivalScene->player()->setMoveUp(true);    return true;
            } else if (key == Qt::Key_S || key == Qt::Key_Down) {
                m_survivalScene->player()->setMoveDown(true);  return true;
            } else if (key == Qt::Key_A || key == Qt::Key_Left) {
                m_survivalScene->player()->setMoveLeft(true);  return true;
            } else if (key == Qt::Key_D || key == Qt::Key_Right) {
                m_survivalScene->player()->setMoveRight(true); return true;
            }
            break;
        }
    }
    else if (event->type() == QEvent::KeyRelease && m_state == GameState::Playing) {
        QKeyEvent *ke = static_cast<QKeyEvent*>(event);
        int key = ke->key();
        if (key == Qt::Key_W || key == Qt::Key_Up)
            { m_survivalScene->player()->setMoveUp(false);    return true; }
        else if (key == Qt::Key_S || key == Qt::Key_Down)
            { m_survivalScene->player()->setMoveDown(false);  return true; }
        else if (key == Qt::Key_A || key == Qt::Key_Left)
            { m_survivalScene->player()->setMoveLeft(false);  return true; }
        else if (key == Qt::Key_D || key == Qt::Key_Right)
            { m_survivalScene->player()->setMoveRight(false); return true; }
    }
    return QMainWindow::eventFilter(obj, event);
}

// ========================= 菜单 =========================
void GameWindow::showMainMenu()
{
    hideMenu();
    m_menu = new Menu(Menu::MAIN_MENU);
    m_survivalScene->addItem(m_menu);
    m_menu->show();
    m_state = GameState::MainMenu;
    m_view->viewport()->update();
    if (m_stats->allSavedSlots().isEmpty()) {
        m_menu->setItemDisabled(1, true);
    }

    connect(m_menu, &Menu::startGame, this, &GameWindow::startNewGame);
    connect(m_menu, &Menu::loadGame, this, [this]() {
        hideMenu();
        showSaveLoadUI(SaveLoadUI::LOAD_MODE);
    });
    connect(m_menu, &Menu::quitGame,  this, &QMainWindow::close);
    setFocus();
}

void GameWindow::showPauseMenu()
{
    hideMenu();
    m_menu = new Menu(Menu::PAUSE_MENU);
    m_survivalScene->addItem(m_menu);
    m_menu->show();
    m_state = GameState::Paused;

    connect(m_menu, &Menu::resumeGame, this, &GameWindow::resumeGame);
    connect(m_menu, &Menu::saveGame, this, [this]() {
        hideMenu();
        showSaveLoadUI(SaveLoadUI::SAVE_MODE);
    });
    connect(m_menu, &Menu::quitGame, this, [this]() { showMainMenu(); });
    setFocus();
}

void GameWindow::showGameOverMenu(int wave, int kills, float time, bool isNewRecord)
{
    hideMenu();
    m_state = GameState::GameOver;

    int sec = static_cast<int>(time);
    int m = sec / 60;
    int s = sec % 60;
    QString msg = QString("存活时间: %1:%2\n波次: %3\n击杀: %4")
        .arg(m, 2, 10, QChar('0')).arg(s, 2, 10, QChar('0'))
        .arg(wave).arg(kills);

    if (isNewRecord) msg += "\n\n新纪录！";
    msg += "\n战绩: 最高波次 " + QString::number(m_stats->recordWave());

    QMessageBox::information(this, "游戏结束", msg);
    showMainMenu();
}

void GameWindow::hideMenu()
{
    if (m_menu) {
        Menu* menu = m_menu;
        m_menu = nullptr;
        m_survivalScene->removeItem(menu);
        menu->deleteLater();
        m_view->viewport()->update();
    }
}

void GameWindow::hideCharSelectUI()
{
    if (m_charSelectUI) {
        CharacterSelectUI* ui = m_charSelectUI;
        m_charSelectUI = nullptr;
        m_survivalScene->removeItem(ui);
        ui->deleteLater();
        m_view->viewport()->update();
    }
}

void GameWindow::hideWeaponSelectUI()
{
    if (m_wpnSelectUI) {
        WeaponSelectUI* ui = m_wpnSelectUI;
        m_wpnSelectUI = nullptr;
        m_survivalScene->removeItem(ui);
        ui->deleteLater();
        m_view->viewport()->update();
    }
}

void GameWindow::hideSaveLoadUI()
{
    if (m_saveLoadUI) {
        SaveLoadUI* ui = m_saveLoadUI;
        m_saveLoadUI = nullptr;
        m_survivalScene->removeItem(ui);
        ui->deleteLater();
        m_view->viewport()->update();
    }
}

// ========================= 游戏流程 =========================
void GameWindow::startNewGame()
{
    hideMenu();
    showCharacterSelect();
}

void GameWindow::pauseGame()
{
    m_survivalScene->pauseGame();
    showPauseMenu();
}

void GameWindow::resumeGame()
{
    hideMenu();
    m_survivalScene->resumeGame();
    m_state = GameState::Playing;
    setFocus();
}

void GameWindow::gameOver(int wave, int kills, float time, bool isNewRecord)
{
    showGameOverMenu(wave, kills, time, isNewRecord);
}

// ========================= 选择流程 =========================
void GameWindow::showCharacterSelect()
{
    hideCharSelectUI();
    hideWeaponSelectUI();

    QList<CharacterSelectUI::CharacterInfo> infos;
    auto chars = SurvivalPlayer::availableCharacters();
    for (const auto& c : chars) {
        CharacterSelectUI::CharacterInfo info;
        info.id    = c.id;
        info.name  = c.name;
        info.hp    = c.hp;
        info.atk   = c.atk;
        info.def   = c.def;
        info.spd   = c.spd;
        info.color = c.color;
        info.desc  = c.desc;
        infos.append(info);
    }

    m_charSelectUI = new CharacterSelectUI(infos);
    m_survivalScene->addItem(m_charSelectUI);
    m_charSelectUI->appear();
    m_state = GameState::CharacterSelect;
    m_view->viewport()->update();
    setFocus();

    connect(m_charSelectUI, &CharacterSelectUI::characterSelected, this,
            [this](int idx) {
                m_selectedCharacter = SurvivalPlayer::availableCharacters()[idx].id;
                hideCharSelectUI();
                showWeaponSelect();
            });
}

void GameWindow::showWeaponSelect()
{
    hideWeaponSelectUI();

    auto weapons = GameData::instance()->weapons();

    m_wpnSelectUI = new WeaponSelectUI(weapons);
    m_survivalScene->addItem(m_wpnSelectUI);
    m_wpnSelectUI->appear();
    m_state = GameState::WeaponSelect;
    m_view->viewport()->update();
    setFocus();

    connect(m_wpnSelectUI, &WeaponSelectUI::weaponSelected, this,
            [this](int idx) {
                m_selectedWeapon = GameData::instance()->weapons()[idx].id;
                hideWeaponSelectUI();
                m_state = GameState::Playing;
                m_survivalScene->startGame(m_selectedCharacter, m_selectedWeapon);
                setFocus();
            });
}

// ========================= 存档界面 =========================
void GameWindow::showSaveLoadUI(SaveLoadUI::Mode mode)
{
    hideSaveLoadUI();
    hideMenu();

    // 准备5个槽的预览数据
    QList<SaveLoadUI::SlotPreview> previews;
    for (int i = 0; i < 5; ++i) {
        SaveLoadUI::SlotPreview sp;
        if (m_stats->hasSavedGame(i)) {
            sp.occupied = true;
            m_stats->loadSavedGame(sp.data, i);
        } else {
            sp.occupied = false;
        }
        previews.append(sp);
    }

    m_saveLoadUI = new SaveLoadUI(mode, previews);
    m_survivalScene->addItem(m_saveLoadUI);
    m_saveLoadUI->appear();
    m_state = GameState::SaveLoad;
    m_view->viewport()->update();
    setFocus();

    // 保存请求
    connect(m_saveLoadUI, &SaveLoadUI::saveRequested, this,
            [this](int slot) {
                // 如果槽位已有存档，覆盖确认
                if (m_stats->hasSavedGame(slot)) {
                    auto result = QMessageBox::question(this, "覆盖确认",
                        QString("存档槽 #%1 已有存档，是否覆盖？").arg(slot + 1),
                        QMessageBox::Yes | QMessageBox::No);
                    if (result != QMessageBox::Yes) return;
                }
                saveCurrentGame(slot);
                hideSaveLoadUI();
                showMainMenu();
            });

    // 读取请求
    connect(m_saveLoadUI, &SaveLoadUI::loadRequested, this,
            [this](int slot) {
                hideSaveLoadUI();
                loadSavedGame(slot);
            });

    // 删除请求
    connect(m_saveLoadUI, &SaveLoadUI::deleteRequested, this,
            [this](int slot) {
                auto result = QMessageBox::question(this, "删除确认",
                    QString("确定删除存档槽 #%1？此操作不可撤销。").arg(slot + 1),
                    QMessageBox::Yes | QMessageBox::No);
                if (result != QMessageBox::Yes) return;
                m_stats->deleteSavedGame(slot);
                // 刷新界面
                hideSaveLoadUI();
                showSaveLoadUI(SaveLoadUI::LOAD_MODE);
            });

    // 取消 — 保存模式回到暂停菜单，读档模式回到主菜单
    connect(m_saveLoadUI, &SaveLoadUI::cancelled, this,
            [this, mode]() {
                hideSaveLoadUI();
                if (mode == SaveLoadUI::SAVE_MODE)
                    showPauseMenu();
                else
                    showMainMenu();
            });
}

// ========================= 存档操作 =========================
void GameWindow::saveCurrentGame(int slot)
{
    SurvivalSaveData data;
    data.currentWave  = m_survivalScene->waveManager()->currentWave();
    data.totalKills   = m_survivalScene->totalKills();
    data.elapsedTime  = m_survivalScene->elapsedTime();
    data.playerLevel  = m_survivalScene->player()->level();
    data.playerHealth = m_survivalScene->player()->health();
    data.playerMaxHealth = m_survivalScene->player()->maxHealth();
    data.playerExp    = m_survivalScene->player()->exp();
    data.characterId  = m_selectedCharacter;
    data.weaponId     = m_selectedWeapon;
    data.savedAt      = QDateTime::currentDateTime();

    for (const auto& as : m_survivalScene->player()->activeSkills()) {
        data.skills.append({as.skillId, as.level});
    }

    m_stats->saveCurrentGame(data, slot);
    setFocus();
}

void GameWindow::loadSavedGame(int slot)
{
    resumeSavedGame(slot);
}

void GameWindow::resumeSavedGame(int slot)
{
    SurvivalSaveData data;
    if (!m_stats->loadSavedGame(data, slot)) {
        QMessageBox::information(this, "读档", "读取存档失败！");
        showMainMenu();
        return;
    }
    m_selectedCharacter = data.characterId;
    m_selectedWeapon    = data.weaponId;
    m_survivalScene->startFromSave(data);
    m_state = GameState::Playing;
    setFocus();
}
