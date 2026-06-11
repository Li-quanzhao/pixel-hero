#ifndef SURVIVALSTATS_H
#define SURVIVALSTATS_H

#include <QObject>
#include <QJsonObject>
#include "SurvivalSaveData.h"

class SurvivalStats : public QObject
{
    Q_OBJECT

public:
    SurvivalStats(QObject* parent = nullptr);

    // 局内统计
    void setWave(int w)       { m_currentWave = w; }
    void setElapsedTime(float t) { m_elapsedTime = t; }
    void addKill()              { m_kills++; }
    
    int currentWave() const   { return m_currentWave; }
    int kills() const         { return m_kills; }
    int gold() const          { return m_gold; }
    float elapsedTime() const { return m_elapsedTime; }
    void setGold(int g)       { m_gold = g; }

    // 记录
    bool isNewRecord() const;
    int recordWave() const  { return m_recordWave; }
    int recordKills() const { return m_recordKills; }
    float recordTime() const { return m_recordTime; }
    void saveRecord(int wave, int kills, float time);

    // 5槽存档
    bool saveCurrentGame(const SurvivalSaveData& data, int slot);
    bool loadSavedGame(SurvivalSaveData& data, int slot);
    bool hasSavedGame(int slot) const;
    void deleteSavedGame(int slot);
    QList<int> allSavedSlots() const;  // 哪些槽有存档
    QString savePath(int slot) const;
    QString lastError() const { return m_lastError; }

private:
    int    m_kills;
    int    m_currentWave;
    float  m_elapsedTime;
    int    m_gold;

    int    m_recordWave;
    int    m_recordKills;
    float  m_recordTime;

    QString m_lastError;

    void loadRecord();
    QString recordPath() const;
};

#endif // SURVIVALSTATS_H
