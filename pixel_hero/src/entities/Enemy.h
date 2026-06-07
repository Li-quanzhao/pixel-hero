#ifndef ENEMY_H
#define ENEMY_H

#include <QGraphicsPixmapItem>
#include <QPointF>

class Player;

class Enemy : public QGraphicsPixmapItem
{
public:
    enum EnemyType { GOBLIN, SLIME, SKELETON, BOSS };
    enum State { IDLE, PATROL, CHASE, ATTACKING, DEAD };

    Enemy(EnemyType type, QGraphicsItem *parent = nullptr);
    ~Enemy();

    int health() const;
    int maxHealth() const;
    int attack() const;
    int defense() const;
    int expReward() const;
    int goldReward() const;
    QString name() const;
    EnemyType enemyType() const;
    State state() const;

    void takeDamage(int damage);
    void update(qreal deltaTime);
    void updateAI(Player* player, qreal deltaTime);
    bool isAlive() const;

private:
    EnemyType m_type;
    State m_state;
    int m_health;
    int m_maxHealth;
    int m_attack;
    int m_defense;
    int m_expReward;
    int m_goldReward;
    qreal m_moveSpeed;

    qreal m_attackTimer;
    qreal m_attackInterval;
    qreal m_patrolTimer;
    QPointF m_patrolOrigin;
    int m_patrolDirX;
    int m_patrolDirY;
};

#endif // ENEMY_H
