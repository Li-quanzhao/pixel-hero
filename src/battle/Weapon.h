#ifndef WEAPON_H
#define WEAPON_H

#include "Item.h"

class Weapon : public Item
{
public:
    Weapon(const QString& id, const QString& name);
    ~Weapon();

    int attackBonus() const;
    void setAttackBonus(int bonus);

private:
    int m_attackBonus;
};

#endif // WEAPON_H