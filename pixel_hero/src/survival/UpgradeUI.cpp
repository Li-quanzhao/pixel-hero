#include "UpgradeUI.h"
#include <QPainter>

UpgradeUI::UpgradeUI(QGraphicsItem* parent)
    : SelectableListBase(3, parent)
{
    setWrapAround(false);
    setConfirmMode(ClickToSelect);
    setVisible(false);
}

UpgradeUI::~UpgradeUI() {}

void UpgradeUI::showUpgrade(const QList<SkillData>& options)
{
    m_options = options;
    setItemCount(options.size());
    appear();  // 从基类继承: 显示+聚焦+更新
}

void UpgradeUI::hide()
{
    dismiss();  // 从基类继承
}

QRectF UpgradeUI::itemRect(int index) const
{
    int x = START_X + index * (CARD_WIDTH + CARD_GAP);
    return QRectF(x, START_Y, CARD_WIDTH, CARD_HEIGHT);
}

void UpgradeUI::onConfirm()
{
    if (selectedIndex() < m_options.size())
        emit skillSelected(m_options[selectedIndex()].id);
}

bool UpgradeUI::handleExtraKey(int key)
{
    switch (key) {
    case Qt::Key_1: if (0 < m_options.size()) { setSelectedIndex(0); confirm(); } return true;
    case Qt::Key_2: if (1 < m_options.size()) { setSelectedIndex(1); confirm(); } return true;
    case Qt::Key_3: if (2 < m_options.size()) { setSelectedIndex(2); confirm(); } return true;
    case Qt::Key_Escape: hide(); emit skillSkipped(); return true;
    default: return false;
    }
}

void UpgradeUI::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    if (!isVisible()) return;

    // 半透明背景
    painter->fillRect(boundingRect(), QColor(0, 0, 0, 160));

    // 标题
    painter->setPen(Qt::white);
    QFont titleFont;
    titleFont.setPixelSize(22);
    titleFont.setBold(true);
    painter->setFont(titleFont);
    painter->drawText(QRectF(0, 100, 800, 40), Qt::AlignCenter,
                      QStringLiteral("选择升级技能"));

    QFont nameFont;
    nameFont.setPixelSize(14);
    nameFont.setBold(true);

    QFont descFont;
    descFont.setPixelSize(11);

    for (int i = 0; i < m_options.size(); i++) {
        QRectF rect = itemRect(i);
        bool selected = (i == selectedIndex());

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
        QString typeStr = (sd.type == "active") ? QStringLiteral("主动") : QStringLiteral("被动");
        painter->setPen(sd.type == "active" ? QColor(0xff, 0x88, 0x44) : QColor(0x44, 0xaa, 0xff));
        QFont typeFont;
        typeFont.setPixelSize(10);
        painter->setFont(typeFont);
        painter->drawText(rect.adjusted(8, 30, -8, -120), Qt::AlignCenter, typeStr);

        // 描述/效果
        painter->setPen(QColor(200, 200, 200));
        painter->setFont(descFont);
        QString desc = sd.description + "\n\n";
        if (sd.levels.isEmpty()) {
            desc += QStringLiteral("已满");
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
                          QStringLiteral("按 %1 选择").arg(i + 1));
    }
}
