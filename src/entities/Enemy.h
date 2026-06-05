#ifndef ENEMY_H
#define ENEMY_H

#include <QGraphicsPixmapItem>

class Enemy : public QGraphicsPixmapItem
{
public:
    enum EnemyType {
        GOBLIN,
        SLIME,
        SKELETON,
        BOSS
    };

    Enemy(EnemyType type, QGraphicsItem *parent = nullptr);
    ~Enemy();

    int health() const;
    int maxHealth() const;
    int attack() const;
    int defense() const;
    int expReward() const;

    void takeDamage(int damage);
    void update(qreal deltaTime);
    bool isAlive() const;

private:
    EnemyType m_type;
    int m_health;
    int m_maxHealth;
    int m_attack;
    int m_defense;
    int m_expReward;
    qreal m_moveSpeed;

    qreal m_attackTimer;
    qreal m_attackInterval;
};

#endif // ENEMY_H