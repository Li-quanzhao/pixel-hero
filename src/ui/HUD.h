#ifndef HUD_H
#define HUD_H

#include <QObject>
#include <QGraphicsItem>
#include <QRectF>
#include <QTimer>
#include "Player.h"

class HUD : public QObject, public QGraphicsItem
{
    Q_OBJECT

public:
    HUD(QGraphicsItem *parent = nullptr);
    ~HUD();

    void setPlayer(Player* player);
    void updateHUD();

    // 更新特定属性
    void updateHealth(int health, int maxHealth);
    void updateLevel(int level);
    void updateExp(int exp, int expToNext);
    void updateGold(int gold);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    Player *m_player;
    int m_health;
    int m_maxHealth;
    int m_level;
    int m_exp;
    int m_expToNext;
    int m_gold;
    QTimer *m_updateTimer;
};

#endif // HUD_H
