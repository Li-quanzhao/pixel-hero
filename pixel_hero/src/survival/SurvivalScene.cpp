#include "SurvivalScene.h"
#include "SurvivalStats.h"
#include "entities/Enemy.h"
#include "utils/GameData.h"
#include <QPainter>
#include <QRandomGenerator>
#include <QLineF>
#include <cmath>
#include <algorithm>

SurvivalScene::SurvivalScene(QObject* parent)
    : QGraphicsScene(parent)
    , m_player(nullptr), m_waveManager(nullptr)
    , m_upgradeUI(nullptr), m_hud(nullptr), m_stats(nullptr)
    , m_gameTimer(new QTimer(this))
    , m_isPaused(false), m_isStarted(false)
    , m_totalKills(0)
{
    setSceneRect(0, 0, 800, 600);

    m_gameTimer->setInterval(16);
    connect(m_gameTimer, &QTimer::timeout, this, &SurvivalScene::updateGame);
}

SurvivalScene::~SurvivalScene() {}

void SurvivalScene::initScene()
{
    clear();

    // 背景
    setBackgroundBrush(QColor(20, 20, 35));

    // 玩家
    m_player = new SurvivalPlayer();
    m_player->setPos(400, 300);
    addItem(m_player);

    // 波次管理器
    m_waveManager = new WaveManager(this, this);

    // HUD
    m_hud = new SurvivalHUD();
    m_hud->bind(m_player, m_waveManager);
    addItem(m_hud);

    // 升级UI
    m_upgradeUI = new UpgradeUI();
    addItem(m_upgradeUI);

    // 技能选择信号
    connect(m_upgradeUI, &UpgradeUI::skillSelected, this, &SurvivalScene::onSkillSelected);
    connect(m_upgradeUI, &UpgradeUI::skillSkipped, this, &SurvivalScene::onSkillSkipped);

    // 升级检测：在 updateGame 中轮询
}

void SurvivalScene::startGame()
{
    // 无参数版本：使用默认角色(战士)+默认武器(短剑)
    startGame("warrior", "short_sword");
}

void SurvivalScene::startGame(const QString& characterId, const QString& weaponId)
{
    initScene();

    // 应用角色配置
    auto chars = SurvivalPlayer::availableCharacters();
    for (const auto& c : chars) {
        if (c.id == characterId) {
            m_player->applyCharacter(c);
            break;
        }
    }

    // 应用武器
    const WeaponData* wpn = GameData::instance()->getWeaponById(weaponId);
    if (wpn) m_player->applyWeapon(*wpn);

    m_gameTimer->start();
    m_isStarted = true;
    m_isPaused = false;
    m_totalKills = 0;
    m_waveManager->start();
    m_hud->updateHUD();
}

void SurvivalScene::startFromSave(const SurvivalSaveData& data)
{
    initScene();

    // 应用角色配置
    auto chars = SurvivalPlayer::availableCharacters();
    for (const auto& c : chars) {
        if (c.id == data.characterId) {
            m_player->applyCharacter(c);
            break;
        }
    }

    // 应用武器
    const WeaponData* wpn = GameData::instance()->getWeaponById(data.weaponId);
    if (wpn) m_player->applyWeapon(*wpn);

    // 恢复玩家状态
    m_player->setLevel(data.playerLevel);
    m_player->setExp(data.playerExp);
    m_player->setHealth(data.playerHealth);
    m_player->setMaxHealth(data.playerMaxHealth);

    // 恢复技能
    for (const auto& pair : data.skills) {
        m_player->addSkill(pair.first, pair.second);
    }

    // 恢复波次（先加载配置再恢复）
    m_waveManager->loadWaveConfig();
    m_waveManager->restoreState(data.currentWave, data.totalKills, data.elapsedTime);

    m_gameTimer->start();
    m_isStarted = true;
    m_isPaused = false;
    m_totalKills = data.totalKills;
    m_hud->updateHUD();
}

void SurvivalScene::pauseGame()
{
    m_isPaused = true;
    m_gameTimer->stop();
}

void SurvivalScene::resumeGame()
{
    m_isPaused = false;
    m_gameTimer->start();
}

void SurvivalScene::endGame()
{
    m_gameTimer->stop();
    m_isPaused = true;
    m_isStarted = false;

    int wave = m_waveManager ? m_waveManager->currentWave() : 0;
    float time = m_waveManager ? m_waveManager->elapsedTime() : 0;
    int kills = m_totalKills;

    // 更新统计信息
    if (m_stats) {
        m_stats->setWave(wave);
        m_stats->setElapsedTime(time);
    }

    bool record = m_stats ? m_stats->isNewRecord() : false;
    if (m_stats && record) {
        m_stats->saveRecord(wave, kills, time);
    }
    emit gameFinished(wave, kills, time, record);
}

float SurvivalScene::elapsedTime() const
{
    return m_waveManager ? m_waveManager->elapsedTime() : 0;
}

void SurvivalScene::updateGame()
{
    if (!m_player || !m_waveManager || m_isPaused) return;
    if (m_upgradeUI->isVisible()) return; // 升级选技时暂停

    // 玩家死亡检测
    if (!m_player->isAlive()) {
        endGame();
        return;
    }

    qreal dt = 0.016;

    // 1. 处理WASD移动
    Player::MoveState& ms = m_player->moveState();
    qreal spd = m_player->speed() * 60 * dt;
    qreal dx = 0, dy = 0;
    if (ms.moveLeft)  dx -= spd;
    if (ms.moveRight) dx += spd;
    if (ms.moveUp)    dy -= spd;
    if (ms.moveDown)  dy += spd;

    QPointF newPos = m_player->pos() + QPointF(dx, dy);
    // 边界碰撞
    newPos.setX(qBound(24.0, newPos.x(), 776.0));
    newPos.setY(qBound(24.0, newPos.y(), 576.0));
    m_player->setPos(newPos);

    // 2. 玩家更新（技能冷却）
    m_player->update(dt);

    // 3. 自动攻击临近敌人
    autoAttackNearestEnemy();

    // 4. 自动释放技能
    autoCastSkills();

    // 5. 收集存活敌人 + AI更新
    QList<QGraphicsItem*> items = this->items();
    QList<Enemy*> aliveEnemies;
    for (QGraphicsItem* item : items) {
        Enemy* enemy = dynamic_cast<Enemy*>(item);
        if (enemy && enemy->isAlive()) {
            aliveEnemies.append(enemy);
        }
    }
    for (Enemy* enemy : aliveEnemies) {
        enemy->updateAI(m_player, aliveEnemies, dt);
    }

    // 6. 波次管理器更新
    m_waveManager->update(dt);

    // 7. 清理死亡敌人
    cleanupDeadEnemies();

    // 8. 检查玩家死亡
    checkPlayerDeath();

    // 9. HUD更新
    m_hud->updateHUD();

    // 10. 检测升级（攒到 pending，先展示横幅再弹升级UI）
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
    Enemy* target = findNearestEnemy();
    if (target) {
        m_player->attackEnemy(target);
        showSlashEffect(target->pos());
    }
}

Enemy* SurvivalScene::findNearestEnemy() const
{
    if (!m_player) return nullptr;
    QList<QGraphicsItem*> items = const_cast<SurvivalScene*>(this)->items();
    QPointF playerPos = m_player->pos();
    Enemy* nearest = nullptr;
    qreal minDist = 60; // 攻击范围

    for (QGraphicsItem* item : items) {
        Enemy* enemy = dynamic_cast<Enemy*>(item);
        if (!enemy || !enemy->isAlive()) continue;
        qreal d = QLineF(playerPos, enemy->pos()).length();
        if (d < minDist) { minDist = d; nearest = enemy; }
    }
    return nearest;
}

void SurvivalScene::autoCastSkills()
{
    if (!m_player) return;
    QList<QGraphicsItem*> itemList = items();

    for (auto& as : m_player->activeSkills()) {
        if (as.currentCooldown > 0) continue;
        const SkillData* sd = GameData::instance()->getSkillById(as.skillId);
        if (!sd || sd->type != "active") continue;

        // 收集存活敌人
        QList<Enemy*> enemies;
        for (QGraphicsItem* item : itemList) {
            Enemy* e = dynamic_cast<Enemy*>(item);
            if (e && e->isAlive()) enemies.append(e);
        }
        if (enemies.isEmpty()) continue;

        QPointF pp = m_player->pos();

        if (as.skillId == "fireball") {
            // 向最近敌人发射火球
            Enemy* nearest = nullptr;
            qreal nd = 99999;
            for (Enemy* e : enemies) {
                qreal d = QLineF(pp, e->pos()).length();
                if (d < nd) { nd = d; nearest = e; }
            }
            if (nearest) {
                showFireballEffect(pp, nearest->pos());
                nearest->takeDamage(as.damage);
            }
        } else if (as.skillId == "lightning") {
            // 连锁攻击
            struct ED { Enemy* e; qreal d; };
            QList<ED> sorted;
            for (Enemy* e : enemies) {
                sorted.append({e, QLineF(pp, e->pos()).length()});
            }
            std::sort(sorted.begin(), sorted.end(), [](const ED& a, const ED& b) { return a.d < b.d; });
            int chain = qMin(as.extra, sorted.size());
            QPointF prevPos = pp;
            for (int i = 0; i < chain; i++) {
                sorted[i].e->takeDamage(as.damage);
                showLightningEffect(prevPos, sorted[i].e->pos());
                prevPos = sorted[i].e->pos();
            }
        } else if (as.skillId == "frost_nova") {
            // 范围伤害
            showFrostNovaEffect(pp, as.extra);
            for (Enemy* e : enemies) {
                qreal d = QLineF(pp, e->pos()).length();
                if (d <= as.extra) e->takeDamage(as.damage);
            }
        }

        // 重置冷却
        as.currentCooldown = as.cooldown;
    }
}

void SurvivalScene::cleanupDeadEnemies()
{
    QList<QGraphicsItem*> itemList = items();
    for (QGraphicsItem* item : itemList) {
        Enemy* enemy = dynamic_cast<Enemy*>(item);
        if (enemy && !enemy->isAlive()) {
            m_player->addExp(enemy->expReward());
            m_totalKills++;
            m_waveManager->addKill();
            if (m_stats) m_stats->addKill();
            removeItem(enemy);
            delete enemy;
        }
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
    // 应用本次升级的属性加成
    m_player->applyPendingLevelUp();
    m_upgradeUI->hide();
    resumeGame();
}

void SurvivalScene::onSkillSkipped()
{
    if (!m_player) return;
    // 跳过升级，但仍应用属性加成
    m_player->applyPendingLevelUp();
    resumeGame();
}

// ========================= 战斗特效 =========================

void SurvivalScene::showSlashEffect(QPointF pos)
{
    // 白色挥砍弧线
    auto* slash = addEllipse(-8, -8, 16, 16, QPen(QColor(255, 255, 200, 200), 2),
                             QBrush(QColor(255, 255, 255, 80)));
    slash->setPos(pos);
    QTimer::singleShot(150, this, [this, slash]() { removeItem(slash); delete slash; });
}

void SurvivalScene::showFireballEffect(QPointF from, QPointF to)
{
    // 橙色火球弹道 (多个小圆组成轨迹)
    QLineF line(from, to);
    int steps = 5;
    for (int i = 1; i <= steps; ++i) {
        QPointF p = line.pointAt(i / qreal(steps));
        auto* orb = addEllipse(-3, -3, 6, 6, Qt::NoPen,
                               QBrush(QColor(255, 136 + i * 20, 0, 180)));
        orb->setPos(p);
        QTimer::singleShot(100 + i * 40, this, [this, orb]() { removeItem(orb); delete orb; });
    }
    // 撞击点火花
    auto* hit = addEllipse(-6, -6, 12, 12, Qt::NoPen,
                           QBrush(QColor(255, 200, 0, 200)));
    hit->setPos(to);
    QTimer::singleShot(200, this, [this, hit]() { removeItem(hit); delete hit; });
}

void SurvivalScene::showLightningEffect(QPointF from, QPointF to)
{
    // 蓝白闪电锯齿线
    QPainterPath path;
    path.moveTo(from);
    QLineF line(from, to);
    qreal len = line.length();
    int segs = qMax(2, int(len / 20));
    for (int i = 1; i <= segs; ++i) {
        qreal t = i / qreal(segs);
        QPointF p = line.pointAt(t);
        qreal jitterX = (i % 2 == 0 ? 6 : -6);
        qreal jitterY = (QRandomGenerator::global()->bounded(8) - 4);
        path.lineTo(p + QPointF(jitterX, jitterY));
    }
    auto* bolt = addPath(path, QPen(QColor(100, 180, 255, 220), 2),
                         QBrush(Qt::NoBrush));
    // 外层辉光
    auto* glow = addPath(path, QPen(QColor(150, 210, 255, 100), 4),
                         QBrush(Qt::NoBrush));
    QTimer::singleShot(250, this, [this, bolt, glow]() {
        removeItem(bolt); delete bolt;
        removeItem(glow); delete glow;
    });
}

void SurvivalScene::showFrostNovaEffect(QPointF center, float /*radius*/)
{
    // 冰蓝扩散环
    auto* ring = addEllipse(center.x() - 5, center.y() - 5, 10, 10,
                            QPen(QColor(100, 200, 255, 200), 3),
                            QBrush(Qt::NoBrush));
    auto* ring2 = addEllipse(center.x() - 30, center.y() - 30, 60, 60,
                             QPen(QColor(80, 180, 240, 120), 2),
                             QBrush(Qt::NoBrush));
    QTimer::singleShot(350, this, [this, ring, ring2]() {
        removeItem(ring); delete ring;
        removeItem(ring2); delete ring2;
    });
}

void SurvivalScene::drawBackground(QPainter* painter, const QRectF& rect)
{
    painter->fillRect(rect, QColor(20, 20, 35));
}

QList<Enemy*> SurvivalScene::aliveEnemies() const
{
    QList<Enemy*> result;
    QList<QGraphicsItem*> itemList = const_cast<SurvivalScene*>(this)->items();
    for (QGraphicsItem* item : itemList) {
        Enemy* e = dynamic_cast<Enemy*>(item);
        if (e && e->isAlive()) result.append(e);
    }
    return result;
}
