#include "Armor.h"

Armor::Armor(const QString& id, const QString& name)
    : Item(id, name, Item::ARMOR), m_defenseBonus(0)
{
}

Armor::~Armor()
{
}

int Armor::defenseBonus() const { return m_defenseBonus; }
void Armor::setDefenseBonus(int bonus) { m_defenseBonus = bonus; }