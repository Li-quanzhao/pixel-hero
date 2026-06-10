#include "WeaponSelectUI.h"
#include <QPainter>
#include <QGraphicsScene>

WeaponSelectUI::WeaponSelectUI(const QList<WeaponData>& weapons,
                               QGraphicsItem* parent)
    : QObject(nullptr), QGraphicsItem(parent)
    , m_weapons(weapons)
    , m_visible(false), m_selectedIndex(0)
{
    setZValue(201);
    setFlag(QGraphicsItem::ItemIsFocusable, true);
    setAcceptHoverEvents(true);
}

WeaponSelectUI::~WeaponSelectUI() {}

void WeaponSelectUI::appear()
{
    m_visible = true;
    m_selectedIndex = 0;
    show();
    setFocus();
    update();
}

void WeaponSelectUI::dismiss()
{
    m_visible = false;
    hide();
    clearFocus();
    update();
}

void WeaponSelectUI::prev()
{
    if (m_visible && m_selectedIndex > 0) {
        m_selectedIndex--;
        update();
    }
}

void WeaponSelectUI::next()
{
    if (m_visible && m_selectedIndex < m_weapons.size() - 1) {
        m_selectedIndex++;
        update();
    }
}

void WeaponSelectUI::confirm()
{
    if (m_visible)
        emit weaponSelected(m_selectedIndex);
}

QRectF WeaponSelectUI::boundingRect() const
{
    return QRectF(0, 0, 800, 600);
}

QRectF WeaponSelectUI::cardRect(int index) const
{
    int totalW = m_weapons.size() * CARD_W + (m_weapons.size() - 1) * CARD_GAP;
    int startX = (800 - totalW) / 2;
    int x = startX + index * (CARD_W + CARD_GAP);
    return QRectF(x, 90, CARD_W, CARD_H);
}

void WeaponSelectUI::drawStatRow(QPainter* p, qreal x, qreal y, int w,
                                 const QString& label, const QString& value,
                                 const QColor& valColor) const
{
    QFont f("Arial", 9);
    p->setFont(f);
    p->setPen(QColor(0xcc, 0xcc, 0xcc));
    p->drawText(QPointF(x, y + 11), label);

    p->setPen(valColor);
    int valW = QFontMetrics(f).horizontalAdvance(value);
    p->drawText(QPointF(x + w - valW, y + 11), value);
}

void WeaponSelectUI::keyPressEvent(QKeyEvent* event)
{
    if (!m_visible) {
        QGraphicsItem::keyPressEvent(event);
        return;
    }

    int key = event->key();
    if (key == Qt::Key_Left || key == Qt::Key_A) {
        if (m_selectedIndex > 0) { m_selectedIndex--; update(); }
    } else if (key == Qt::Key_Right || key == Qt::Key_D) {
        if (m_selectedIndex < m_weapons.size() - 1) { m_selectedIndex++; update(); }
    } else if (key == Qt::Key_Return || key == Qt::Key_Enter || key == Qt::Key_Space) {
        emit weaponSelected(m_selectedIndex);
    } else {
        QGraphicsItem::keyPressEvent(event);
    }
}

// ---- 鼠标 ----
void WeaponSelectUI::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (!m_visible) return;
    for (int i = 0; i < m_weapons.size(); ++i) {
        if (cardRect(i).contains(event->pos())) {
            if (m_selectedIndex == i)
                emit weaponSelected(m_selectedIndex);    // 已选中，再次点击触发
            else {
                m_selectedIndex = i;                     // 首次点击，选中
                update();
            }
            return;
        }
    }
}

void WeaponSelectUI::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (!m_visible) return;
    for (int i = 0; i < m_weapons.size(); ++i) {
        if (cardRect(i).contains(event->pos())) {
            if (m_selectedIndex != i) { m_selectedIndex = i; update(); }
            return;
        }
    }
}

void WeaponSelectUI::wheelEvent(QGraphicsSceneWheelEvent* event)
{
    if (!m_visible) return;
    if (event->delta() > 0) prev(); else next();
}

void WeaponSelectUI::paint(QPainter* painter, const QStyleOptionGraphicsItem*,
                           QWidget*)
{
    if (!m_visible) return;

    // 背景遮罩
    painter->setBrush(QColor(0x1a, 0x1a, 0x2e, 210));
    painter->drawRect(0, 0, 800, 600);

    // 标题
    QFont titleFont("Arial", 20, QFont::Bold);
    painter->setFont(titleFont);
    painter->setPen(QColor(0xff, 0xcc, 0x00));
    painter->drawText(QRectF(0, 20, 800, 36), Qt::AlignCenter,
                      "— 选择你的武器 —");

    // 提示
    QFont hintFont("Arial", 9);
    painter->setFont(hintFont);
    painter->setPen(QColor(0x88, 0x88, 0x99));
    painter->drawText(QRectF(0, 54, 800, 18), Qt::AlignCenter,
                      "悬停/点击选择   再次点击确认    Enter 确认    Esc 返回");

    // 武器卡片
    for (int i = 0; i < m_weapons.size(); ++i) {
        const auto& wp = m_weapons[i];
        QRectF r = cardRect(i);
        bool sel = (i == m_selectedIndex);

        // 卡片底
        painter->setPen(sel ? QPen(QColor(0xff, 0xcc, 0x00), 3) : QPen(QColor(0x55, 0x60, 0x70), 1));
        painter->setBrush(sel ? QColor(0x35, 0x40, 0x55) : QColor(0x22, 0x28, 0x38));
        painter->drawRoundedRect(r, 10, 10);

        // 武器预览 (图标区域)
        QRectF icon(r.left() + 30, r.top() + 18, CARD_W - 60, 90);
        painter->setPen(Qt::NoPen);
        painter->setBrush(wp.color);
        painter->drawRoundedRect(icon, 10, 10);

        // 武器名称
        QFont nameFont("Arial", 14, QFont::Bold);
        painter->setFont(nameFont);
        painter->setPen(sel ? QColor(0xff, 0xcc, 0x00) : Qt::white);
        painter->drawText(QRectF(r.left() + 8, icon.bottom() + 8,
                                 CARD_W - 16, 22), Qt::AlignCenter, wp.name);

        // 属性行
        int rowW = CARD_W - 32;
        qreal sx = r.left() + 16;
        qreal sy = icon.bottom() + 34;

        // ATK
        QString atkStr = QString("+%1").arg(wp.attackBonus);
        drawStatRow(painter, sx, sy, rowW, "攻击力", atkStr,
                    QColor(0xff, 0x88, 0x00));

        // 攻速
        QString spdStr;
        QColor spdCol;
        if (wp.speedMod > 1.0f) {
            spdStr = QString("+%1%").arg((int)((wp.speedMod - 1.0f) * 100));
            spdCol = QColor(0x40, 0xc0, 0x60);
        } else if (wp.speedMod < 1.0f) {
            spdStr = QString("-%1%").arg((int)((1.0f - wp.speedMod) * 100));
            spdCol = QColor(0xe0, 0x60, 0x60);
        } else {
            spdStr = "正常";
            spdCol = QColor(0xcc, 0xcc, 0xcc);
        }
        drawStatRow(painter, sx, sy + 16, rowW, "攻速", spdStr, spdCol);

        // 范围
        QString rngStr;
        if (wp.rangeBonus > 0)
            rngStr = QString("+%1").arg((int)wp.rangeBonus);
        else if (wp.rangeBonus < 0)
            rngStr = QString("%1").arg((int)wp.rangeBonus);
        else
            rngStr = "普通";
        drawStatRow(painter, sx, sy + 32, rowW, "范围", rngStr,
                    QColor(0x88, 0x88, 0xff));

        // 技能加成
        QString sklStr;
        if (wp.skillBonus > 1.0f)
            sklStr = QString("+%1%").arg((int)((wp.skillBonus - 1.0f) * 100));
        else
            sklStr = "无";
        drawStatRow(painter, sx, sy + 48, rowW, "技能", sklStr,
                    QColor(0xff, 0x88, 0xff));
    }

    // 底部描述
    if (m_selectedIndex >= 0 && m_selectedIndex < m_weapons.size()) {
        const auto& wp = m_weapons[m_selectedIndex];
        QFont descFont("Arial", 10);
        painter->setFont(descFont);
        painter->setPen(QColor(0xdd, 0xdd, 0xdd));
        QRectF descRect(60, INFO_Y, 680, 100);
        painter->drawText(descRect, Qt::AlignHCenter | Qt::AlignTop, wp.desc);
    }
}
