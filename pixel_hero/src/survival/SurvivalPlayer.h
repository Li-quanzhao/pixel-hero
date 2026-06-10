#ifndef SURVIVALPLAYER_H
#define SURVIVALPLAYER_H

#include "entities/Player.h"
#include "utils/GameData.h"
#include <QList>
#include <QPair>

class SurvivalPlayer : public Player
{

public:
    struct ActiveSkill {
        QString skillId;
        int level;
        int damage;
        float cooldown;
        float currentCooldown;
        int extra;
    };

    // 角色配置
    struct CharacterConfig {
        QString id;
        QString name;
        int hp;
        int atk;
        int def;
        int spd;
        QColor color;
        QString desc;
    };

    static QList<CharacterConfig> availableCharacters();

    SurvivalPlayer(QGraphicsItem* parent = nullptr);
    ~SurvivalPlayer();

    void applyCharacter(const CharacterConfig& cfg);

    // 技能管理
    void addSkill(const QString& id, int level);
    void upgradeSkill(const QString& id, int newLevel);
    int  skillLevel(const QString& id) const;
    const QList<ActiveSkill>& activeSkills() const { return m_activeSkills; }
    QList<ActiveSkill>& activeSkills() { return m_activeSkills; }

    // 属性（基础值 + 被动加成）
    int attack() const override;
    int speed() const override;

    // 帧更新（技能冷却 + 自动释放）
    void update(qreal deltaTime) override;

    // 升级检查（重写父类 — 仅计数，不立即升级）
    void checkLevelUp() override;
    int  pendingLevelUps() const { return m_pendingLevelUps; }
    void applyPendingLevelUp();   // 消耗一次待升级并应用属性加成

private:
    QList<ActiveSkill> m_activeSkills;

    int m_passiveAttackBoost;
    int m_passiveSpeedBoost;
    int m_pendingLevelUps;

    void recalcPassives();
    void autoCastSkills();
};

#endif // SURVIVALPLAYER_H
