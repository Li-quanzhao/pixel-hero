#ifndef WEAPONSELECTUI_H
#define WEAPONSELECTUI_H

#include <QObject>
#include <QGraphicsItem>
#include <QString>
#include <QList>
#include <QColor>
#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>
#include "entities/Weapon.h"

class WeaponSelectUI : public QObject, public QGraphicsItem
{
    Q_OBJECT

public:
    WeaponSelectUI(const QList<WeaponData>& weapons,
                   QGraphicsItem* parent = nullptr);
    ~WeaponSelectUI();

    void appear();
    void dismiss();

    void prev();
    void next();
    void confirm();
    int  selectedIndex() const { return m_selectedIndex; }

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

signals:
    void weaponSelected(int index);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void wheelEvent(QGraphicsSceneWheelEvent* event) override;

private:
    QList<WeaponData> m_weapons;
    bool  m_visible;
    int   m_selectedIndex;

    static constexpr int CARD_W   = 160;
    static constexpr int CARD_H   = 310;
    static constexpr int CARD_GAP = 24;
    static constexpr int INFO_Y   = 440;

    QRectF cardRect(int index) const;
    void   drawStatRow(QPainter* p, qreal x, qreal y, int w,
                       const QString& label, const QString& value,
                       const QColor& valColor) const;
};

#endif // WEAPONSELECTUI_H
