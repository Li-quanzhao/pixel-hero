#include "GameData.h"
#include <QFile>
#include <QJsonParseError>

GameData* GameData::m_instance = nullptr;

GameData::GameData(QObject *parent) : QObject(parent) {}
GameData::~GameData() {}

GameData* GameData::instance()
{
    if (!m_instance) { m_instance = new GameData(); }
    return m_instance;
}

// ========== 技能数据 ==========
bool GameData::loadSkillData(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) { initDefaultSkills(); return false; }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
    if (error.error != QJsonParseError::NoError) { initDefaultSkills(); return false; }

    QJsonArray skillsArr = doc.object()["skills"].toArray();
    for (const QJsonValue& val : skillsArr) {
        QJsonObject obj = val.toObject();
        SkillData sd;
        sd.id          = obj["id"].toString();
        sd.name        = obj["name"].toString();
        sd.type        = obj["type"].toString();
        sd.description = obj["description"].toString();
        sd.maxLevel    = obj["maxLevel"].toInt(5);

        QJsonArray levelsArr = obj["levels"].toArray();
        for (const QJsonValue& lv : levelsArr) {
            QJsonObject lo = lv.toObject();
            SkillData::LevelData ld;
            ld.damage   = lo["damage"].toInt();
            ld.cooldown = static_cast<float>(lo["cooldown"].toDouble());
            ld.extra    = lo["extra"].toInt();
            ld.desc     = lo["desc"].toString();
            sd.levels.append(ld);
        }
        m_skills.append(sd);
    }

    if (m_skills.isEmpty()) initDefaultSkills();
    return true;
}

const SkillData* GameData::getSkillById(const QString& id) const
{
    for (const auto& s : m_skills) {
        if (s.id == id) return &s;
    }
    return nullptr;
}

QList<SkillData> GameData::allSkills() const { return m_skills; }

// ========== 波次数据 ==========
bool GameData::loadWaveData(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) { initDefaultWaves(); return false; }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
    if (error.error != QJsonParseError::NoError) { initDefaultWaves(); return false; }

    QJsonObject survivalObj = doc.object()["survivalWaves"].toObject();
    m_enemyBaseStats = survivalObj["enemyBaseStats"].toObject();

    QJsonArray wavesArr = survivalObj["waves"].toArray();
    for (const QJsonValue& val : wavesArr) {
        QJsonObject obj = val.toObject();
        WaveConfig wc;
        wc.waveNumber    = obj["wave"].toInt();
        wc.startTime     = static_cast<float>(obj["startTime"].toDouble());
        wc.totalCount    = obj["count"].toInt();
        wc.spawnInterval = static_cast<float>(obj["interval"].toDouble());
        wc.isBossWave    = obj["isBossWave"].toBool(false);
        wc.isEliteWave   = obj["isEliteWave"].toBool(false);

        QJsonArray enemyArr = obj["enemies"].toArray();
        for (const QJsonValue& ev : enemyArr) {
            wc.enemies.append(ev.toString());
        }
        m_waveConfigs.append(wc);
    }

    if (m_waveConfigs.isEmpty()) initDefaultWaves();
    return true;
}

QList<WaveConfig> GameData::waveConfigs() const { return m_waveConfigs; }

QJsonObject GameData::enemyBaseStats() const { return m_enemyBaseStats; }

// ========== 武器数据 ==========
bool GameData::loadWeaponData(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) { initDefaultWeapons(); return false; }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
    if (error.error != QJsonParseError::NoError) { initDefaultWeapons(); return false; }

    QJsonArray weaponsArr = doc.object()["weapons"].toArray();
    for (const QJsonValue& val : weaponsArr) {
        QJsonObject obj = val.toObject();
        WeaponData wd;
        wd.id          = obj["id"].toString();
        wd.name        = obj["name"].toString();
        wd.attackBonus = obj["attackBonus"].toInt();
        wd.speedMod    = static_cast<float>(obj["speedMod"].toDouble(1.0));
        wd.rangeBonus  = static_cast<float>(obj["range"].toInt()) - 50.0f;
        wd.color       = QColor(obj["color"].toString("#ffffff"));
        wd.desc        = obj["desc"].toString();
        m_weapons.append(wd);
    }

    if (m_weapons.isEmpty()) initDefaultWeapons();
    return true;
}

QList<WeaponData> GameData::weapons() const { return m_weapons; }

const WeaponData* GameData::getWeaponById(const QString& id) const
{
    for (const auto& w : m_weapons) {
        if (w.id == id) return &w;
    }
    return nullptr;
}

void GameData::initDefaultWeapons()
{
    m_weapons.clear();
    m_weapons.append({"short_sword", "短剑", 3,  1.0f, 0,   1.0f, QColor("#c0c0c0"), "攻击+3  攻速正常"});
    m_weapons.append({"long_sword",  "长剑", 8,  0.7f, 20,  1.0f, QColor("#ffd700"), "攻击+8  攻速-30%  范围更大"});
    m_weapons.append({"staff",      "法杖", 12, 0.5f, 40,  1.2f, QColor("#9932cc"), "攻击+12  攻速-50%  技能伤害+20%"});
    m_weapons.append({"dagger",     "匕首", 4,  1.5f, -10, 1.0f, QColor("#00ff7f"), "攻击+4  攻速+50%  移速+10%"});
}

// ========== 硬编码回退 ==========
void GameData::initDefaultSkills()
{
    m_skills.clear();
    // fireball
    SkillData fb;
    fb.id = "fireball"; fb.name = "火球术"; fb.type = "active"; fb.description = "向最近敌人发射火球"; fb.maxLevel = 5;
    fb.levels = {
        {15, 2.0f, 1, "伤害15，冷却2秒"}, {25, 1.8f, 2, "伤害25，冷却1.8秒"},
        {35, 1.6f, 2, "伤害35，冷却1.6秒"}, {50, 1.4f, 3, "伤害50，冷却1.4秒"},
        {70, 1.0f, 3, "伤害70，冷却1秒"}
    };
    m_skills.append(fb);

    // lightning
    SkillData lt;
    lt.id = "lightning"; lt.name = "闪电链"; lt.type = "active"; lt.description = "连锁攻击多个敌人"; lt.maxLevel = 5;
    lt.levels = {
        {20, 2.5f, 2, "伤害20，连锁2个"}, {30, 2.3f, 3, "伤害30，连锁3个"},
        {45, 2.0f, 4, "伤害45，连锁4个"}, {60, 1.8f, 5, "伤害60，连锁5个"},
        {80, 1.5f, 6, "伤害80，连锁6个"}
    };
    m_skills.append(lt);

    // frost_nova - 冰弹投射物
    SkillData fn;
    fn.id = "frost_nova"; fn.name = "冰霜新星"; fn.type = "active";
    fn.description = "向最近敌人发射冰弹，命中造成范围伤害"; fn.maxLevel = 5;
    fn.levels = {
        {15, 3.5f, 60, "伤害15，爆炸范围60px"},
        {25, 3.2f, 70, "伤害25，爆炸范围70px"},
        {40, 2.8f, 80, "伤害40，爆炸范围80px"},
        {60, 2.4f, 90, "伤害60，爆炸范围90px"},
        {80, 2.0f, 100, "伤害80，爆炸范围100px"}
    };
    m_skills.append(fn);

    // attack_boost
    SkillData ab;
    ab.id = "attack_boost"; ab.name = "攻击强化"; ab.type = "passive"; ab.description = "永久提升攻击力"; ab.maxLevel = 5;
    ab.levels = {
        {0, 0, 5, "攻击+5"}, {0, 0, 10, "攻击+10"}, {0, 0, 18, "攻击+18"},
        {0, 0, 30, "攻击+30"}, {0, 0, 50, "攻击+50"}
    };
    m_skills.append(ab);

    // speed_boost
    SkillData sb;
    sb.id = "speed_boost"; sb.name = "敏捷"; sb.type = "passive"; sb.description = "永久提升移动速度"; sb.maxLevel = 5;
    sb.levels = {
        {0, 0, 1, "速度+1"}, {0, 0, 2, "速度+2"}, {0, 0, 3, "速度+3"},
        {0, 0, 5, "速度+5"}, {0, 0, 7, "速度+7"}
    };
    m_skills.append(sb);

    // life_drain
    SkillData ld;
    ld.id = "life_drain"; ld.name = "生命汲取"; ld.type = "passive";
    ld.description = "击杀敌人有几率掉落血瓶"; ld.maxLevel = 5;
    ld.levels = {
        {0, 0, 10, "10%掉落血瓶，恢复5HP"},
        {0, 0, 15, "15%掉落血瓶，恢复5HP"},
        {0, 0, 20, "20%掉落血瓶，恢复5HP"},
        {0, 0, 25, "25%掉落血瓶，恢复5HP"},
        {0, 0, 30, "30%掉落血瓶，恢复5HP"}
    };
    m_skills.append(ld);
}

void GameData::initDefaultWaves()
{
    m_waveConfigs.clear();
    m_waveConfigs.append({1,  0,   {"slime"},                    8,  3.0f, false, false});
    m_waveConfigs.append({2,  30,  {"slime","goblin"},           12, 2.5f, false, false});
    m_waveConfigs.append({3,  60,  {"goblin"},                   15, 2.2f, false, false});
    m_waveConfigs.append({4,  100, {"goblin","slime"},           18, 2.0f, false, false});
    m_waveConfigs.append({5,  140, {"goblin_elite"},             1,  0.0f, false, true});
    m_waveConfigs.append({6,  150, {"skeleton"},                 15, 1.8f, false, false});
    m_waveConfigs.append({7,  200, {"skeleton","goblin"},        18, 1.6f, false, false});
    m_waveConfigs.append({8,  250, {"bat"},                      20, 1.5f, false, false});
    m_waveConfigs.append({9,  300, {"bat","skeleton"},           22, 1.3f, false, false});
    m_waveConfigs.append({10, 360, {"dragon"},                   1,  0.0f, true,  false});
    m_waveConfigs.append({11, 370, {"goblin","bat"},             20, 1.2f, false, false});
    m_waveConfigs.append({12, 430, {"skeleton","bat"},           22, 1.1f, false, false});
    m_waveConfigs.append({13, 490, {"goblin_elite","skeleton"},  3,  1.0f, false, true});
    m_waveConfigs.append({14, 510, {"bat","skeleton","goblin"},  25, 0.9f, false, false});
    m_waveConfigs.append({15, 570, {"goblin","skeleton","bat"},  28, 0.8f, false, false});
    m_waveConfigs.append({16, 630, {"dragon"},                   1,  0.0f, true,  false});
    m_waveConfigs.append({17, 640, {"bat","bat","bat"},          30, 0.7f, false, false});
    m_waveConfigs.append({18, 710, {"skeleton","goblin_elite"},  25, 0.7f, false, false});
    m_waveConfigs.append({19, 780, {"bat","skeleton","goblin_elite"}, 30, 0.6f, false, false});
    m_waveConfigs.append({20, 840, {"dragon","goblin_elite"},    3,  0.5f, true,  false});
}
