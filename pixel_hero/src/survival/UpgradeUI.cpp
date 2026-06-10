#include "UpgradeUI.h"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>
#include <QKeyEvent>
#include <QRandomGenerator>

UpgradeUI::UpgradeUI(QGraphicsItem* parent)
    : QGraphicsItem(parent)
    , m_isVisible(false)
    , m_selectedIndex(0)
{
    setVisible(false);
    setZValue(200);
    setFlag(QGraphicsItem::ItemIsFocusable, true);
    setAcceptHoverEvents(true);
}

UpgradeUI::~UpgradeUI() {}

bool UpgradeUI::isVisible() const { return m_isVisible; }

QRectF UpgradeUI::boundingRect() const
{
    return QRectF(0, 0, 800, 600);
}

void UpgradeUI::showUpgrade(const QList<SkillData>& options)
{
    m_options = options;
    m_selectedIndex = 0;
    m_isVisible = true;
    setVisible(true);
    update();
}

void UpgradeUI::hide()
{
    m_isVisible = false;
    setVisible(false);
}

void UpgradeUI::selectOption(int index)
{
    if (index >= 0 && index < m_options.size()) {
        m_selectedIndex = index;
        emit skillSelected(m_options[index].id);
    }
}

QRectF UpgradeUI::getCardRect(int index) const
{
    int x = START_X + index * (CARD_WIDTH + CARD_GAP);
    return QRectF(x, START_Y, CARD_WIDTH, CARD_HEIGHT);
}

void UpgradeUI::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    if (!m_isVisible) return;

    // 半透明背景
    painter->fillRect(boundingRect(), QColor(0, 0, 0, 160));

    // 标题
    painter->setPen(Qt::white);
    QFont titleFont;
    titleFont.setPixelSize(22);
    titleFont.setBold(true);
    painter->setFont(titleFont);
    painter->drawText(QRectF(0, 100, 800, 40), Qt::AlignCenter, "选择升级技能");

    QFont nameFont;
    nameFont.setPixelSize(14);
    nameFont.setBold(true);

    QFont descFont;
    descFont.setPixelSize(11);

    for (int i = 0; i < m_options.size(); i++) {
        QRectF rect = getCardRect(i);
        bool selected = (i == m_selectedIndex);

        // 卡片背景
        painter->setPen(QPen(selected ? QColor(0xff, 0xcc, 0x00) : QColor(100, 100, 100), 2));
        painter->setBrush(selected ? QColor(50, 50, 70, 220) : QColor(30, 30, 50, 200));
        painter->drawRoundedRect(rect, 8, 8);

        // 技能名称
        const SkillData& sd = m_options[i];
        painter->setPen(Qt::white);
        painter->setFont(nameFont);
        painter->drawText(rect.adjusted(8, 8, -8, -140), Qt::AlignCenter, sd.name);

        // 类型标签
        QString typeStr = (sd.type == "active") ? "主动" : "被动";
        painter->setPen(sd.type == "active" ? QColor(0xff, 0x88, 0x44) : QColor(0x44, 0xaa, 0xff));
        QFont typeFont;
        typeFont.setPixelSize(10);
        painter->setFont(typeFont);
        painter->drawText(rect.adjusted(8, 30, -8, -120), Qt::AlignCenter, typeStr);

        // 描述/效果
        painter->setPen(QColor(200, 200, 200));
        painter->setFont(descFont);
        QString desc = sd.description + "\n\n";
        // 检查是否已拥有
        // (简化处理——显示最高级描述)
        if (sd.levels.isEmpty()) {
            desc += "已满";
        } else {
            desc += sd.levels[0].desc;
        }
        painter->drawText(rect.adjusted(8, 55, -8, -20), Qt::AlignCenter | Qt::TextWordWrap, desc);

        // 快捷键提示
        painter->setPen(QColor(150, 150, 150));
        QFont keyFont;
        keyFont.setPixelSize(10);
        painter->setFont(keyFont);
        painter->drawText(rect.adjusted(8, -30, -8, -8), Qt::AlignCenter,
                          QString("按 %1 选择").arg(i + 1));
    }
}

void UpgradeUI::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (!m_isVisible) return;
    for (int i = 0; i < m_options.size(); i++) {
        if (getCardRect(i).contains(event->pos())) {
            selectOption(i);
            return;
        }
    }
}

void UpgradeUI::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (!m_isVisible) return;
    for (int i = 0; i < m_options.size(); i++) {
        if (getCardRect(i).contains(event->pos())) {
            if (m_selectedIndex != i) { m_selectedIndex = i; update(); }
            return;
        }
    }
}

void UpgradeUI::wheelEvent(QGraphicsSceneWheelEvent* event)
{
    if (!m_isVisible) return;
    if (event->delta() > 0)
        m_selectedIndex = qMax(0, m_selectedIndex - 1);
    else
        m_selectedIndex = qMin(m_options.size() - 1, m_selectedIndex + 1);
    update();
}

void UpgradeUI::keyPressEvent(QKeyEvent* event)
{
    if (!m_isVisible) return;

    switch (event->key()) {
    case Qt::Key_1: case Qt::Key_A: selectOption(0); break;
    case Qt::Key_2: case Qt::Key_S: selectOption(1); break;
    case Qt::Key_3: case Qt::Key_D: selectOption(2); break;
    case Qt::Key_Left:  m_selectedIndex = qMax(0, m_selectedIndex - 1); update(); break;
    case Qt::Key_Right: m_selectedIndex = qMin(m_options.size() - 1, m_selectedIndex + 1); update(); break;
    case Qt::Key_Return: case Qt::Key_Space: selectOption(m_selectedIndex); break;
    default: break;
    }
}
