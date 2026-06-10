#include "EnemyFactory.h"
#include <QDebug>

EnemyFactory::EnemyFactory() {}

void EnemyFactory::loadStats(const QJsonObject& statsJson)
{
    m_stats.clear();
    for (const QString& key : statsJson.keys()) {
        QJsonObject obj = statsJson[key].toObject();
        EnemyStats s;
        s.hp  = obj["hp"].toInt(10);
        s.atk = obj["atk"].toInt(5);
        s.def = obj["def"].toInt(1);
        s.spd = static_cast<float>(obj["spd"].toDouble(1.0));
        s.exp = obj["exp"].toInt(10);
        m_stats[key] = s;
    }
}

Enemy* EnemyFactory::createEnemy(const QString& id, qreal x, qreal y)
{
    Enemy::EnemyType type;
    if (id == "slime")           type = Enemy::SLIME;
    else if (id == "goblin")     type = Enemy::GOBLIN;
    else if (id == "skeleton")   type = Enemy::SKELETON;
    else if (id == "bat")        type = Enemy::BAT;
    else if (id == "goblin_elite") type = Enemy::GOBLIN_ELITE;
    else if (id == "dragon")     type = Enemy::DRAGON;
    else {
        qWarning() << "Unknown enemy id:" << id;
        type = Enemy::SLIME;
    }

    Enemy* enemy = new Enemy(type);
    enemy->setPos(x, y);

    // 从 JSON 覆盖属性
    if (m_stats.contains(id)) {
        EnemyStats& s = m_stats[id];
        enemy->setHealth(s.hp);
        enemy->setMaxHealth(s.hp);
        enemy->setAttack(s.atk);
        enemy->setDefense(s.def);
        enemy->setMoveSpeed(s.spd);
        enemy->setExpReward(s.exp);
    }

    return enemy;
}
