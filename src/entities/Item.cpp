#include "Item.h"

Item::Item(const QString& id, const QString& name, ItemType type)
    : m_id(id), m_name(name), m_type(type), m_price(0)
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

void Item::setPrice(int price) { m_price = price; }
void Item::setDescription(const QString& description) { m_description = description; }