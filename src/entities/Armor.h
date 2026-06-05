#ifndef ARMOR_H
#define ARMOR_H

#include "Item.h"

class Armor : public Item
{
public:
    Armor(const QString& id, const QString& name);
    ~Armor();

    int defenseBonus() const;
    void setDefenseBonus(int bonus);

private:
    int m_defenseBonus;
};

#endif // ARMOR_H