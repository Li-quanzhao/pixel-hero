#include "Menu.h"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>

Menu::Menu(MenuType type, QGraphicsItem *parent)
    : QObject(nullptr), QGraphicsItem(parent), m_type(type), m_isVisible(true), m_selectedIndex(0)
{
    setZValue(200);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemIsFocusable, true);
    setAcceptHoverEvents(true);
    setMenuType(type);
}

Menu::~Menu()
{
}

void Menu::setMenuType(MenuType type)
{
    m_type = type;
    m_menuItems.clear();
    m_selectedIndex = 0;

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

void Menu::selectPrevious()
{
    if (m_selectedIndex > 0) {
        m_selectedIndex--;
        update();
    }
}

void Menu::selectNext()
{
    if (m_selectedIndex < m_menuItems.size() - 1) {
        m_selectedIndex++;
        update();
    }
}

void Menu::confirmSelection()
{
    switch (m_type) {
    case MAIN_MENU:
        switch (m_selectedIndex) {
        case 0: emit startGame(); break;
        case 1: emit loadGame(); break;
        case 2: emit quitGame(); break;
        }
        break;

    case PAUSE_MENU:
        switch (m_selectedIndex) {
        case 0: emit resumeGame(); break;
        case 1: emit saveGame(); break;
        case 2: emit loadGame(); break;
        case 3: emit quitGame(); break;
        }
        break;

    case GAME_OVER_MENU:
        switch (m_selectedIndex) {
        case 0: emit startGame(); break;
        case 1: emit quitGame(); break;
        case 2: emit quitGame(); break;
        }
        break;
    }
}

QRectF Menu::boundingRect() const
{
    return QRectF(0, 0, 800, 600);
}

QRectF Menu::getItemRect(int index) const
{
    int itemY = MENU_Y + index * (MENU_HEIGHT + ITEM_SPACING);
    return QRectF(MENU_X, itemY, MENU_WIDTH, MENU_HEIGHT);
}

void Menu::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (!m_isVisible) return;

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
    case MAIN_MENU: title = "像素勇者"; break;
    case PAUSE_MENU: title = "游戏暂停"; break;
    case GAME_OVER_MENU: title = "游戏结束"; break;
    }

    painter->drawText(MENU_X + (MENU_WIDTH - painter->fontMetrics().horizontalAdvance(title)) / 2, MENU_Y - 35, title);

    // 菜单项
    painter->setFont(QFont("Arial", 14));

    for (int i = 0; i < m_menuItems.size(); ++i) {
        QRectF itemRect = getItemRect(i);

        // 选中项高亮
        if (i == m_selectedIndex) {
            painter->setBrush(QColor(0x4a, 0x55, 0x68));
            painter->setPen(QColor(0xff, 0xcc, 0x00));
        } else {
            painter->setBrush(QColor(0x2d, 0x37, 0x48));
            painter->setPen(QColor(Qt::white));
        }

        painter->drawRect(itemRect);

        QString text = m_menuItems[i];
        painter->drawText(itemRect.left() + (MENU_WIDTH - painter->fontMetrics().horizontalAdvance(text)) / 2,
                         itemRect.top() + 28, text);
    }
}

void Menu::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (!m_isVisible) return;

    QPointF pos = event->pos();

    for (int i = 0; i < m_menuItems.size(); ++i) {
        if (getItemRect(i).contains(pos)) {
            m_selectedIndex = i;
            confirmSelection();
            // confirmSelection() 可能触发菜单删除
            // deleteLater 已调度删除，直接返回
            return;
        }
    }

    QGraphicsItem::mousePressEvent(event);
}

void Menu::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!m_isVisible) return;

    QPointF pos = event->pos();

    // 检查鼠标悬停在哪个菜单项上
    for (int i = 0; i < m_menuItems.size(); ++i) {
        if (getItemRect(i).contains(pos)) {
            if (m_selectedIndex != i) {
                m_selectedIndex = i;
                update();
            }
            break;
        }
    }

    QGraphicsItem::mouseMoveEvent(event);
}
