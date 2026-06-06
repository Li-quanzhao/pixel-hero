#ifndef MAPMANAGER_H
#define MAPMANAGER_H

#include <QObject>
#include <QPointF>
#include <QString>
#include <QList>
#include "Tile.h"
#include "Enemy.h"
#include "NPC.h"

class QGraphicsScene;

class MapManager : public QObject
{
    Q_OBJECT

public:
    MapManager(QObject *parent = nullptr);
    ~MapManager();

    void loadMap(const QString& mapName);
    void renderMap(QGraphicsScene* scene);
    QPointF getPlayerSpawnPoint() const;
    bool checkCollision(QRectF rect);
    QList<Enemy*> getEnemies() const;
    QList<NPC*> getNPCs() const;
    void update(qreal deltaTime);

private:
    QString m_currentMap;
    QList<QList<Tile*>> m_tiles;
    QPointF m_spawnPoint;
    QList<Enemy*> m_enemies;
    QList<NPC*> m_npcs;
    int m_mapWidth;
    int m_mapHeight;

    void clearMap();
    Tile::TileType charToTileType(QChar c) const;
    Enemy::EnemyType stringToEnemyType(const QString& s) const;
    NPC::NPCType stringToNPCType(const QString& s) const;
};

#endif // MAPMANAGER_H
