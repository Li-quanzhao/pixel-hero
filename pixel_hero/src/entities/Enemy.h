#ifndef ENEMY_H
#define ENEMY_H

#include <QGraphicsPixmapItem>
#include <QPointF>
#include <QList>

class Player;

class Enemy : public QGraphicsPixmapItem
{
public:
    enum EnemyType { GOBLIN, SLIME, SKELETON, BAT, GOBLIN_ELITE, DRAGON };
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

    void setHealth(int v);
    void setMaxHealth(int v);
    void setAttack(int v);
    void setDefense(int v);
    void setExpReward(int v);
    void setGoldReward(int v);
    void setMoveSpeed(float v);
    void setAttackRange(float v);

    void takeDamage(int damage);
    void update(qreal deltaTime);
    void updateAI(Player* player, QList<Enemy*> otherEnemies, qreal deltaTime);
    bool isAlive() const;

    QPointF pos() const { return QGraphicsPixmapItem::pos(); }

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
    qreal m_attackRange;

    qreal m_attackTimer;
    qreal m_attackInterval;
};

#endif // ENEMY_H
