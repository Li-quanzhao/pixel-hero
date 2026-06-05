#include "Weapon.h"

Weapon::Weapon(const QString& id, const QString& name)
    : Item(id, name, Item::WEAPON), m_attackBonus(0)
{
}

Weapon::~Weapon()
{
}

int Weapon::attackBonus() const { return m_attackBonus; }
void Weapon::setAttackBonus(int bonus) { m_attackBonus = bonus; }