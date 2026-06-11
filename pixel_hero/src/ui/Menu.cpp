#include "Menu.h"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>

Menu::Menu(MenuType type, QGraphicsItem *parent)
    : SelectableListBase(3, parent), m_type(type)
{
    setWrapAround(true);
    setConfirmMode(ClickToConfirm);
    setMenuType(type);
}

Menu::~Menu()
{
}

void Menu::setMenuType(MenuType type)
{
    m_type = type;
    m_menuItems.clear();
    m_statsText.clear();

    switch (type) {
    case MAIN_MENU:
        m_menuItems << QStringLiteral("开始游戏") << QStringLiteral("读取存档") << QStringLiteral("退出游戏");
        break;
    case PAUSE_MENU:
        m_menuItems << QStringLiteral("继续游戏") << QStringLiteral("保存并退出") << QStringLiteral("退出游戏(不保存)");
        break;
    case GAME_OVER_MENU:
        m_menuItems << QStringLiteral("重新开始") << QStringLiteral("返回主菜单") << QStringLiteral("退出游戏");
        break;
    }
    setItemCount(3);
}

void Menu::setGameOverStats(const QString& statsText)
{
    m_statsText = statsText;
}

QRectF Menu::itemRect(int index) const
{
    int itemY = MENU_Y + index * (MENU_HEIGHT + ITEM_SPACING);
    return QRectF(MENU_X, itemY, MENU_WIDTH, MENU_HEIGHT);
}

void Menu::onConfirm()
{
    switch (m_type) {
    case MAIN_MENU:
        switch (selectedIndex()) {
        case 0: emit startGame(); break;
        case 1: emit loadGame(); break;
        case 2: emit quitGame(); break;
        }
        break;

    case PAUSE_MENU:
        switch (selectedIndex()) {
        case 0: emit resumeGame(); break;
        case 1: emit saveGame(); break;
        case 2: emit quitGame(); break;
        }
        break;

    case GAME_OVER_MENU:
        switch (selectedIndex()) {
        case 0: emit startGame(); break;
        case 1: emit quitGame(); break;
        case 2: emit quitGame(); break;
        }
        break;
    }
}

void Menu::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (!isVisible()) return;

    // 半透明背景遮罩
    painter->setBrush(QColor(0x1a, 0x1a, 0x2e, 200));
    painter->drawRect(0, 0, 800, 600);

    // 标题背景
    painter->setBrush(QColor(0x2d, 0x37, 0x48));
    painter->drawRect(MENU_X, MENU_Y - 60, MENU_WIDTH, 40);

    // 标题文字
    painter->setPen(QColor(Qt::white));
    painter->setFont(QFont("Arial", 16, QFont::Bold));

    QString title;
    switch (m_type) {
    case MAIN_MENU:    title = QStringLiteral("像素勇者"); break;
    case PAUSE_MENU:   title = QStringLiteral("游戏暂停"); break;
    case GAME_OVER_MENU: title = QStringLiteral("游戏结束"); break;
    }

    painter->drawText(MENU_X + (MENU_WIDTH - painter->fontMetrics().horizontalAdvance(title)) / 2,
                      MENU_Y - 35, title);

    // GAME_OVER模式: 显示统计数据
    if (m_type == GAME_OVER_MENU && !m_statsText.isEmpty()) {
        QFont statsFont("Arial", 13);
        painter->setFont(statsFont);
        painter->setPen(QColor(0xdd, 0xdd, 0xdd));
        QRectF statsRect(MENU_X - 60, MENU_Y - 20, MENU_WIDTH + 120, 100);
        painter->drawText(statsRect, Qt::AlignHCenter | Qt::AlignTop, m_statsText);
    }

    // 菜单项
    painter->setFont(QFont("Arial", 14));

    for (int i = 0; i < m_menuItems.size(); ++i) {
        QRectF rect = itemRect(i);

        // 选中项高亮
        if (i == selectedIndex()) {
            painter->setBrush(QColor(0x4a, 0x55, 0x68));
            painter->setPen(QColor(0xff, 0xcc, 0x00));
        } else if (isItemDisabled(i)) {
            painter->setBrush(QColor(0x20, 0x25, 0x30));
            painter->setPen(QColor(0x66, 0x66, 0x66));
        } else {
            painter->setBrush(QColor(0x2d, 0x37, 0x48));
            painter->setPen(QColor(Qt::white));
        }

        painter->drawRect(rect);

        QString text = m_menuItems[i];
        painter->drawText(rect.left() + (MENU_WIDTH - painter->fontMetrics().horizontalAdvance(text)) / 2,
                          rect.top() + 28, text);
    }
}
