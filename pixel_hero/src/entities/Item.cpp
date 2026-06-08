#include "Item.h"

Item::Item(const QString& id, const QString& name, ItemType type)
    : m_id(id), m_name(name), m_type(type), m_price(0)
    , m_healAmount(0), m_attack(0), m_defense(0)
{
}

Item::~Item()
{
}

QString Item::id() const { return m_id; }
QString Item::name() const { return m_name; }
Item::ItemType Item::type() const { return m_type; }
int Item::price() const { return m_price; }
QString Item::description() const { return m_description; }
int Item::healAmount() const { return m_healAmount; }
int Item::attack() const { return m_attack; }
int Item::defense() const { return m_defense; }

void Item::setPrice(int price) { m_price = price; }
void Item::setDescription(const QString& description) { m_description = description; }
void Item::setHealAmount(int amount) { m_healAmount = amount; }
void Item::setAttack(int attack) { m_attack = attack; }
void Item::setDefense(int defense) { m_defense = defense; }