#ifndef PICKUPITEM_H
#define PICKUPITEM_H

#include <QGraphicsPixmapItem>

class PickupItem : public QGraphicsPixmapItem
{
public:
    enum PickupType { HP, XP, GOLD };

    PickupItem(PickupType type, int value, QGraphicsItem* parent = nullptr);
    ~PickupItem();

    PickupType pickupType() const { return m_type; }
    int       value()      const { return m_value; }
    float     lifetime()   const { return m_lifetime; }

    void update(qreal dt);

    bool isExpired()   const { return m_expired; }
    bool isCollected() const { return m_collected; }
    void markCollected()     { m_collected = true; }

private:
    PickupType m_type;
    int        m_value;
    float      m_lifetime;
    bool       m_expired;
    bool       m_collected;
    float      m_blinkTimer;

    void drawSprite();
};

#endif // PICKUPITEM_H
