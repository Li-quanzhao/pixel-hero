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

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

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
};

#endif // MENU_H