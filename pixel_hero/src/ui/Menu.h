#ifndef MENU_H
#define MENU_H

#include <QObject>
#include <QString>
#include <QList>
#include "SelectableListBase.h"

class Menu : public SelectableListBase
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
    void setGameOverStats(const QString& statsText);  // GAME_OVER模式显示统计数据

    // 键盘操作 (保留原方法名兼容 GameWindow eventFilter)
    void selectPrevious() { selectPrev(); }
    void selectNext()     { SelectableListBase::selectNext(); }
    void confirmSelection() { confirm(); }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

signals:
    void startGame();
    void resumeGame();
    void saveGame();
    void loadGame();
    void quitGame();

protected:
    QRectF itemRect(int index) const override;
    void onConfirm() override;

private:
    MenuType m_type;
    QList<QString> m_menuItems;
    QString  m_statsText;  // GAME_OVER模式统计数据

    // 菜单布局参数
    static constexpr int MENU_X      = 300;
    static constexpr int MENU_Y      = 200;
    static constexpr int MENU_WIDTH  = 200;
    static constexpr int MENU_HEIGHT = 40;
    static constexpr int ITEM_SPACING = 10;
};

#endif // MENU_H
