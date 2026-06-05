#include "Enemy.h"

Enemy::Enemy(EnemyType type, QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent), m_type(type), m_attackTimer(0)
{
    QPixmap enemyPixmap(40, 40);
    
    switch (type) {
    case GOBLIN:
        m_maxHealth = 50;
        m_attack = 8;
        m_defense = 3;
        m_expReward = 20;
        m_moveSpeed = 1.5;
        m_attackInterval = 1.5;
        enemyPixmap.fill(QColor(0x8b, 0x45, 0x13));
        break;
    case SLIME:
        m_maxHealth = 20;
        m_attack = 5;
        m_defense = 1;
        m_expReward = 10;
        m_moveSpeed = 1.0;
        m_attackInterval = 2.0;
        enemyPixmap.fill(QColor(0x22, 0x8b, 0x22));
        break;
    case SKELETON:
        m_maxHealth = 80;
        m_attack = 12;
        m_defense = 5;
        m_expReward = 35;
        m_moveSpeed = 1.2;
        m_attackInterval = 1.2;
        enemyPixmap.fill(QColor(0xc0, 0xc0, 0xc0));
        break;
    case BOSS:
        m_maxHealth = 500;
        m_attack = 30;
        m_defense = 15;
        m_expReward = 200;
        m_moveSpeed = 0.8;
        m_attackInterval = 1.0;
        enemyPixmap.fill(QColor(0x8b, 0x00, 0x00));
        break;
    }
    
    setPixmap(enemyPixmap);

    m_health = m_maxHealth;
    setOffset(-boundingRect().width() / 2, -boundingRect().height());
}

Enemy::~Enemy()
{
}

int Enemy::health() const { return m_health; }
int Enemy::maxHealth() const { return m_maxHealth; }
int Enemy::attack() const { return m_attack; }
int Enemy::defense() const { return m_defense; }
int Enemy::expReward() const { return m_expReward; }

void Enemy::takeDamage(int damage)
{
    int finalDamage = qMax(0, damage - m_defense);
    m_health -= finalDamage;
}

void Enemy::update(qreal deltaTime)
{
    m_attackTimer += deltaTime;
}

bool Enemy::isAlive() const
{
    return m_health > 0;
}