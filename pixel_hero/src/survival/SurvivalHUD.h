#ifndef SURVIVALHUD_H
#define SURVIVALHUD_H

#include <QObject>
#include <QGraphicsItem>
#include <QRectF>
#include <QTimer>
#include "SurvivalPlayer.h"
#include "WaveManager.h"

class SurvivalHUD : public QObject, public QGraphicsItem
{
    Q_OBJECT

public:
    SurvivalHUD(QGraphicsItem* parent = nullptr);
    ~SurvivalHUD();

    void bind(SurvivalPlayer* player, WaveManager* waveManager);
    void updateHUD();
    void setShowLevelUpBanner(bool show) { m_showLevelUpBanner = show; update(); }

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

private:
    SurvivalPlayer* m_player;
    WaveManager*    m_waveManager;

    // 显示缓存
    int     m_health;
    int     m_maxHealth;
    int     m_level;
    int     m_exp;
    int     m_expToNext;
    int     m_wave;
    int     m_kills;
    int     m_timeSeconds;
    int     m_gold;
    QStringList m_activeSkillNames;
    bool    m_showLevelUpBanner = false;
};

#endif // SURVIVALHUD_H
