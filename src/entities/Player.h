#ifndef PLAYER_H
#define PLAYER_H

#include <QGraphicsPixmapItem>
#include <QList>
#include "Weapon.h"
#include "Armor.h"
#include "Skill.h"

class Player : public QGraphicsPixmapItem
{
public:
    Player(QGraphicsItem *parent = nullptr);
    ~Player();

    int level() const;
    int exp() const;
    int health() const;
    int maxHealth() const;
    int attack() const;
    int defense() const;
    int speed() const;

    void setLevel(int level);
    void setExp(int exp);
    void setHealth(int health);
    void setMaxHealth(int maxHealth);
    void setAttack(int attack);
    void setDefense(int defense);
    void setSpeed(int speed);

    void update(qreal deltaTime);
    void attackEnemy(class Enemy* enemy);
    void takeDamage(int damage);
    void useSkill(int skillIndex);
    void addExp(int amount);

    Weapon* getEquippedWeapon() const;

    void setMoveUp(bool value);
    void setMoveDown(bool value);
    void setMoveLeft(bool value);
    void setMoveRight(bool value);
    void attack();

private:
    int m_level;
    int m_exp;
    int m_health;
    int m_maxHealth;
    int m_attack;
    int m_defense;
    int m_speed;

    Weapon *m_equippedWeapon;
    Armor *m_equippedArmor;
    QList<Skill*> m_skills;

    bool m_moveUp;
    bool m_moveDown;
    bool m_moveLeft;
    bool m_moveRight;
    bool m_isAttacking;

    qreal m_attackCooldown;
    qreal m_currentAttackCooldown;

    void move(qreal deltaTime);
    void checkLevelUp();
};

#endif // PLAYER_H