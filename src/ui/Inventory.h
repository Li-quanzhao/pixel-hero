#ifndef INVENTORY_H
#define INVENTORY_H

#include <QGraphicsItem>
#include <QRectF>
#include <QList>
#include "Item.h"

class Inventory : public QGraphicsItem
{
public:
    Inventory(QGraphicsItem *parent = nullptr);
    ~Inventory();

    void addItem(Item* item);
    void removeItem(Item* item);
    void clear();
    QList<Item*> items() const;

    void show();
    void hide();
    bool isVisible() const;

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    QList<Item*> m_items;
    bool m_isVisible;
    int m_selectedSlot;
};

#endif // INVENTORY_H