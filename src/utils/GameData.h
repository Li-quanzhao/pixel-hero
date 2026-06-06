#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <QObject>
#include <QString>
#include <QPoint>
#include <QStringList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QList>
#include <QVector>

class GameData : public QObject
{
    Q_OBJECT

public:
    struct LevelData {
        QString id;
        QString name;
        int width;
        int height;
        QString tileset;
        QPoint spawnPoint;
        QStringList tileMap;
        QStringList enemies;
        QList<QPoint> enemyPositions;
        QStringList npcs;
        QList<QPoint> npcPositions;
    };

    struct ItemData {
        QString id;
        QString name;
        QString type;
        int attack;
        int defense;
        int price;
        QString description;
    };

    static GameData* instance();

    bool loadLevelData(const QString& path);
    bool loadItemData(const QString& path);

    QList<LevelData> levels() const;
    QList<ItemData> items() const;

    LevelData getLevelById(const QString& id);
    ItemData getItemById(const QString& id);

private:
    GameData(QObject *parent = nullptr);
    ~GameData();

    static GameData* m_instance;
    QList<LevelData> m_levels;
    QList<ItemData> m_items;
};

#endif // GAMEDATA_H
