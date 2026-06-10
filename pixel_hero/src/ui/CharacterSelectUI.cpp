#include "CharacterSelectUI.h"
#include <QPainter>
#include <QGraphicsScene>

CharacterSelectUI::CharacterSelectUI(const QList<CharacterInfo>& characters,
                                     QGraphicsItem* parent)
    : QObject(nullptr), QGraphicsItem(parent)
    , m_characters(characters)
    , m_visible(false), m_selectedIndex(0)
{
    setZValue(200);
    setFlag(QGraphicsItem::ItemIsFocusable, true);
    setAcceptHoverEvents(true);
}

CharacterSelectUI::~CharacterSelectUI() {}

void CharacterSelectUI::appear()
{
    m_visible = true;
    m_selectedIndex = 0;
    show();
    setFocus();
    update();
}

void CharacterSelectUI::dismiss()
{
    m_visible = false;
    hide();
    clearFocus();
    update();
}

void CharacterSelectUI::prev()
{
    if (m_visible && m_selectedIndex > 0) {
        m_selectedIndex--;
        update();
    }
}

void CharacterSelectUI::next()
{
    if (m_visible && m_selectedIndex < m_characters.size() - 1) {
        m_selectedIndex++;
        update();
    }
}

void CharacterSelectUI::confirm()
{
    if (m_visible)
        emit characterSelected(m_selectedIndex);
}

QRectF CharacterSelectUI::boundingRect() const
{
    return QRectF(0, 0, 800, 600);
}

QRectF CharacterSelectUI::cardRect(int index) const
{
    int totalW = m_characters.size() * CARD_W + (m_characters.size() - 1) * CARD_GAP;
    int startX = (800 - totalW) / 2;
    int x = startX + index * (CARD_W + CARD_GAP);
    return QRectF(x, 80, CARD_W, CARD_H);
}

void CharacterSelectUI::drawStatBar(QPainter* p, qreal x, qreal y, int w,
                                    const QString& label, int val, int maxVal,
                                    const QColor& barColor) const
{
    // 标签
    QFont f("Arial", 8);
    p->setFont(f);
    p->setPen(QColor(0xcc, 0xcc, 0xcc));
    int labelW = QFontMetrics(f).horizontalAdvance(label);
    p->drawText(QPointF(x, y + 11), label);

    // 数值
    f.setPixelSize(9);
    p->setFont(f);
    p->setPen(Qt::white);
    QString valText = QString::number(val);
    p->drawText(QPointF(x + labelW + 6, y + 11), valText);

    // 条形图
    qreal barX = x + labelW + 32;
    qreal barW = w - labelW - 36;
    qreal barH = 8;
    qreal barY = y + 4;

    p->setBrush(QColor(0x33, 0x33, 0x44));
    p->setPen(Qt::NoPen);
    p->drawRoundedRect(QRectF(barX, barY, barW, barH), 3, 3);

    if (maxVal > 0) {
        qreal fillW = barW * qMin(val, maxVal) / maxVal;
        p->setBrush(barColor);
        p->drawRoundedRect(QRectF(barX, barY, fillW, barH), 3, 3);
    }
}

void CharacterSelectUI::keyPressEvent(QKeyEvent* event)
{
    if (!m_visible) {
        QGraphicsItem::keyPressEvent(event);
        return;
    }

    int key = event->key();
    if (key == Qt::Key_Left || key == Qt::Key_A) {
        if (m_selectedIndex > 0) { m_selectedIndex--; update(); }
    } else if (key == Qt::Key_Right || key == Qt::Key_D) {
        if (m_selectedIndex < m_characters.size() - 1) { m_selectedIndex++; update(); }
    } else if (key == Qt::Key_Return || key == Qt::Key_Enter || key == Qt::Key_Space) {
        emit characterSelected(m_selectedIndex);
    } else {
        QGraphicsItem::keyPressEvent(event);
    }
}

// ---- 鼠标 ----
void CharacterSelectUI::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (!m_visible) return;
    for (int i = 0; i < m_characters.size(); ++i) {
        if (cardRect(i).contains(event->pos())) {
            if (m_selectedIndex == i)
                emit characterSelected(m_selectedIndex); // 已选中，再次点击触发
            else {
                m_selectedIndex = i;                     // 首次点击，选中
                update();
            }
            return;
        }
    }
}

void CharacterSelectUI::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (!m_visible) return;
    for (int i = 0; i < m_characters.size(); ++i) {
        if (cardRect(i).contains(event->pos())) {
            if (m_selectedIndex != i) { m_selectedIndex = i; update(); }
            return;
        }
    }
}

void CharacterSelectUI::wheelEvent(QGraphicsSceneWheelEvent* event)
{
    if (!m_visible) return;
    if (event->delta() > 0) prev(); else next();
}

void CharacterSelectUI::paint(QPainter* painter, const QStyleOptionGraphicsItem*,
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
                      "— 选择你的英雄 —");

    // 提示
    QFont hintFont("Arial", 9);
    painter->setFont(hintFont);
    painter->setPen(QColor(0x88, 0x88, 0x99));
    painter->drawText(QRectF(0, 54, 800, 18), Qt::AlignCenter,
                      "悬停/点击选择   再次点击确认    Enter 确认    Esc 返回");

    // 角色卡片
    for (int i = 0; i < m_characters.size(); ++i) {
        const auto& ch = m_characters[i];
        QRectF r = cardRect(i);
        bool sel = (i == m_selectedIndex);

        // 卡片底
        painter->setPen(sel ? QPen(QColor(0xff, 0xcc, 0x00), 3) : QPen(QColor(0x55, 0x60, 0x70), 1));
        painter->setBrush(sel ? QColor(0x35, 0x40, 0x55) : QColor(0x22, 0x28, 0x38));
        painter->drawRoundedRect(r, 10, 10);

        // 颜色预览方块
        QRectF avatar(r.left() + 30, r.top() + 16, CARD_W - 60, CARD_W - 60);
        painter->setPen(Qt::NoPen);
        painter->setBrush(ch.color);
        painter->drawRoundedRect(avatar, 8, 8);

        // 名称
        QFont nameFont("Arial", 13, QFont::Bold);
        painter->setFont(nameFont);
        painter->setPen(sel ? QColor(0xff, 0xcc, 0x00) : Qt::white);
        painter->drawText(QRectF(r.left() + 8, avatar.bottom() + 6,
                                 CARD_W - 16, 22), Qt::AlignCenter, ch.name);

        // 属性条
        // 用最大值映射: HP=150,ATK=25,DEF=6,SPD=6
        int barW = CARD_W - 32;
        qreal sx = r.left() + 16;
        qreal sy = avatar.bottom() + 30;
        drawStatBar(painter, sx, sy,      barW, "HP",  ch.hp,  150, QColor(0xe0, 0x40, 0x40));
        drawStatBar(painter, sx, sy + 14, barW, "ATK", ch.atk, 25,  QColor(0xff, 0x88, 0x00));
        drawStatBar(painter, sx, sy + 28, barW, "DEF", ch.def, 6,   QColor(0x40, 0x80, 0xe0));
        drawStatBar(painter, sx, sy + 42, barW, "SPD", ch.spd, 6,   QColor(0x40, 0xc0, 0x60));
    }

    // 底部描述
    if (m_selectedIndex >= 0 && m_selectedIndex < m_characters.size()) {
        const auto& ch = m_characters[m_selectedIndex];
        QFont descFont("Arial", 10);
        painter->setFont(descFont);
        painter->setPen(QColor(0xdd, 0xdd, 0xdd));
        QRectF descRect(60, INFO_Y, 680, 200);
        painter->drawText(descRect, Qt::AlignHCenter | Qt::AlignTop, ch.desc);
    }
}
