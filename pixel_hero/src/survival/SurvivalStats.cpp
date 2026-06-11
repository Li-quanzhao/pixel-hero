#include "SurvivalStats.h"
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QDateTime>
#include <QCoreApplication>

SurvivalStats::SurvivalStats(QObject* parent)
    : QObject(parent)
    , m_kills(0), m_currentWave(0), m_elapsedTime(0)
    , m_gold(0)
    , m_recordWave(0), m_recordKills(0), m_recordTime(0)
{
    loadRecord();
}

bool SurvivalStats::isNewRecord() const
{
    return m_currentWave > m_recordWave;
}

void SurvivalStats::saveRecord(int wave, int kills, float time)
{
    if (wave > m_recordWave) {
        m_recordWave = wave;
        m_recordKills = kills;
        m_recordTime = time;

        QJsonObject obj;
        obj["wave"] = m_recordWave;
        obj["kills"] = m_recordKills;
        obj["time"] = m_recordTime;

        QDir().mkpath(QFileInfo(recordPath()).absolutePath());
        QFile file(recordPath());
        if (file.open(QIODevice::WriteOnly)) {
            file.write(QJsonDocument(obj).toJson());
        }
    }
}

void SurvivalStats::loadRecord()
{
    QFile file(recordPath());
    if (!file.open(QIODevice::ReadOnly)) return;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject obj = doc.object();
    m_recordWave  = obj["wave"].toInt();
    m_recordKills = obj["kills"].toInt();
    m_recordTime  = static_cast<float>(obj["time"].toDouble());
}

bool SurvivalStats::saveCurrentGame(const SurvivalSaveData& data, int slot)
{
    QString path = savePath(slot);
    if (path.isEmpty()) return false;

    QJsonObject obj;
    obj["wave"]  = data.currentWave;
    obj["kills"] = data.totalKills;
    obj["time"]  = data.elapsedTime;
    obj["level"] = data.playerLevel;
    obj["hp"]    = data.playerHealth;
    obj["maxHp"] = data.playerMaxHealth;
    obj["exp"]   = data.playerExp;
    obj["character"] = data.characterId;
    obj["weapon"]    = data.weaponId;
    obj["gold"] = data.playerGold;
    obj["savedAt"]   = QDateTime::currentDateTime().toString(Qt::ISODate);

    QJsonArray skillsArr;
    for (const auto& pair : data.skills) {
        QJsonObject so;
        so["id"] = pair.first;
        so["level"] = pair.second;
        skillsArr.append(so);
    }
    obj["skills"] = skillsArr;

    QDir().mkpath(QFileInfo(path).absolutePath());
    QFile file(path);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(obj).toJson());
        file.close();
        return true;
    }
    m_lastError = file.errorString();
    return false;
}

bool SurvivalStats::loadSavedGame(SurvivalSaveData& data, int slot)
{
    QFile file(savePath(slot));
    if (!file.open(QIODevice::ReadOnly)) return false;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject obj = doc.object();

    data.currentWave  = obj["wave"].toInt();
    data.totalKills   = obj["kills"].toInt();
    data.elapsedTime  = static_cast<float>(obj["time"].toDouble());
    data.playerLevel  = obj["level"].toInt();
    data.playerHealth = obj["hp"].toInt();
    data.playerMaxHealth = obj["maxHp"].toInt();
    data.playerExp    = obj["exp"].toInt();
    data.characterId  = obj["character"].toString("warrior");
    data.weaponId     = obj["weapon"].toString("short_sword");
    data.playerGold   = obj["gold"].toInt(0);
    data.savedAt      = QDateTime::fromString(obj["savedAt"].toString(), Qt::ISODate);

    data.skills.clear();
    QJsonArray skillsArr = obj["skills"].toArray();
    for (const QJsonValue& v : skillsArr) {
        QJsonObject so = v.toObject();
        data.skills.append({so["id"].toString(), so["level"].toInt()});
    }
    return true;
}

bool SurvivalStats::hasSavedGame(int slot) const
{
    return QFile::exists(savePath(slot));
}

void SurvivalStats::deleteSavedGame(int slot)
{
    QFile::remove(savePath(slot));
}

QList<int> SurvivalStats::allSavedSlots() const
{
    QList<int> result;
    for (int i = 0; i < 5; ++i) {
        if (hasSavedGame(i)) result.append(i);
    }
    return result;
}

QString SurvivalStats::savePath(int slot) const
{
    static const QString dir = []() -> QString {
        // 优先 exe 同目录 saves/ — 开发环境100%可写
        QString d = QCoreApplication::applicationDirPath() + "/saves";
        QDir().mkpath(d + "/PixelHero");
        return d + "/PixelHero";
    }();
    return dir + QString("/survival_save_%1.json").arg(slot);
}

QString SurvivalStats::recordPath() const
{
    static const QString dir = []() -> QString {
        QString d = QCoreApplication::applicationDirPath() + "/saves";
        QDir().mkpath(d + "/PixelHero");
        return d + "/PixelHero";
    }();
    return dir + "/survival_record.json";
}
