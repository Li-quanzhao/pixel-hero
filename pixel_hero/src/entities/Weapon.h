#ifndef WEAPON_H
#define WEAPON_H

#include <QString>
#include <QColor>

struct WeaponData {
    QString id;
    QString name;
    int    attackBonus = 0;
    float  speedMod    = 1.0f;   // 攻速倍率 (>1 更快)
    float  rangeBonus  = 0;      // 攻击距离加成
    float  skillBonus  = 1.0f;   // 技能伤害倍率
    QColor color;
    QString desc;
};

#endif // WEAPON_H
