#ifndef ENEMYFACTORY_H
#define ENEMYFACTORY_H

#include <QString>
#include <QMap>
#include <QJsonObject>
#include "entities/Enemy.h"

struct EnemyStats {
    int hp;
    int atk;
    int def;
    float spd;
    int exp;
};

class EnemyFactory
{
public:
    EnemyFactory();
    void loadStats(const QJsonObject& statsJson);
    Enemy* createEnemy(const QString& id, qreal x, qreal y);

private:
    QMap<QString, EnemyStats> m_stats;
};

#endif // ENEMYFACTORY_H
