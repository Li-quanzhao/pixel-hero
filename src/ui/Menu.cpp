#include "Menu.h"
#include <QPainter>

Menu::Menu(MenuType type, QGraphicsItem *parent)
    : QObject(nullptr), QGraphicsItem(parent), m_type(type), m_isVisible(true), m_selectedIndex(0)
{
    setZValue(200);
    setMenuType(type);
}

Menu::~Menu()
{
}

void Menu::setMenuType(MenuType type)
{
    m_type = type;
    m_menuItems.clear();

    switch (type) {
    case MAIN_MENU:
        m_menuItems << "开始游戏" << "读取存档" << "退出游戏";
        break;
    case PAUSE_MENU:
        m_menuItems << "继续游戏" << "保存游戏" << "读取存档" << "退出游戏";
        break;
    case GAME_OVER_MENU:
        m_menuItems << "重新开始" << "返回主菜单" << "退出游戏";
        break;
    }
}

void Menu::show()
{
    m_isVisible = true;
    update();
}

void Menu::hide()
{
    m_isVisible = false;
    update();
}

bool Menu::isVisible() const
{
    return m_isVisible;
}

QRectF Menu::boundingRect() const
{
    return QRectF(0, 0, 800, 600);
}

void Menu::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (!m_isVisible) return;

    painter->setBrush(QColor(0x1a, 0x1a, 0x2e, 200));
    painter->drawRect(0, 0, 800, 600);

    int menuX = 300;
    int menuY = 200;
    int menuWidth = 200;
    int menuHeight = 40;

    painter->setBrush(QColor(0x2d, 0x37, 0x48));
    painter->drawRect(menuX, menuY - 60, menuWidth, 40);

    painter->setPen(QColor(Qt::white));
    painter->setFont(QFont("Arial", 16, QFont::Bold));

    QString title;
    switch (m_type) {
    case MAIN_MENU: title = "像素勇者"; break;
    case PAUSE_MENU: title = "游戏暂停"; break;
    case GAME_OVER_MENU: title = "游戏结束"; break;
    }

    painter->drawText(menuX + (menuWidth - painter->fontMetrics().horizontalAdvance(title)) / 2, menuY - 35, title);

    painter->setFont(QFont("Arial", 14));

    for (int i = 0; i < m_menuItems.size(); ++i) {
        int itemY = menuY + i * (menuHeight + 10);

        if (i == m_selectedIndex) {
            painter->setBrush(QColor(0x4a, 0x55, 0x68));
        } else {
            painter->setBrush(QColor(0x2d, 0x37, 0x48));
        }

        painter->drawRect(menuX, itemY, menuWidth, menuHeight);

        QString text = m_menuItems[i];
        painter->drawText(menuX + (menuWidth - painter->fontMetrics().horizontalAdvance(text)) / 2, itemY + 28, text);
    }
}