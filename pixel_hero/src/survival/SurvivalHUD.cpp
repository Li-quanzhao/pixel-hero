#include "SurvivalHUD.h"
#include <QPainter>
#include <QFont>

SurvivalHUD::SurvivalHUD(QGraphicsItem* parent)
    : QGraphicsItem(parent)
    , m_player(nullptr), m_waveManager(nullptr)
    , m_health(0), m_maxHealth(0), m_level(0), m_exp(0), m_expToNext(0)
    , m_wave(0), m_kills(0), m_timeSeconds(0)
{
    setZValue(100);
    setCacheMode(DeviceCoordinateCache);
}

SurvivalHUD::~SurvivalHUD() {}

void SurvivalHUD::bind(SurvivalPlayer* player, WaveManager* waveManager)
{
    m_player = player;
    m_waveManager = waveManager;
}

QRectF SurvivalHUD::boundingRect() const
{
    return QRectF(0, 0, 800, 600);
}

void SurvivalHUD::updateHUD()
{
    if (!m_player) return;

    m_health     = m_player->health();
    m_maxHealth  = m_player->maxHealth();
    m_level      = m_player->level();
    m_exp        = m_player->exp();
    m_expToNext  = m_level * 80;
    m_kills      = m_waveManager ? m_waveManager->totalKills() : 0;
    m_wave       = m_waveManager ? m_waveManager->currentWave() : 0;
    m_timeSeconds= m_waveManager ? static_cast<int>(m_waveManager->elapsedTime()) : 0;

    m_activeSkillNames.clear();
    for (const auto& as : m_player->activeSkills()) {
        const SkillData* sd = GameData::instance()->getSkillById(as.skillId);
        if (sd) m_activeSkillNames.append(sd->name + "Lv" + QString::number(as.level));
    }

    update();
}

void SurvivalHUD::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    QFont font;
    font.setPixelSize(13);
    painter->setFont(font);

    const int BAR_W = 150;
    const int BAR_H = 14;

    // ===== HP血条 (左上) =====
    painter->setPen(Qt::white);
    painter->drawText(10, 22, QString("Lv.%1").arg(m_level));

    float hpRatio = m_maxHealth > 0 ? (float)m_health / m_maxHealth : 0;
    int barX = 65;
    painter->fillRect(barX, 10, BAR_W, BAR_H, QColor(60, 0, 0));
    painter->fillRect(barX, 10, static_cast<int>(BAR_W * hpRatio), BAR_H, QColor(220, 30, 30));
    painter->setPen(QColor(200, 200, 200));
    painter->drawRect(barX, 10, BAR_W, BAR_H);
    painter->drawText(QRectF(barX, 10, BAR_W, BAR_H), Qt::AlignCenter,
                      QString("HP %1/%2").arg(m_health).arg(m_maxHealth));

    // ===== 经验条 =====
    float xpRatio = m_expToNext > 0 ? (float)m_exp / m_expToNext : 0;
    painter->fillRect(barX, 28, BAR_W, BAR_H - 4, QColor(20, 20, 60));
    painter->fillRect(barX, 28, static_cast<int>(BAR_W * xpRatio), BAR_H - 4, QColor(80, 80, 200));
    painter->setPen(QColor(180, 180, 180));
    painter->drawRect(barX, 28, BAR_W, BAR_H - 4);
    QFont xpFont;
    xpFont.setPixelSize(10);
    painter->setFont(xpFont);
    painter->drawText(QRectF(barX, 28, BAR_W, BAR_H - 4), Qt::AlignCenter,
                      QString("XP %1/%2").arg(m_exp).arg(m_expToNext));

    // ===== 右上：存活时间 + 波次 =====
    QFont rightFont;
    rightFont.setPixelSize(15);
    rightFont.setBold(true);
    painter->setFont(rightFont);
    painter->setPen(Qt::white);

    int sec = m_timeSeconds;
    int m = sec / 60;
    int s = sec % 60;
    painter->drawText(QRectF(570, 10, 220, 22), Qt::AlignRight,
                      QString("存活 %1:%2").arg(m, 2, 10, QChar('0')).arg(s, 2, 10, QChar('0')));

    painter->drawText(QRectF(570, 32, 220, 22), Qt::AlignRight,
                      QString("Wave %1").arg(m_wave));

    // ===== 技能列表 =====
    QFont skillFont;
    skillFont.setPixelSize(12);
    painter->setFont(skillFont);
    painter->setPen(QColor(0xff, 0xcc, 0x44));
    QString skillText = "技能: " + m_activeSkillNames.join("  ");
    if (skillText == "技能: ") skillText = "技能: 暂无";
    painter->drawText(10, 58, skillText);

    // ===== 击杀数 =====
    painter->setPen(QColor(0xaa, 0xaa, 0xaa));
    painter->drawText(10, 76, QString("击杀: %1").arg(m_kills));

    // ===== 底部操作提示 =====
    QFont tipFont;
    tipFont.setPixelSize(11);
    painter->setFont(tipFont);
    painter->setPen(QColor(120, 120, 120));
    painter->drawText(QRectF(0, 565, 800, 30), Qt::AlignCenter,
                      "WASD移动 | 自动攻击临近敌人 | ESC暂停");

    // ===== 升级横幅 =====
    if (m_showLevelUpBanner) {
        QFont bannerFont;
        bannerFont.setPixelSize(48);
        bannerFont.setBold(true);
        painter->setFont(bannerFont);

        // 发光效果(描边)
        painter->setPen(QPen(QColor(0xff, 0xcc, 0x00, 180), 3));
        painter->drawText(QRectF(0, 220, 800, 60), Qt::AlignCenter, "LEVEL UP!");

        painter->setPen(QColor(0xff, 0xdd, 0x44));
        painter->drawText(QRectF(0, 220, 800, 60), Qt::AlignCenter, "LEVEL UP!");
    }
}
