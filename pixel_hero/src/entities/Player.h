#ifndef PLAYER_H
#define PLAYER_H

#include <QGraphicsPixmapItem>
#include "Weapon.h"

class Enemy;

class Player : public QGraphicsPixmapItem
{
public:
    struct MoveState {
        bool moveUp    = false;
        bool moveDown  = false;
        bool moveLeft  = false;
        bool moveRight = false;
    };

    Player(QGraphicsItem *parent = nullptr);
    ~Player();

    int level() const;
    int exp() const;
    int health() const;
    int maxHealth() const;
    virtual int attack() const;
    virtual int defense() const;
    virtual int speed() const;

    void setLevel(int level);
    void setExp(int exp);
    void setHealth(int health);
    void setMaxHealth(int maxHealth);
    virtual void setAttack(int attack);
    void setDefense(int defense);
    void setSpeed(int speed);

    // 武器
    void applyWeapon(const WeaponData& weapon);
    const WeaponData& weapon() const { return m_weapon; }
    float attackSpeed() const;

    virtual void update(qreal deltaTime);
    void attackEnemy(Enemy* enemy);
    void takeDamage(int damage);
    bool isInvincible() const;
    void addExp(int amount);
    bool isAlive() const;

    // 移动
    MoveState& moveState();
    void setMoveUp(bool value);
    void setMoveDown(bool value);
    void setMoveLeft(bool value);
    void setMoveRight(bool value);

    // 升级检测（子类可重写）
    virtual void checkLevelUp();

private:
    int m_level, m_exp, m_health, m_maxHealth;
    int m_attack, m_defense, m_speed;
    WeaponData m_weapon;

    MoveState m_moveState;
    qreal m_attackCooldown;
    qreal m_currentAttackCooldown;

    // 无敌
    bool  m_invincible;
    float m_invincibleTimer;
    float m_blinkTimer;

    static constexpr float INVINCIBLE_TIME = 0.5f;
    static constexpr float BLINK_INTERVAL  = 0.1f;
};

#endif // PLAYER_H
