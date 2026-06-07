#include "SaveManager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QStandardPaths>

SaveManager* SaveManager::m_instance = nullptr;

SaveManager::SaveManager() {}

SaveManager* SaveManager::instance()
{
    if (!m_instance) m_instance = new SaveManager();
    return m_instance;
}

QString SaveManager::saveFilePath(int slot) const
{
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    return dir + QString("/save_%1.json").arg(slot);
}

bool SaveManager::saveGame(const SaveData& data, int slot)
{
    QJsonObject obj;
    obj["playerName"] = data.playerName;
    obj["level"] = data.level;
    obj["exp"] = data.exp;
    obj["health"] = data.health;
    obj["maxHealth"] = data.maxHealth;
    obj["attack"] = data.attack;
    obj["defense"] = data.defense;
    obj["gold"] = data.gold;
    obj["currentMap"] = data.currentMap;
    obj["posX"] = data.posX;
    obj["posY"] = data.posY;

    QJsonArray invArr;
    for (const QString& id : data.inventoryIds) invArr.append(id);
    obj["inventoryIds"] = invArr;
    obj["weaponId"] = data.weaponId;
    obj["armorId"] = data.armorId;

    QFile file(saveFilePath(slot));
    if (!file.open(QIODevice::WriteOnly)) return false;
    file.write(QJsonDocument(obj).toJson());
    return true;
}

bool SaveManager::loadGame(SaveData& data, int slot)
{
    QFile file(saveFilePath(slot));
    if (!file.open(QIODevice::ReadOnly)) return false;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject obj = doc.object();

    data.playerName = obj["playerName"].toString();
    data.level = obj["level"].toInt();
    data.exp = obj["exp"].toInt();
    data.health = obj["health"].toInt();
    data.maxHealth = obj["maxHealth"].toInt();
    data.attack = obj["attack"].toInt();
    data.defense = obj["defense"].toInt();
    data.gold = obj["gold"].toInt();
    data.currentMap = obj["currentMap"].toString();
    data.posX = obj["posX"].toInt();
    data.posY = obj["posY"].toInt();

    QJsonArray invArr = obj["inventoryIds"].toArray();
    for (const QJsonValue& v : invArr) data.inventoryIds.append(v.toString());

    data.weaponId = obj["weaponId"].toString();
    data.armorId = obj["armorId"].toString();
    return true;
}

bool SaveManager::hasSave(int slot)
{
    return QFile::exists(saveFilePath(slot));
}

QList<SaveData> SaveManager::listSaves()
{
    QList<SaveData> list;
    for (int i = 0; i < 3; ++i) {
        SaveData d;
        if (loadGame(d, i)) list.append(d);
    }
    return list;
}
