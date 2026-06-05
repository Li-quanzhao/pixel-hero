#include "HUD.h"
#include <QPainter>

HUD::HUD(QGraphicsItem *parent)
    : QGraphicsItem(parent), m_player(nullptr), m_health(100), m_maxHealth(100), m_level(1), m_exp(0), m_gold(0)
{
    setZValue(100);
}

HUD::~HUD()
{
}

void HUD::setPlayer(Player* player)
{
    m_player = player;
}

void HUD::updateHUD()
{
    if (m_player) {
        m_health = m_player->health();
        m_maxHealth = m_player->maxHealth();
        m_level = m_player->level();
        m_exp = m_player->exp();
    }
}

QRectF HUD::boundingRect() const
{
    return QRectF(0, 0, 800, 60);
}

void HUD::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setBrush(QColor(0x2d, 0x37, 0x48));
    painter->drawRect(0, 0, 800, 60);

    painter->setPen(QColor(Qt::white));
    painter->setFont(QFont("Arial", 12));
    painter->drawText(10, 20, QString("等级: %1").arg(m_level));
    painter->drawText(10, 40, QString("经验: %1").arg(m_exp));
    painter->drawText(700, 20, QString("金币: %1").arg(m_gold));

    int healthBarWidth = 200;
    int healthBarHeight = 20;
    int healthBarX = 100;
    int healthBarY = 15;

    painter->setBrush(QColor(0x4a, 0x55, 0x68));
    painter->drawRect(healthBarX, healthBarY, healthBarWidth, healthBarHeight);

    int healthWidth = (m_health * healthBarWidth) / m_maxHealth;
    painter->setBrush(QColor(0xe5, 0x3e, 0x3e));
    painter->drawRect(healthBarX, healthBarY, healthWidth, healthBarHeight);

    painter->setPen(QColor(Qt::white));
    painter->drawText(healthBarX + 5, healthBarY + 15, QString("%1/%2").arg(m_health).arg(m_maxHealth));
}