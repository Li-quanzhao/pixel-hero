#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QList>
#include <QPair>
#include <QColor>
#include "entities/Weapon.h"

// 技能数据结构
struct SkillData {
    QString id;
    QString name;
    QString type;           // "active" / "passive"
    QString description;
    int maxLevel;

    struct LevelData {
        int damage;
        float cooldown;
        int extra;          // 附加参数(弹数/范围/连锁数/速度加成等)
        QString desc;
    };
    QList<LevelData> levels;
};

// 波次配置
struct WaveConfig {
    int waveNumber;
    float startTime;
    QStringList enemies;
    int totalCount;
    float spawnInterval;
    bool isBossWave;
    bool isEliteWave;
};

class GameData : public QObject
{
    Q_OBJECT

public:
    static GameData* instance();

    // 技能数据
    bool loadSkillData(const QString& path);
    const SkillData* getSkillById(const QString& id) const;
    QList<SkillData> allSkills() const;

    // 波次数据
    bool loadWaveData(const QString& path);
    QList<WaveConfig> waveConfigs() const;
    QJsonObject enemyBaseStats() const;

    // 武器数据
    bool loadWeaponData(const QString& path);
    QList<WeaponData> weapons() const;
    const WeaponData* getWeaponById(const QString& id) const;

private:
    GameData(QObject *parent = nullptr);
    ~GameData();

    static GameData* m_instance;
    QList<SkillData> m_skills;
    QList<WaveConfig> m_waveConfigs;
    QList<WeaponData> m_weapons;
    QJsonObject m_enemyBaseStats;

    void initDefaultSkills();
    void initDefaultWaves();
    void initDefaultWeapons();
};

#endif // GAMEDATA_H
