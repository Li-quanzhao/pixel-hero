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

    void setPrice(int price);
    void setDescription(const QString& description);

private:
    QString m_id;
    QString m_name;
    ItemType m_type;
    int m_price;
    QString m_description;
};

#endif // ITEM_H