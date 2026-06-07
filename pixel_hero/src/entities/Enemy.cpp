#include "Enemy.h"
#include "Player.h"
#include <QPainter>
#include <cmath>

Enemy::Enemy(EnemyType type, QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent)
    , m_type(type), m_state(IDLE)
    , m_attackTimer(0), m_patrolTimer(0)
    , m_patrolDirX(1), m_patrolDirY(0)
{
    QPixmap enemyPixmap(40, 40);

    switch (type) {
    case GOBLIN:
        m_maxHealth = 50; m_attack = 8; m_defense = 3;
        m_expReward = 20; m_goldReward = 10;
        m_moveSpeed = 1.5; m_attackInterval = 1.5;
        enemyPixmap.fill(QColor(0x8b, 0x45, 0x13));
        break;
    case SLIME:
        m_maxHealth = 20; m_attack = 5; m_defense = 1;
        m_expReward = 10; m_goldReward = 5;
        m_moveSpeed = 1.0; m_attackInterval = 2.0;
        enemyPixmap.fill(QColor(0x22, 0x8b, 0x22));
        break;
    case SKELETON:
        m_maxHealth = 80; m_attack = 12; m_defense = 5;
        m_expReward = 35; m_goldReward = 20;
        m_moveSpeed = 1.2; m_attackInterval = 1.2;
        enemyPixmap.fill(QColor(0xc0, 0xc0, 0xc0));
        break;
    case BOSS:
        m_maxHealth = 300; m_attack = 25; m_defense = 15;
        m_expReward = 200; m_goldReward = 100;
        m_moveSpeed = 0.8; m_attackInterval = 1.0;
        enemyPixmap.fill(QColor(0x8b, 0x00, 0x00));
        break;
    }

    setPixmap(enemyPixmap);
    m_health = m_maxHealth;
    setOffset(-boundingRect().width() / 2, -boundingRect().height());
    m_patrolOrigin = pos();
}

Enemy::~Enemy() {}

int Enemy::health() const { return m_health; }
int Enemy::maxHealth() const { return m_maxHealth; }
int Enemy::attack() const { return m_attack; }
int Enemy::defense() const { return m_defense; }
int Enemy::expReward() const { return m_expReward; }
int Enemy::goldReward() const { return m_goldReward; }
Enemy::EnemyType Enemy::enemyType() const { return m_type; }
Enemy::State Enemy::state() const { return m_state; }

QString Enemy::name() const
{
    switch (m_type) {
    case GOBLIN:   return "哥布林";
    case SLIME:    return "史莱姆";
    case SKELETON: return "骷髅兵";
    case BOSS:     return "BOSS";
    default:       return "未知";
    }
}

void Enemy::takeDamage(int damage)
{
    int finalDamage = qMax(0, damage - m_defense);
    m_health -= finalDamage;

    // 受伤后进入追击状态
    if (m_health > 0 && m_state != ATTACKING) {
        m_state = CHASE;
    }
}

void Enemy::update(qreal deltaTime)
{
    m_attackTimer += deltaTime;
}

void Enemy::updateAI(Player* player, qreal deltaTime)
{
    if (!isAlive()) {
        if (m_state != DEAD) {
            m_state = DEAD;
            hide();
        }
        return;
    }

    if (!player) return;

    m_attackTimer += deltaTime;
    m_patrolTimer += deltaTime;

    QPointF playerPos = player->pos();
    QPointF myPos = pos();
    qreal dist = std::sqrt(std::pow(playerPos.x() - myPos.x(), 2) +
                           std::pow(playerPos.y() - myPos.y(), 2));

    const qreal CHASE_RANGE = 120;
    const qreal ATTACK_RANGE = 40;
    const qreal PATROL_RANGE = 60;

    switch (m_state) {
    case IDLE:
        if (dist < CHASE_RANGE) m_state = CHASE;
        else if (m_patrolTimer > 3.0) { m_state = PATROL; m_patrolTimer = 0; }
        break;

    case PATROL: {
        qreal dx = m_patrolDirX * m_moveSpeed * 0.5 * deltaTime * 60;
        qreal dy = m_patrolDirY * m_moveSpeed * 0.5 * deltaTime * 60;
        setPos(myPos.x() + dx, myPos.y() + dy);

        qreal patrolDist = std::sqrt(std::pow(pos().x() - m_patrolOrigin.x(), 2) +
                                     std::pow(pos().y() - m_patrolOrigin.y(), 2));
        if (patrolDist > PATROL_RANGE || m_patrolTimer > 2.0) {
            m_patrolDirX = (std::rand() % 3) - 1;
            m_patrolDirY = (std::rand() % 3) - 1;
            m_patrolTimer = 0;
            m_state = IDLE;
        }
        if (dist < CHASE_RANGE) m_state = CHASE;
        break;
    }

    case CHASE:
        if (dist < ATTACK_RANGE) {
            m_state = ATTACKING;
        } else {
            qreal dx = (playerPos.x() > myPos.x() ? 1 : -1) * m_moveSpeed * deltaTime * 60;
            qreal dy = (playerPos.y() > myPos.y() ? 1 : -1) * m_moveSpeed * deltaTime * 60;
            setPos(myPos.x() + dx, myPos.y() + dy);
        }
        if (dist > CHASE_RANGE * 1.5) m_state = IDLE;
        break;

    case ATTACKING:
        if (m_attackTimer >= m_attackInterval && dist < ATTACK_RANGE) {
            player->takeDamage(m_attack);
            m_attackTimer = 0;
        }
        if (dist > ATTACK_RANGE * 1.5) m_state = CHASE;
        break;

    case DEAD:
        break;
    }
}

bool Enemy::isAlive() const { return m_health > 0; }
