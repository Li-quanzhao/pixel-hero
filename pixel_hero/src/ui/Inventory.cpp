#include "Inventory.h"
#include "Weapon.h"
#include "Armor.h"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QMessageBox>

Inventory::Inventory(QGraphicsItem *parent)
    : QObject(nullptr), QGraphicsItem(parent)
    , m_player(nullptr), m_isVisible(false)
    , m_selectedSlot(0), m_slotCount(24)
{
    setZValue(200);
    setAcceptHoverEvents(true);
}

Inventory::~Inventory() {}

void Inventory::setPlayer(Player* player) { m_player = player; refresh(); }
void Inventory::refresh() { update(); }

QRectF Inventory::getSlotRect(int i) const
{
    int x = START_X + (i % COLS) * (SLOT_SIZE + GAP);
    int y = START_Y + (i / COLS) * (SLOT_SIZE + GAP);
    return QRectF(x, y, SLOT_SIZE, SLOT_SIZE);
}

void Inventory::show()  { m_isVisible = true;  update(); }
void Inventory::hide()  { m_isVisible = false; update(); emit closed(); }
bool Inventory::isVisible() const { return m_isVisible; }

void Inventory::selectPrevious()
{
    if (m_selectedSlot > 0) { m_selectedSlot--; update(); }
}
void Inventory::selectNext()
{
    if (m_selectedSlot < m_slotCount - 1) { m_selectedSlot++; update(); }
}
void Inventory::confirmItem()
{
    if (!m_player) return;
    Item* item = m_player->itemAt(m_selectedSlot);
    if (!item) return;

    QString typeName;
    switch (item->type()) {
    case Item::WEAPON: typeName = "武器"; break;
    case Item::ARMOR:  typeName = "护甲"; break;
    case Item::CONSUMABLE: typeName = "消耗品"; break;
    default: typeName = "物品";
    }

    if (item->type() == Item::WEAPON) {
        Weapon* w = dynamic_cast<Weapon*>(item);
        if (w) {
            m_player->removeItemFromInventory(m_selectedSlot);
            m_player->equipWeapon(w);
            m_selectedSlot = qMax(0, m_selectedSlot - 1);
            refresh();
        }
    } else if (item->type() == Item::ARMOR) {
        Armor* a = dynamic_cast<Armor*>(item);
        if (a) {
            m_player->removeItemFromInventory(m_selectedSlot);
            m_player->equipArmor(a);
            m_selectedSlot = qMax(0, m_selectedSlot - 1);
            refresh();
        }
    } else if (item->type() == Item::CONSUMABLE) {
        if (item->id() == "potion_hp") {
            m_player->setHealth(m_player->health() + 50);
            m_player->removeItemFromInventory(m_selectedSlot);
            refresh();
        }
    }

    emit itemUsed(m_selectedSlot);
}

QRectF Inventory::boundingRect() const { return QRectF(0, 0, 800, 600); }

void Inventory::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option); Q_UNUSED(widget);
    if (!m_isVisible) return;

    painter->setBrush(QColor(0x0a, 0x0a, 0x1e, 220));
    painter->drawRect(0, 0, 800, 600);

    int totalW = COLS * SLOT_SIZE + (COLS - 1) * GAP;
    int totalH = 4 * SLOT_SIZE + 3 * GAP;

    // 背景面板
    painter->setBrush(QColor(0x2d, 0x37, 0x48));
    painter->drawRect(START_X - 10, START_Y - 40, totalW + 20, totalH + 60);

    // 标题
    painter->setPen(QColor(0xff, 0xcc, 0x00));
    painter->setFont(QFont("Arial", 14, QFont::Bold));
    painter->drawText(START_X, START_Y - 20, "背 包 (I键关闭)");

    // 装备区
    painter->setPen(QColor(Qt::white));
    painter->setFont(QFont("Arial", 11));
    int equipY = START_Y;
    painter->drawText(START_X + totalW + 20, equipY, "装备栏");
    painter->drawText(START_X + totalW + 20, equipY + 30,
        m_player && m_player->getEquippedWeapon()
            ? QString("武器: ") + m_player->getEquippedWeapon()->name()
            : "武器: 无");
    painter->drawText(START_X + totalW + 20, equipY + 50,
        m_player && m_player->getEquippedArmor()
            ? QString("护甲: ") + m_player->getEquippedArmor()->name()
            : "护甲: 无");

    // 金币
    painter->setPen(QColor(0xff, 0xcc, 0x00));
    painter->drawText(START_X + totalW + 20, equipY + 80,
        QString("金币: %1").arg(m_player ? m_player->gold() : 0));

    // 格子
    for (int i = 0; i < m_slotCount; ++i) {
        QRectF r = getSlotRect(i);

        if (i == m_selectedSlot) {
            painter->setBrush(QColor(0x4a, 0x55, 0x68));
            painter->setPen(QPen(QColor(0xff, 0xcc, 0x00), 2));
        } else {
            painter->setBrush(QColor(0x1a, 0x20, 0x2c));
            painter->setPen(QPen(QColor(0x3a, 0x45, 0x58), 1));
        }
        painter->drawRect(r);

        if (m_player) {
            Item* item = m_player->itemAt(i);
            if (item) {
                painter->setPen(QColor(Qt::white));
                painter->setFont(QFont("Arial", 9));
                QString name = item->name();
                if (name.length() > 6) name = name.left(5) + "..";

                // 类型颜色
                QColor typeColor = Qt::white;
                switch (item->type()) {
                case Item::WEAPON:    typeColor = QColor(0xe5, 0x3e, 0x3e); break;
                case Item::ARMOR:     typeColor = QColor(0x31, 0x82, 0xce); break;
                case Item::CONSUMABLE:typeColor = QColor(0x38, 0xa1, 0x69); break;
                default: break;
                }
                painter->setPen(typeColor);
                painter->drawText(r.x() + 3, r.y() + 18, name);

                painter->setPen(QColor(0xaa, 0xaa, 0xaa));
                painter->setFont(QFont("Arial", 7));
                painter->drawText(r.x() + 3, r.y() + 34,
                    QString("售价:%1").arg(item->price()));
            }
        }
    }

    // 选中物品详情
    if (m_player) {
        Item* sel = m_player->itemAt(m_selectedSlot);
        if (sel) {
            int infoX = START_X + totalW + 20;
            painter->setPen(QColor(0xff, 0xcc, 0x00));
            painter->setFont(QFont("Arial", 11, QFont::Bold));
            painter->drawText(infoX, equipY + 110, "物品详情");
            painter->setPen(QColor(Qt::white));
            painter->setFont(QFont("Arial", 10));
            painter->drawText(infoX, equipY + 130, QString("名称: %1").arg(sel->name()));
            painter->drawText(infoX, equipY + 148, sel->description());

            if (sel->type() == Item::WEAPON) {
                Weapon* w = dynamic_cast<Weapon*>(sel);
                if (w) painter->drawText(infoX, equipY + 166, QString("攻击+%1").arg(w->attackBonus()));
            } else if (sel->type() == Item::ARMOR) {
                Armor* a = dynamic_cast<Armor*>(sel);
                if (a) painter->drawText(infoX, equipY + 166, QString("防御+%1").arg(a->defenseBonus()));
            }
        }
    }
}

void Inventory::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (!m_isVisible) return;
    QPointF pos = event->pos();
    for (int i = 0; i < m_slotCount; ++i) {
        if (getSlotRect(i).contains(pos)) {
            m_selectedSlot = i;
            confirmItem();
            return;
        }
    }
    QGraphicsItem::mousePressEvent(event);
}
