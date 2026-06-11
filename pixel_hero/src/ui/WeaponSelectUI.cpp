#include "WeaponSelectUI.h"
#include <QPainter>
#include <QGraphicsScene>

WeaponSelectUI::WeaponSelectUI(const QList<WeaponData>& weapons,
                               QGraphicsItem* parent)
    : SelectableListBase(weapons.size(), parent)
    , m_weapons(weapons)
{
    setWrapAround(false);
    setConfirmMode(ClickToSelect);
}

WeaponSelectUI::~WeaponSelectUI() {}

QRectF WeaponSelectUI::itemRect(int index) const
{
    int totalW = m_weapons.size() * CARD_W + (m_weapons.size() - 1) * CARD_GAP;
    int startX = (800 - totalW) / 2;
    int x = startX + index * (CARD_W + CARD_GAP);
    return QRectF(x, 90, CARD_W, CARD_H);
}

void WeaponSelectUI::onConfirm()
{
    emit weaponSelected(selectedIndex());
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

void WeaponSelectUI::paint(QPainter* painter, const QStyleOptionGraphicsItem*,
                           QWidget*)
{
    if (!isVisible()) return;

    // 背景遮罩
    painter->setBrush(QColor(0x1a, 0x1a, 0x2e, 210));
    painter->drawRect(0, 0, 800, 600);

    // 标题
    QFont titleFont("Arial", 20, QFont::Bold);
    painter->setFont(titleFont);
    painter->setPen(QColor(0xff, 0xcc, 0x00));
    painter->drawText(QRectF(0, 20, 800, 36), Qt::AlignCenter,
                      QStringLiteral("— 选择你的武器 —"));

    // 提示
    QFont hintFont("Arial", 9);
    painter->setFont(hintFont);
    painter->setPen(QColor(0x88, 0x88, 0x99));
    painter->drawText(QRectF(0, 54, 800, 18), Qt::AlignCenter,
                      QStringLiteral("悬停/点击选择   再次点击确认    Enter 确认    Esc 返回"));

    // 武器卡片
    for (int i = 0; i < m_weapons.size(); ++i) {
        const auto& wp = m_weapons[i];
        QRectF r = itemRect(i);
        bool sel = (i == selectedIndex());

        // 卡片底
        painter->setPen(sel ? QPen(QColor(0xff, 0xcc, 0x00), 3) : QPen(QColor(0x55, 0x60, 0x70), 1));
        painter->setBrush(sel ? QColor(0x35, 0x40, 0x55) : QColor(0x22, 0x28, 0x38));
        painter->drawRoundedRect(r, 10, 10);

        // 武器预览图标
        QRectF icon(r.left() + 30, r.top() + 18, CARD_W - 60, 90);
        QPixmap wpSprite(QString(":/sprites/weapons/%1").arg(wp.id));
        if (!wpSprite.isNull()) {
            qreal sx = icon.left() + (icon.width() - wpSprite.width()) / 2;
            qreal sy = icon.top() + (icon.height() - wpSprite.height()) / 2;
            painter->drawPixmap(QPointF(sx, sy), wpSprite);
        }

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
        drawStatRow(painter, sx, sy, rowW, QStringLiteral("攻击力"), atkStr,
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
            spdStr = QStringLiteral("正常");
            spdCol = QColor(0xcc, 0xcc, 0xcc);
        }
        drawStatRow(painter, sx, sy + 16, rowW, QStringLiteral("攻速"), spdStr, spdCol);

        // 范围
        QString rngStr;
        if (wp.rangeBonus > 0)
            rngStr = QString("+%1").arg((int)wp.rangeBonus);
        else if (wp.rangeBonus < 0)
            rngStr = QString("%1").arg((int)wp.rangeBonus);
        else
            rngStr = QStringLiteral("普通");
        drawStatRow(painter, sx, sy + 32, rowW, QStringLiteral("范围"), rngStr,
                    QColor(0x88, 0x88, 0xff));

        // 技能加成
        QString sklStr;
        if (wp.skillBonus > 1.0f)
            sklStr = QString("+%1%").arg((int)((wp.skillBonus - 1.0f) * 100));
        else
            sklStr = QStringLiteral("无");
        drawStatRow(painter, sx, sy + 48, rowW, QStringLiteral("技能"), sklStr,
                    QColor(0xff, 0x88, 0xff));
    }

    // 底部描述
    int sel = selectedIndex();
    if (sel >= 0 && sel < m_weapons.size()) {
        const auto& wp = m_weapons[sel];
        QFont descFont("Arial", 10);
        painter->setFont(descFont);
        painter->setPen(QColor(0xdd, 0xdd, 0xdd));
        QRectF descRect(60, INFO_Y, 680, 100);
        painter->drawText(descRect, Qt::AlignHCenter | Qt::AlignTop, wp.desc);
    }
}
