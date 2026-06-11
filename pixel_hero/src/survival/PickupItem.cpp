#include "PickupItem.h"
#include "config/GameConfig.h"
#include <QPainter>
#include <QPolygon>

PickupItem::PickupItem(PickupType type, int value, QGraphicsItem* parent)
    : QGraphicsPixmapItem(parent)
    , m_type(type)
    , m_value(value)
    , m_lifetime(pixel_hero::config::PICKUP_LIFETIME)
    , m_expired(false)
    , m_collected(false)
    , m_blinkTimer(0.25f)
{
    setZValue(10);
    drawSprite();
}

PickupItem::~PickupItem() {}

void PickupItem::drawSprite()
{
    QPixmap pix(16, 16);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing, false);

    switch (m_type) {
    case HP: {
        // 红色方形 + 中心白色十字
        p.fillRect(0, 0, 16, 16, QColor(0xE0, 0x40, 0x40));
        p.fillRect(7, 3, 2, 10, QColor(0xFF, 0xFF, 0xFF));
        p.fillRect(3, 7, 10, 2, QColor(0xFF, 0xFF, 0xFF));
        break;
    }
    case XP: {
        // 蓝色底 + 浅蓝菱形
        p.fillRect(0, 0, 16, 16, QColor(0x40, 0x80, 0xE0));
        QPolygon diamond;
        diamond << QPoint(8, 1) << QPoint(15, 8) << QPoint(8, 15) << QPoint(1, 8);
        p.setBrush(QColor(0x80, 0xC0, 0xFF));
        p.setPen(Qt::NoPen);
        p.drawPolygon(diamond);
        break;
    }
    case GOLD: {
        // 金色底 + 浅金圆
        p.fillRect(0, 0, 16, 16, QColor(0xFF, 0xD7, 0x40));
        p.setBrush(QColor(0xFF, 0xE8, 0x80));
        p.setPen(Qt::NoPen);
        p.drawEllipse(QPointF(8, 8), 5, 5);
        break;
    }
    }
    p.end();
    setPixmap(pix);
    setOffset(-8, -8);
}

void PickupItem::update(qreal dt)
{
    m_lifetime -= dt;
    if (m_lifetime <= 0) {
        m_expired = true;
        return;
    }

    if (m_lifetime <= pixel_hero::config::PICKUP_BLINK_START) {
        m_blinkTimer -= dt;
        if (m_blinkTimer <= 0) {
            m_blinkTimer = 0.25f;
            setVisible(!isVisible());
        }
    }
}
