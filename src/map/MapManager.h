#ifndef MAPMANAGER_H
#define MAPMANAGER_H

#include <QObject>
#include <QPointF>
#include <QString>
#include <QList>
#include "Tile.h"

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
    void switchMap(const QString& newMap);
    void update(qreal deltaTime);

private:
    QString m_currentMap;
    QList<QList<Tile*>> m_tiles;
    QPointF m_spawnPoint;
    int m_mapWidth;
    int m_mapHeight;

    void clearMap();
    void parseMapData(const QString& mapData);
};

#endif // MAPMANAGER_H