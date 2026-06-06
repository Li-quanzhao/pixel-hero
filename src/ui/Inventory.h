#ifndef INVENTORY_H
#define INVENTORY_H

#include <QObject>
#include <QGraphicsItem>
#include <QRectF>
#include <QList>
#include "Item.h"
#include "Player.h"

class Inventory : public QObject, public QGraphicsItem
{
    Q_OBJECT

public:
    Inventory(QGraphicsItem *parent = nullptr);
    ~Inventory();

    void setPlayer(Player* player);
    void refresh();

    void show();
    void hide();
    bool isVisible() const;

    void selectPrevious();
    void selectNext();
    void confirmItem();

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

signals:
    void itemUsed(int index);
    void closed();

private:
    Player* m_player;
    bool m_isVisible;
    int m_selectedSlot;
    int m_slotCount;

    static const int SLOT_SIZE = 50;
    static const int GAP = 5;
    static const int COLS = 6;
    static const int START_X = 140;
    static const int START_Y = 120;

    QRectF getSlotRect(int i) const;
};

#endif // INVENTORY_H
