#include "Inventory.h"
#include <QPainter>

Inventory::Inventory(QGraphicsItem *parent)
    : QGraphicsItem(parent), m_isVisible(false), m_selectedSlot(0)
{
    setZValue(150);
}

Inventory::~Inventory()
{
    clear();
}

void Inventory::addItem(Item* item)
{
    m_items.append(item);
}

void Inventory::removeItem(Item* item)
{
    m_items.removeAll(item);
    delete item;
}

void Inventory::clear()
{
    qDeleteAll(m_items);
    m_items.clear();
}

QList<Item*> Inventory::items() const
{
    return m_items;
}

void Inventory::show()
{
    m_isVisible = true;
    update();
}

void Inventory::hide()
{
    m_isVisible = false;
    update();
}

bool Inventory::isVisible() const
{
    return m_isVisible;
}

QRectF Inventory::boundingRect() const
{
    return QRectF(0, 0, 800, 600);
}

void Inventory::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (!m_isVisible) return;

    painter->setBrush(QColor(0x1a, 0x1a, 0x2e, 200));
    painter->drawRect(0, 0, 800, 600);

    int invX = 200;
    int invY = 150;
    int slotWidth = 50;
    int slotHeight = 50;
    int gap = 5;
    int cols = 6;

    painter->setBrush(QColor(0x2d, 0x37, 0x48));
    painter->drawRect(invX - 10, invY - 30, cols * (slotWidth + gap) + 20, 4 * (slotHeight + gap) + 40);

    painter->setPen(QColor(Qt::white));
    painter->setFont(QFont("Arial", 14, QFont::Bold));
    painter->drawText(invX, invY - 15, "背包");

    for (int i = 0; i < 24; ++i) {
        int x = invX + (i % cols) * (slotWidth + gap);
        int y = invY + (i / cols) * (slotHeight + gap);

        if (i == m_selectedSlot) {
            painter->setBrush(QColor(0x4a, 0x55, 0x68));
        } else {
            painter->setBrush(QColor(0x1a, 0x20, 0x2c));
        }

        painter->drawRect(x, y, slotWidth, slotHeight);

        if (i < m_items.size()) {
            painter->setPen(QColor(Qt::white));
            painter->setFont(QFont("Arial", 10));
            QString name = m_items[i]->name();
            if (name.length() > 6) {
                name = name.left(6) + "...";
            }
            painter->drawText(x + 5, y + 20, name);
        }
    }
}