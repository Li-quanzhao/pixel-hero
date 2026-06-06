#ifndef PLAYER_H
#define PLAYER_H

#include <QGraphicsPixmapItem>
#include <QList>
#include "Weapon.h"
#include "Armor.h"
#include "Skill.h"
#include "Item.h"

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
    int attack() const;
    int defense() const;
    int speed() const;
    int gold() const;

    void setLevel(int level);
    void setExp(int exp);
    void setHealth(int health);
    void setMaxHealth(int maxHealth);
    void setAttack(int attack);
    void setDefense(int defense);
    void setSpeed(int speed);
    void setGold(int gold);

    void update(qreal deltaTime);
    void attackEnemy(class Enemy* enemy);
    void takeDamage(int damage);
    void useSkill(int skillIndex);
    void addExp(int amount);

    // 装备
    Weapon* getEquippedWeapon() const;
    void equipWeapon(Weapon* weapon);
    void unequipWeapon();
    Armor* getEquippedArmor() const;
    void equipArmor(Armor* armor);
    void unequipArmor();

    // 背包
    void addItemToInventory(Item* item);
    void removeItemFromInventory(int index);
    Item* itemAt(int index) const;
    int itemCount() const;
    QList<Item*>& inventory();

    // 移动
    MoveState& moveState();
    void setMoveUp(bool value);
    void setMoveDown(bool value);
    void setMoveLeft(bool value);
    void setMoveRight(bool value);
    void doAttack();

    // 存档用
    void applySaveData(int lvl, int e, int hp, int mhp, int atk, int def, int g);

private:
    int m_level, m_exp, m_health, m_maxHealth;
    int m_attack, m_defense, m_speed, m_gold;

    Weapon *m_equippedWeapon;
    Armor *m_equippedArmor;
    QList<Skill*> m_skills;
    QList<Item*> m_inventory;

    MoveState m_moveState;
    bool m_isAttacking;
    qreal m_attackCooldown;
    qreal m_currentAttackCooldown;

    void checkLevelUp();
};

#endif // PLAYER_H
