#ifndef HUD_H
#define HUD_H

#include <QGraphicsItem>
#include <QRectF>
#include "Player.h"

class HUD : public QGraphicsItem
{
public:
    HUD(QGraphicsItem *parent = nullptr);
    ~HUD();

    void setPlayer(Player* player);
    void updateHUD();

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    Player *m_player;
    int m_health;
    int m_maxHealth;
    int m_level;
    int m_exp;
    int m_gold;
};

#endif // HUD_H