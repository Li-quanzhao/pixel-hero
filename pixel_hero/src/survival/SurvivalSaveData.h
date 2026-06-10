#ifndef SURVIVALSAVEDATA_H
#define SURVIVALSAVEDATA_H

#include <QString>
#include <QList>
#include <QPair>
#include <QDateTime>

struct SurvivalSaveData {
    int     currentWave     = 0;
    int     totalKills      = 0;
    float   elapsedTime     = 0;
    int     playerLevel     = 1;
    int     playerHealth    = 100;
    int     playerMaxHealth = 100;
    int     playerExp       = 0;
    QString characterId     = "warrior";
    QString weaponId        = "short_sword";
    QList<QPair<QString,int>> skills;
    QDateTime savedAt;
};

#endif // SURVIVALSAVEDATA_H
