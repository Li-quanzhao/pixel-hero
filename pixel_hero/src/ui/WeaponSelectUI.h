#ifndef WEAPONSELECTUI_H
#define WEAPONSELECTUI_H

#include <QObject>
#include <QGraphicsItem>
#include <QString>
#include <QList>
#include <QColor>
#include "SelectableListBase.h"
#include "entities/Weapon.h"

class WeaponSelectUI : public SelectableListBase
{
    Q_OBJECT

public:
    WeaponSelectUI(const QList<WeaponData>& weapons,
                   QGraphicsItem* parent = nullptr);
    ~WeaponSelectUI();

    // 键盘操作 (保留为兼容方法)
    void prev() { selectPrev(); }
    void next() { selectNext(); }

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

signals:
    void weaponSelected(int index);

protected:
    QRectF itemRect(int index) const override;
    void onConfirm() override;

private:
    QList<WeaponData> m_weapons;

    static constexpr int CARD_W   = 160;
    static constexpr int CARD_H   = 310;
    static constexpr int CARD_GAP = 24;
    static constexpr int INFO_Y   = 440;

    void drawStatRow(QPainter* p, qreal x, qreal y, int w,
                     const QString& label, const QString& value,
                     const QColor& valColor) const;
};

#endif // WEAPONSELECTUI_H
