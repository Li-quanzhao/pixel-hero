#include "Skill.h"

Skill::Skill(const QString& id, const QString& name)
    : m_id(id), m_name(name), m_damage(0), m_cooldown(0), m_targetType(ENEMY)
{
}

Skill::~Skill()
{
}

QString Skill::id() const { return m_id; }
QString Skill::name() const { return m_name; }
int Skill::damage() const { return m_damage; }
int Skill::cooldown() const { return m_cooldown; }
Skill::TargetType Skill::targetType() const { return m_targetType; }

void Skill::setDamage(int damage) { m_damage = damage; }
void Skill::setCooldown(int cooldown) { m_cooldown = cooldown; }
void Skill::setTargetType(TargetType type) { m_targetType = type; }

void Skill::execute(Player* player)
{
    Q_UNUSED(player);
}