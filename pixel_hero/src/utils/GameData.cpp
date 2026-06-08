#include "GameData.h"
#include <QFile>
#include <QJsonParseError>

GameData* GameData::m_instance = nullptr;

GameData::GameData(QObject *parent)
    : QObject(parent)
{
}

GameData::~GameData()
{
}

GameData* GameData::instance()
{
    if (!m_instance) {
        m_instance = new GameData();
    }
    return m_instance;
}

bool GameData::loadLevelData(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        return false;
    }

    QJsonObject obj = doc.object();
    QJsonArray levelsArray = obj["levels"].toArray();

    for (const QJsonValue& val : levelsArray) {
        QJsonObject levelObj = val.toObject();
        LevelData level;
        level.id = levelObj["id"].toString();
        level.name = levelObj["name"].toString();
        level.width = levelObj["width"].toInt();
        level.height = levelObj["height"].toInt();
        level.tileset = levelObj["tileset"].toString();

        QJsonObject spawnObj = levelObj["spawn_point"].toObject();
        level.spawnPoint = QPoint(spawnObj["x"].toInt(), spawnObj["y"].toInt());

        // 解析瓦片地图
        QJsonArray tileMapArray = levelObj["tile_map"].toArray();
        for (const QJsonValue& rowVal : tileMapArray) {
            level.tileMap.append(rowVal.toString());
        }

        // 解析敌人
        QJsonArray enemiesArray = levelObj["enemies"].toArray();
        for (const QJsonValue& enemyVal : enemiesArray) {
            level.enemies.append(enemyVal.toString());
        }

        // 解析敌人位置
        QJsonArray enemyPosArray = levelObj["enemy_positions"].toArray();
        for (const QJsonValue& posVal : enemyPosArray) {
            QJsonObject posObj = posVal.toObject();
            level.enemyPositions.append(QPoint(posObj["x"].toInt(), posObj["y"].toInt()));
        }

        // 解析 NPC
        QJsonArray npcsArray = levelObj["npcs"].toArray();
        for (const QJsonValue& npcVal : npcsArray) {
            level.npcs.append(npcVal.toString());
        }

        // 解析 NPC 位置
        QJsonArray npcPosArray = levelObj["npc_positions"].toArray();
        for (const QJsonValue& posVal : npcPosArray) {
            QJsonObject posObj = posVal.toObject();
            level.npcPositions.append(QPoint(posObj["x"].toInt(), posObj["y"].toInt()));
        }

        m_levels.append(level);
    }

    return true;
}

bool GameData::loadItemData(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        return false;
    }

    QJsonObject obj = doc.object();
    QJsonArray itemsArray = obj["items"].toArray();

    for (const QJsonValue& val : itemsArray) {
        QJsonObject itemObj = val.toObject();
        ItemData item;
        item.id = itemObj["id"].toString();
        item.name = itemObj["name"].toString();
        item.type = itemObj["type"].toString();
        item.attack = itemObj["attack"].toInt();
        item.defense = itemObj["defense"].toInt();
        item.price = itemObj["price"].toInt();
        item.healAmount = itemObj["healAmount"].toInt(0);
        item.description = itemObj["description"].toString();

        m_items.append(item);
    }

    return true;
}

QList<GameData::LevelData> GameData::levels() const { return m_levels; }
QList<GameData::ItemData> GameData::items() const { return m_items; }

GameData::LevelData GameData::getLevelById(const QString& id)
{
    for (const LevelData& level : m_levels) {
        if (level.id == id) {
            return level;
        }
    }
    return LevelData();
}

GameData::ItemData GameData::getItemById(const QString& id)
{
    for (const ItemData& item : m_items) {
        if (item.id == id) {
            return item;
        }
    }
    return ItemData();
}
