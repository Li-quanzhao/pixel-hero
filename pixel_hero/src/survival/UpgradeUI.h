#ifndef UPGRADEUI_H
#define UPGRADEUI_H

#include <QObject>
#include <QGraphicsItem>
#include <QRectF>
#include <QList>
#include <QGraphicsSceneWheelEvent>
#include "utils/GameData.h"

class UpgradeUI : public QObject, public QGraphicsItem
{
    Q_OBJECT

public:
    UpgradeUI(QGraphicsItem* parent = nullptr);
    ~UpgradeUI();

    void showUpgrade(const QList<SkillData>& options);
    void hide();
    bool isVisible() const;

    void selectOption(int index);

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void wheelEvent(QGraphicsSceneWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

signals:
    void skillSelected(const QString& skillId);

private:
    bool m_isVisible;
    QList<SkillData> m_options;
    int  m_selectedIndex;

    static const int CARD_WIDTH  = 180;
    static const int CARD_HEIGHT = 200;
    static const int CARD_GAP    = 20;
    static const int START_X     = 110;
    static const int START_Y     = 160;

    QRectF getCardRect(int index) const;
};

#endif // UPGRADEUI_H
