#include "SurvivalScene.h"
#include "SurvivalStats.h"
#include "entities/Enemy.h"
#include "utils/GameData.h"
#include "config/GameConfig.h"
#include "survival/skills/SkillExecutor.h"
#include "PickupManager.h"
#include <QPainter>
#include <QRandomGenerator>
#include <QLineF>
#include <QGraphicsScene>
#include <algorithm>

SurvivalScene::SurvivalScene(QObject* parent)
    : QGraphicsScene(parent)
    , m_gameTimer(new QTimer(this))
{
    setSceneRect(pixel_hero::config::SCENE_RECT);
    m_gameTimer->setInterval(pixel_hero::config::GAME_TICK_MS);
    connect(m_gameTimer, &QTimer::timeout, this, &SurvivalScene::updateGame);
}

SurvivalScene::~SurvivalScene() = default;

void SurvivalScene::initScene()
{
    clear();

    setBackgroundBrush(QColor(20, 20, 35));

    // 玩家
    m_player = new SurvivalPlayer();
    m_player->setPos(400, 300);
    addItem(m_player);

    // 波次管理器
    m_waveManager = new WaveManager(this, this);

    // 特效管理器 (必须在EnemyManager之前创建)
    m_effectManager = std::make_unique<pixel_hero::survival::EffectManager>(this, this);

    // 敌人管理器
    m_enemyManager = std::make_unique<pixel_hero::survival::EnemyManager>(
        this, m_waveManager, m_effectManager.get(), this);
    if (m_stats) m_enemyManager->setStats(m_stats);

    // 拾取物管理器
    m_pickupManager = std::make_unique<pixel_hero::survival::PickupManager>(this, this);
    m_enemyManager->setPickupManager(m_pickupManager.get());

    // HUD
    m_hud = new SurvivalHUD();
    m_hud->bind(m_player, m_waveManager);
    addItem(m_hud);

    // 升级UI
    m_upgradeUI = new UpgradeUI();
    addItem(m_upgradeUI);

    connect(m_upgradeUI, &UpgradeUI::skillSelected, this, &SurvivalScene::onSkillSelected);
    connect(m_upgradeUI, &UpgradeUI::skillSkipped, this, &SurvivalScene::onSkillSkipped);
}

void SurvivalScene::startGame()
{
    startGame("warrior", "short_sword");
}

void SurvivalScene::startGame(const QString& characterId, const QString& weaponId)
{
    initScene();

    auto chars = SurvivalPlayer::availableCharacters();
    for (const auto& c : chars) {
        if (c.id == characterId) {
            m_player->applyCharacter(c);
            break;
        }
    }

    const WeaponData* wpn = GameData::instance()->getWeaponById(weaponId);
    if (wpn) m_player->applyWeapon(*wpn);

    m_waveManager->start();
    m_gameTimer->start();
    m_isStarted = true;
    m_isPaused = false;
}

void SurvivalScene::startFromSave(const SurvivalSaveData& data)
{
    // 先关定时器，防止 updateGame 在初始化期间触发
    m_gameTimer->stop();
    m_isPaused = true;

    // 初始化场景（在 apply 之前，因为 initScene 会创建 m_player 等）
    initScene();

    // 应用角色和武器
    auto chars = SurvivalPlayer::availableCharacters();
    for (const auto& c : chars) {
        if (c.id == data.characterId) {
            m_player->applyCharacter(c);
            break;
        }
    }
    const WeaponData* wpn = GameData::instance()->getWeaponById(data.weaponId);
    if (wpn) m_player->applyWeapon(*wpn);

    // 恢复状态（在启动之前完成，避免 updateGame 读到未初始化数据）
    m_player->setHealth(data.playerHealth);
    m_player->setMaxHealth(data.playerMaxHealth);
    m_player->setLevel(data.playerLevel);
    m_player->setExp(data.playerExp);
    m_player->setGold(data.playerGold);
    m_enemyManager->setTotalKills(data.totalKills);

    // 恢复技能
    for (const auto& [skillId, level] : data.skills) {
        if (m_player->skillLevel(skillId) > 0)
            m_player->upgradeSkill(skillId, level);
        else
            m_player->addSkill(skillId, level);
    }

    // 恢复波次状态
    m_waveManager->start();  // 初始化波次配置
    m_waveManager->restoreState(data.currentWave, data.totalKills, data.elapsedTime);

    // 一切就绪，启动游戏循环
    m_isStarted = true;
    m_isPaused = false;
    m_gameTimer->start();
}

void SurvivalScene::pauseGame()
{
    m_isPaused = true;
    m_gameTimer->stop();
}

void SurvivalScene::resumeGame()
{
    m_isPaused = false;
    if (m_isStarted) m_gameTimer->start();
}

void SurvivalScene::endGame()
{
    m_gameTimer->stop();
    m_isPaused = true;
    m_isStarted = false;

    int wave = m_waveManager ? int(m_waveManager->currentWave()) : 1;
    bool isNew = false;
    if (m_stats) {
        m_stats->setWave(wave);
        if (m_player) m_stats->setGold(m_player->gold());
        isNew = m_stats->isNewRecord();
        m_stats->saveRecord(wave, m_enemyManager ? m_enemyManager->totalKills() : 0,
                            elapsedTime());
    }
    emit gameFinished(wave, m_enemyManager ? m_enemyManager->totalKills() : 0,
                      elapsedTime(), isNew);
}

float SurvivalScene::elapsedTime() const
{
    return m_waveManager ? m_waveManager->elapsedTime() : 0;
}

// ==================== 主循环 ====================

void SurvivalScene::updateGame()
{
    if (!m_player || !m_waveManager || m_isPaused) return;
    if (m_upgradeUI->isVisible()) return;

    if (!m_player->isAlive()) {
        endGame();
        return;
    }

    qreal dt = pixel_hero::config::DT;

    // 1. WASD移动 + 边界碰撞
    Player::MoveState& ms = m_player->moveState();
    qreal spd = m_player->speed() * 60 * dt;
    qreal dx = 0, dy = 0;
    if (ms.moveLeft)  dx -= spd;
    if (ms.moveRight) dx += spd;
    if (ms.moveUp)    dy -= spd;
    if (ms.moveDown)  dy += spd;

    QPointF newPos = m_player->pos() + QPointF(dx, dy);
    newPos.setX(qBound(pixel_hero::config::PLAYER_PADDING, newPos.x(),
                        pixel_hero::config::SCREEN_W - pixel_hero::config::PLAYER_PADDING));
    newPos.setY(qBound(pixel_hero::config::PLAYER_PADDING, newPos.y(),
                        pixel_hero::config::SCREEN_H - pixel_hero::config::PLAYER_PADDING));
    m_player->setPos(newPos);

    // 2. 玩家冷却更新
    m_player->update(dt);

    // 3. 自动攻击
    autoAttackNearestEnemy();

    // 4. 自动技能
    autoCastSkills();

    // 5. 敌人管理(扫描+AI+清理) — 一次items()调用
    m_enemyManager->update(dt, m_player);

    // 5.5. 拾取物更新
    if (m_pickupManager) m_pickupManager->update(dt, m_player);

    // 6. 波次更新
    m_waveManager->update(dt);

    // 7. 玩家死亡检测
    checkPlayerDeath();

    // 8. HUD更新
    m_hud->updateHUD();

    // 9. 升级检测
    if (m_player->pendingLevelUps() > 0 && !m_upgradeUI->isVisible()) {
        pauseGame();
        m_hud->setShowLevelUpBanner(true);
        QTimer::singleShot(600, this, [this]() {
            m_hud->setShowLevelUpBanner(false);
            emit levelUp(m_player->level());
        });
    }
}

void SurvivalScene::autoAttackNearestEnemy()
{
    Enemy* target = m_enemyManager->findNearest(
        m_player->pos(), pixel_hero::config::PLAYER_ATTACK_RANGE);
    if (target) {
        m_player->attackEnemy(target);
        m_effectManager->showSlash(target->pos());
    }
}

void SurvivalScene::autoCastSkills()
{
    if (!m_player) return;

    for (auto& as : m_player->activeSkills()) {
        if (as.currentCooldown > 0) continue;
        const SkillData* sd = GameData::instance()->getSkillById(as.skillId);
        if (!sd || sd->type != "active") continue;

        auto executor = pixel_hero::survival::SkillExecutor::create(as.skillId);
        if (executor) {
            executor->execute(as, m_player->pos(),
                              m_enemyManager.get(), m_effectManager.get());
        }

        as.currentCooldown = as.cooldown;
    }
}

void SurvivalScene::checkPlayerDeath()
{
    if (m_player && m_player->health() <= 0) {
        endGame();
    }
}

void SurvivalScene::onSkillSelected(const QString& skillId)
{
    if (!m_player) return;
    int currentLvl = m_player->skillLevel(skillId);
    if (currentLvl > 0) {
        m_player->upgradeSkill(skillId, currentLvl + 1);
    } else {
        m_player->addSkill(skillId, 1);
    }
    m_player->applyPendingLevelUp();
    m_upgradeUI->hide();
    resumeGame();
}

void SurvivalScene::onSkillSkipped()
{
    if (!m_player) return;
    m_player->applyPendingLevelUp();
    resumeGame();
}

// ==================== 背景 ====================

void SurvivalScene::drawBackground(QPainter* painter, const QRectF& rect)
{
    painter->fillRect(rect, QColor(20, 20, 35));
}
