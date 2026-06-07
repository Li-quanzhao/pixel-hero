#ifndef SKILL_H
#define SKILL_H

#include <QString>

class Player;

class Skill
{
public:
    enum TargetType {
        SELF,
        ENEMY,
        ALL_ENEMIES
    };

    Skill(const QString& id, const QString& name);
    virtual ~Skill();

    QString id() const;
    QString name() const;
    int damage() const;
    int cooldown() const;
    TargetType targetType() const;

    void setDamage(int damage);
    void setCooldown(int cooldown);
    void setTargetType(TargetType type);

    virtual void execute(Player* player);

private:
    QString m_id;
    QString m_name;
    int m_damage;
    int m_cooldown;
    TargetType m_targetType;
};

#endif // SKILL_H