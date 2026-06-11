#include "Enemy.h"
#include "Player.h"
#include "config/GameConfig.h"
#include <QPainter>
#include <QTimer>
#include <cmath>

Enemy::Enemy(EnemyType type, QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent)
    , m_type(type), m_state(CHASE)
    , m_attackRange(40)
    , m_attackTimer(0)
{
    QPixmap enemyPixmap;

    switch (type) {
    case GOBLIN:
        m_maxHealth = 50; m_attack = 8; m_defense = 3;
        m_expReward = 20; m_goldReward = 10;
        m_moveSpeed = 1.5; m_attackInterval = 0.8;
        m_attackRange = 40;
        enemyPixmap = QPixmap(":/sprites/enemies/goblin");
        break;
    case SLIME:
        m_maxHealth = 20; m_attack = 5; m_defense = 1;
        m_expReward = 10; m_goldReward = 5;
        m_moveSpeed = 1.0; m_attackInterval = 1.0;
        m_attackRange = 35;
        enemyPixmap = QPixmap(":/sprites/enemies/slime");
        break;
    case SKELETON:
        m_maxHealth = 80; m_attack = 12; m_defense = 5;
        m_expReward = 35; m_goldReward = 20;
        m_moveSpeed = 1.2; m_attackInterval = 0.7;
        m_attackRange = 45;
        enemyPixmap = QPixmap(":/sprites/enemies/skeleton");
        break;
    case BAT:
        m_maxHealth = 15; m_attack = 15; m_defense = 0;
        m_expReward = 25; m_goldReward = 12;
        m_moveSpeed = 3.0; m_attackInterval = 0.5;
        m_attackRange = 35;
        enemyPixmap = QPixmap(":/sprites/enemies/bat");
        break;
    case GOBLIN_ELITE:
        m_maxHealth = 200; m_attack = 20; m_defense = 10;
        m_expReward = 80; m_goldReward = 40;
        m_moveSpeed = 1.0; m_attackInterval = 1.0;
        m_attackRange = 48;
        enemyPixmap = QPixmap(":/sprites/enemies/goblin_elite");
        break;
    case DRAGON:
        m_maxHealth = 500; m_attack = 30; m_defense = 20;
        m_expReward = 300; m_goldReward = 100;
        m_moveSpeed = 0.6; m_attackInterval = 1.0;
        m_attackRange = 55;
        enemyPixmap = QPixmap(":/sprites/enemies/dragon");
        break;
    }

    setPixmap(enemyPixmap);
    m_originalPixmap = enemyPixmap;  // 保存原始精灵图
    m_health = m_maxHealth;
    setOffset(-boundingRect().width() / 2, -boundingRect().height());
}

Enemy::~Enemy() {}

// ---- 属性 ----
int Enemy::health() const { return m_health; }
int Enemy::maxHealth() const { return m_maxHealth; }
int Enemy::attack() const { return m_attack; }
int Enemy::defense() const { return m_defense; }
int Enemy::expReward() const { return m_expReward; }
int Enemy::goldReward() const { return m_goldReward; }
Enemy::EnemyType Enemy::enemyType() const { return m_type; }
Enemy::State Enemy::state() const { return m_state; }

void Enemy::setHealth(int v)     { m_health = v; }
void Enemy::setMaxHealth(int v)  { m_maxHealth = v; }
void Enemy::setAttack(int v)     { m_attack = v; }
void Enemy::setDefense(int v)    { m_defense = v; }
void Enemy::setExpReward(int v)  { m_expReward = v; }
void Enemy::setGoldReward(int v) { m_goldReward = v; }
void Enemy::setMoveSpeed(float v){ m_moveSpeed = v; }
void Enemy::setAttackRange(float v){ m_attackRange = v; }

QString Enemy::name() const
{
    switch (m_type) {
    case GOBLIN:   return "哥布林";
    case SLIME:    return "史莱姆";
    case SKELETON: return "骷髅兵";
    case BAT:      return "蝙蝠";
    case GOBLIN_ELITE: return "精英哥布林";
    case DRAGON:   return "龙";
    default:       return "未知";
    }
}

void Enemy::takeDamage(int damage)
{
    int finalDamage = qMax(0, damage - m_defense);
    m_health -= finalDamage;
    if (m_health > 0) {
        m_state = CHASE;
        flashHit();  // 仅存活时闪烁，避免 use-after-free
    }
}

void Enemy::flashHit()
{
    // 生成白色闪烁精灵图
    QPixmap flash(m_originalPixmap.size());
    flash.fill(Qt::transparent);
    QPainter p(&flash);
    p.setCompositionMode(QPainter::CompositionMode_Source);
    p.drawPixmap(0, 0, m_originalPixmap);
    p.setCompositionMode(QPainter::CompositionMode_SourceIn);
    p.fillRect(flash.rect(), QColor(255, 255, 255, pixel_hero::config::FLASH_ALPHA));
    p.end();
    setPixmap(flash);
    m_flashTimer = pixel_hero::config::FLASH_FRAMES;
}

void Enemy::update(qreal deltaTime)
{
    Q_UNUSED(deltaTime);
    if (m_flashTimer > 0) {
        m_flashTimer--;
        if (m_flashTimer <= 0 && isAlive()) {
            setPixmap(m_originalPixmap);
        }
    }
}

void Enemy::updateAI(Player* player, QList<Enemy*> otherEnemies, qreal deltaTime)
{
    if (!isAlive()) {
        if (m_state != DEAD) { m_state = DEAD; hide(); }
        return;
    }
    if (!player) return;

    m_attackTimer += deltaTime;

    QPointF playerPos = player->pos();
    QPointF myPos = pos();
    qreal dx = playerPos.x() - myPos.x();
    qreal dy = playerPos.y() - myPos.y();
    qreal dist = std::sqrt(dx*dx + dy*dy);

    switch (m_state) {
    case CHASE:
        if (dist < m_attackRange) {
            m_state = ATTACKING;
            m_attackTimer = m_attackInterval; // 接触立即攻击
        } else {
            // 直接奔向玩家 + 分离力
            QPointF toPlayer = (dist > 0.001f)
                ? QPointF(dx / dist, dy / dist)
                : QPointF(0, 0);

            QPointF separation(0, 0);
            for (Enemy* other : otherEnemies) {
                if (other == this || !other->isAlive()) continue;
                QPointF diff = myPos - other->pos();
                qreal sepDist = std::sqrt(diff.x()*diff.x() + diff.y()*diff.y());
                if (sepDist < 25 && sepDist > 0.001f) {
                    qreal force = (25 - sepDist) / 25 * 2.0f;
                    separation += QPointF(diff.x() / sepDist * force,
                                          diff.y() / sepDist * force);
                }
            }

            QPointF velocity = toPlayer + separation;
            qreal vLen = std::sqrt(velocity.x()*velocity.x() + velocity.y()*velocity.y());
            if (vLen > 0.001f) {
                velocity = velocity / vLen * m_moveSpeed * deltaTime * 60;
            }
            setPos(myPos.x() + velocity.x(), myPos.y() + velocity.y());
        }
        break;

    case ATTACKING:
        if (m_attackTimer >= m_attackInterval && dist < m_attackRange) {
            player->takeDamage(m_attack);
            m_attackTimer = 0;
        }
        if (dist > m_attackRange * 1.5f) m_state = CHASE;
        break;

    default: break;
    }
}

bool Enemy::isAlive() const { return m_health > 0; }
