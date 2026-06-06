#include "HUD.h"
#include <QPainter>

HUD::HUD(QGraphicsItem *parent)
    : QObject(nullptr), QGraphicsItem(parent)
    , m_player(nullptr)
    , m_health(100)
    , m_maxHealth(100)
    , m_level(1)
    , m_exp(0)
    , m_expToNext(100)
    , m_gold(0)
    , m_updateTimer(nullptr)
{
    setZValue(100);
}

HUD::~HUD()
{
    if (m_updateTimer) {
        m_updateTimer->stop();
        delete m_updateTimer;
    }
}

void HUD::setPlayer(Player* player)
{
    m_player = player;
    if (m_player) {
        // 初始化数据
        m_health = m_player->health();
        m_maxHealth = m_player->maxHealth();
        m_level = m_player->level();
        m_exp = m_player->exp();
        m_gold = m_player->gold();

        // 启动定时更新
        if (!m_updateTimer) {
            m_updateTimer = new QTimer();
            QObject::connect(m_updateTimer, &QTimer::timeout, this, &HUD::updateHUD);
            m_updateTimer->start(100);  // 每100ms更新一次
        }
    }
}

void HUD::updateHUD()
{
    if (m_player) {
        m_health = m_player->health();
        m_maxHealth = m_player->maxHealth();
        m_level = m_player->level();
        m_exp = m_player->exp();
        m_gold = m_player->gold();
        update();
    }
}

void HUD::updateHealth(int health, int maxHealth)
{
    m_health = health;
    m_maxHealth = maxHealth;
    update();
}

void HUD::updateLevel(int level)
{
    m_level = level;
    update();
}

void HUD::updateExp(int exp, int expToNext)
{
    m_exp = exp;
    m_expToNext = expToNext;
    update();
}

void HUD::updateGold(int gold)
{
    m_gold = gold;
    update();
}

QRectF HUD::boundingRect() const
{
    return QRectF(0, 0, 800, 60);
}

void HUD::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // 背景
    painter->setBrush(QColor(0x1a, 0x1a, 0x2e, 200));
    painter->setPen(Qt::NoPen);
    painter->drawRect(0, 0, 800, 60);

    // 分隔线
    painter->setPen(QColor(0x4a, 0x55, 0x68));
    painter->drawLine(0, 59, 800, 59);

    painter->setPen(QColor(Qt::white));
    painter->setFont(QFont("Arial", 12, QFont::Bold));

    // 等级
    painter->setPen(QColor(0xff, 0xcc, 0x00));
    painter->drawText(15, 25, QString("Lv.%1").arg(m_level));

    // 血条
    int healthBarX = 80;
    int healthBarY = 10;
    int healthBarWidth = 200;
    int healthBarHeight = 20;

    // 血条背景
    painter->setBrush(QColor(0x4a, 0x55, 0x68));
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(healthBarX, healthBarY, healthBarWidth, healthBarHeight, 3, 3);

    // 血条填充
    int healthWidth = (m_health * healthBarWidth) / m_maxHealth;
    painter->setBrush(QColor(0xe5, 0x3e, 0x3e));
    painter->drawRoundedRect(healthBarX, healthBarY, healthWidth, healthBarHeight, 3, 3);

    // 血条文字
    painter->setPen(QColor(Qt::white));
    painter->setFont(QFont("Arial", 10));
    QString healthText = QString("%1/%2").arg(m_health).arg(m_maxHealth);
    int textWidth = painter->fontMetrics().horizontalAdvance(healthText);
    painter->drawText(healthBarX + (healthBarWidth - textWidth) / 2, healthBarY + 15, healthText);

    // 经验条
    int expBarX = 80;
    int expBarY = 35;
    int expBarWidth = 200;
    int expBarHeight = 12;

    painter->setBrush(QColor(0x4a, 0x55, 0x68));
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(expBarX, expBarY, expBarWidth, expBarHeight, 2, 2);

    int expWidth = (m_exp * expBarWidth) / m_expToNext;
    painter->setBrush(QColor(0x61, 0xaf, 0xef));
    painter->drawRoundedRect(expBarX, expBarY, expWidth, expBarHeight, 2, 2);

    // 金币
    painter->setPen(QColor(0xff, 0xd7, 0x00));
    painter->setFont(QFont("Arial", 12, QFont::Bold));
    painter->drawText(700, 30, QString("$ %1").arg(m_gold));

    // 技能提示
    painter->setPen(QColor(0x88, 0x88, 0x88));
    painter->setFont(QFont("Arial", 9));
    painter->drawText(320, 35, "WASD移动 | Space攻击 | E交互 | Esc暂停 | I背包");
}
