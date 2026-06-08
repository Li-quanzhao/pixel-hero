#ifndef ITEM_H
#define ITEM_H

#include <QString>

class Item
{
public:
    enum ItemType {
        CONSUMABLE,
        WEAPON,
        ARMOR,
        MATERIAL,
        QUEST
    };

    Item(const QString& id, const QString& name, ItemType type);
    virtual ~Item();

    QString id() const;
    QString name() const;
    ItemType type() const;
    int price() const;
    QString description() const;
    int healAmount() const;
    int attack() const;
    int defense() const;

    void setPrice(int price);
    void setDescription(const QString& description);
    void setHealAmount(int amount);
    void setAttack(int attack);
    void setDefense(int defense);

private:
    QString m_id;
    QString m_name;
    ItemType m_type;
    int m_price;
    QString m_description;
    int m_healAmount;
    int m_attack;
    int m_defense;
};

#endif // ITEM_H