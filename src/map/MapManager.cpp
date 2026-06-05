#include "MapManager.h"
#include <QGraphicsScene>

MapManager::MapManager(QObject *parent)
    : QObject(parent), m_mapWidth(0), m_mapHeight(0)
{
}

MapManager::~MapManager()
{
    clearMap();
}

void MapManager::loadMap(const QString& mapName)
{
    clearMap();
    m_currentMap = mapName;

    m_mapWidth = 32;
    m_mapHeight = 24;

    m_spawnPoint = QPointF(16 * 32, 12 * 32);

    m_tiles.resize(m_mapHeight);
    for (int y = 0; y < m_mapHeight; ++y) {
        m_tiles[y].resize(m_mapWidth);
        for (int x = 0; x < m_mapWidth; ++x) {
            Tile::TileType type = Tile::FLOOR;
            
            if (x == 0 || x == m_mapWidth - 1 || y == 0 || y == m_mapHeight - 1) {
                type = Tile::WALL;
            } else if (y == 6 && x > 5 && x < 26) {
                type = Tile::GRASS;
            } else if (y == 12 && x > 10 && x < 22) {
                type = Tile::WALL;
            }

            m_tiles[y][x] = new Tile(type, x, y);
        }
    }
}

void MapManager::renderMap(QGraphicsScene* scene)
{
    for (int y = 0; y < m_mapHeight; ++y) {
        for (int x = 0; x < m_mapWidth; ++x) {
            if (m_tiles[y][x]) {
                scene->addItem(m_tiles[y][x]);
            }
        }
    }
}

QPointF MapManager::getPlayerSpawnPoint() const
{
    return m_spawnPoint;
}

bool MapManager::checkCollision(QRectF rect)
{
    int startX = rect.left() / 32;
    int endX = rect.right() / 32;
    int startY = rect.top() / 32;
    int endY = rect.bottom() / 32;

    for (int y = startY; y <= endY; ++y) {
        for (int x = startX; x <= endX; ++x) {
            if (y >= 0 && y < m_mapHeight && x >= 0 && x < m_mapWidth) {
                if (m_tiles[y][x] && m_tiles[y][x]->isCollidable()) {
                    return true;
                }
            } else {
                return true;
            }
        }
    }
    return false;
}

void MapManager::switchMap(const QString& newMap)
{
    loadMap(newMap);
}

void MapManager::update(qreal deltaTime)
{
    Q_UNUSED(deltaTime);
}

void MapManager::clearMap()
{
    for (int y = 0; y < m_tiles.size(); ++y) {
        qDeleteAll(m_tiles[y]);
        m_tiles[y].clear();
    }
    m_tiles.clear();
    m_mapWidth = 0;
    m_mapHeight = 0;
}