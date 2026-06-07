#ifndef MENU_H
#define MENU_H

#include <QObject>
#include <QGraphicsItem>
#include <QRectF>
#include <QString>
#include <QList>

class Menu : public QObject, public QGraphicsItem
{
    Q_OBJECT

public:
    enum MenuType {
        MAIN_MENU,
        PAUSE_MENU,
        GAME_OVER_MENU
    };

    Menu(MenuType type, QGraphicsItem *parent = nullptr);
    ~Menu();

    void setMenuType(MenuType type);
    void show();
    void hide();
    bool isVisible() const;

    // 键盘操作
    void selectPrevious();
    void selectNext();
    void confirmSelection();

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    // 鼠标事件
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

signals:
    void startGame();
    void resumeGame();
    void saveGame();
    void loadGame();
    void quitGame();

private:
    MenuType m_type;
    bool m_isVisible;
    QList<QString> m_menuItems;
    int m_selectedIndex;

    // 菜单布局参数
    const int MENU_X = 300;
    const int MENU_Y = 200;
    const int MENU_WIDTH = 200;
    const int MENU_HEIGHT = 40;
    const int ITEM_SPACING = 10;

    // 获取菜单项的矩形区域
    QRectF getItemRect(int index) const;
};

#endif // MENU_H
