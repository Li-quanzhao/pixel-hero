#include "MapManager.h"
#include "GameData.h"
#include <QGraphicsScene>
#include <QDebug>
#include <cmath>

MapManager::MapManager(QObject *parent)
    : QObject(parent), m_mapWidth(0), m_mapHeight(0)
{
}

MapManager::~MapManager()
{
    clearMap();
}

Tile::TileType MapManager::charToTileType(QChar c) const
{
    switch (c.toUpper().toLatin1()) {
    case 'W': return Tile::WALL;
    case 'F': return Tile::FLOOR;
    case 'G': return Tile::GRASS;
    case 'H': return Tile::WATER;
    case 'L': return Tile::LAVA;
    case 'D': return Tile::DOOR;
    default:  return Tile::FLOOR;
    }
}

Enemy::EnemyType MapManager::stringToEnemyType(const QString& s) const
{
    if (s == "goblin")    return Enemy::GOBLIN;
    if (s == "slime")     return Enemy::SLIME;
    if (s == "skeleton")  return Enemy::SKELETON;
    if (s == "boss")      return Enemy::BOSS;
    return Enemy::SLIME;
}

NPC::NPCType MapManager::stringToNPCType(const QString& s) const
{
    if (s == "villager")     return NPC::VILLAGER;
    if (s == "blacksmith")   return NPC::BLACKSMITH;
    if (s == "shopkeeper")   return NPC::SHOPKEEPER;
    if (s == "quest_giver")  return NPC::QUEST_GIVER;
    return NPC::VILLAGER;
}

void MapManager::loadMap(const QString& mapName)
{
    clearMap();
    m_currentMap = mapName;

    // 尝试从 GameData 加载
    GameData::LevelData levelData = GameData::instance()->getLevelById(mapName);

    if (!levelData.id.isEmpty() && !levelData.tileMap.isEmpty()) {
        // 从 JSON 数据加载
        m_mapHeight = levelData.tileMap.size();
        m_mapWidth = levelData.tileMap[0].length();
        m_spawnPoint = QPointF(levelData.spawnPoint.x() * 32, levelData.spawnPoint.y() * 32);

        m_tiles.resize(m_mapHeight);
        for (int y = 0; y < m_mapHeight; ++y) {
            m_tiles[y].resize(m_mapWidth);
            const QString& row = levelData.tileMap[y];
            for (int x = 0; x < m_mapWidth && x < row.length(); ++x) {
                Tile::TileType type = charToTileType(row[x]);
                m_tiles[y][x] = new Tile(type, x, y);
            }
        }

        // 生成敌人
        for (int i = 0; i < levelData.enemies.size() && i < levelData.enemyPositions.size(); ++i) {
            Enemy::EnemyType type = stringToEnemyType(levelData.enemies[i]);
            Enemy* enemy = new Enemy(type);
            QPoint pos = levelData.enemyPositions[i];
            enemy->setPos(pos.x() * 32 + 16, pos.y() * 32 + 16);
            m_enemies.append(enemy);
        }

        // 生成 NPC
        for (int i = 0; i < levelData.npcs.size() && i < levelData.npcPositions.size(); ++i) {
            NPC::NPCType type = stringToNPCType(levelData.npcs[i]);
            NPC* npc = new NPC(type);
            QPoint pos = levelData.npcPositions[i];
            npc->setPos(pos.x() * 32 + 16, pos.y() * 32 + 16);
            m_npcs.append(npc);
        }
    } else {
        // 回退代码生成
        m_mapWidth = 32;
        m_mapHeight = 24;
        m_spawnPoint = QPointF(4 * 32, 4 * 32);

        m_tiles.resize(m_mapHeight);
        for (int y = 0; y < m_mapHeight; ++y) {
            m_tiles[y].resize(m_mapWidth);
            for (int x = 0; x < m_mapWidth; ++x) {
                Tile::TileType type = Tile::FLOOR;
                if (x == 0 || x == m_mapWidth - 1 || y == 0 || y == m_mapHeight - 1)
                    type = Tile::WALL;
                else if (x >= 13 && x <= 16 && y >= 6 && y <= 8)
                    type = Tile::GRASS;
                else if (x >= 13 && x <= 18 && y >= 12 && y <= 12)
                    type = Tile::WALL;
                m_tiles[y][x] = new Tile(type, x, y);
            }
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

    for (Enemy* enemy : m_enemies) {
        scene->addItem(enemy);
    }

    for (NPC* npc : m_npcs) {
        scene->addItem(npc);
    }
}

void MapManager::clearScene(QGraphicsScene* scene)
{
    for (int y = 0; y < m_mapHeight; ++y) {
        for (int x = 0; x < m_mapWidth; ++x) {
            if (m_tiles[y][x] && m_tiles[y][x]->scene()) {
                scene->removeItem(m_tiles[y][x]);
            }
        }
    }
    for (Enemy* enemy : m_enemies) {
        if (enemy->scene()) scene->removeItem(enemy);
    }
    for (NPC* npc : m_npcs) {
        if (npc->scene()) scene->removeItem(npc);
    }
}

void MapManager::removeEnemy(Enemy* enemy)
{
    m_enemies.removeAll(enemy);
}

QPointF MapManager::getPlayerSpawnPoint() const
{
    return m_spawnPoint;
}

QList<Enemy*> MapManager::getEnemies() const
{
    return m_enemies;
}

QList<NPC*> MapManager::getNPCs() const
{
    return m_npcs;
}

bool MapManager::checkCollision(QRectF rect)
{
    int startX = static_cast<int>(rect.left() / 32);
    int endX   = static_cast<int>(rect.right() / 32);
    int startY = static_cast<int>(rect.top() / 32);
    int endY   = static_cast<int>(rect.bottom() / 32);

    for (int y = startY; y <= endY; ++y) {
        for (int x = startX; x <= endX; ++x) {
            if (y >= 0 && y < m_mapHeight && x >= 0 && x < m_mapWidth) {
                if (m_tiles[y][x] && m_tiles[y][x]->isCollidable()) {
                    return true;
                }
            } else {
                return true; // 出界视为碰撞
            }
        }
    }
    return false;
}

void MapManager::update(qreal deltaTime)
{
    Q_UNUSED(deltaTime);
    // 敌人AI更新在此处理（可选）
}

void MapManager::clearMap()
{
    for (int y = 0; y < m_tiles.size(); ++y) {
        qDeleteAll(m_tiles[y]);
        m_tiles[y].clear();
    }
    m_tiles.clear();

    qDeleteAll(m_enemies);
    m_enemies.clear();

    qDeleteAll(m_npcs);
    m_npcs.clear();

    m_mapWidth = 0;
    m_mapHeight = 0;
}
