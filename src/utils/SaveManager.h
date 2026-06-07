#ifndef SAVEMANAGER_H
#define SAVEMANAGER_H

#include <QObject>
#include <QString>
#include <QList>

struct SaveData {
    QString playerName;
    int level;
    int exp;
    int health;
    int maxHealth;
    int attack;
    int defense;
    int gold;
    QString currentMap;
    int posX;
    int posY;
    // 背包物品ID列表
    QStringList inventoryIds;
    // 装备ID
    QString weaponId;
    QString armorId;
};

class SaveManager : public QObject
{
    Q_OBJECT

public:
    static SaveManager* instance();

    bool saveGame(const SaveData& data, int slot = 0);
    bool loadGame(SaveData& data, int slot = 0);
    bool hasSave(int slot = 0);
    QList<SaveData> listSaves();

private:
    SaveManager();
    static SaveManager* m_instance;
    QString saveFilePath(int slot) const;
};

#endif // SAVEMANAGER_H
