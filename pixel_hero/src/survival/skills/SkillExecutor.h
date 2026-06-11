#ifndef SKILLEXECUTOR_H
#define SKILLEXECUTOR_H

#include <QString>
#include <QList>
#include <QPointF>
#include <QMap>
#include <memory>
#include "SurvivalPlayer.h"

class Enemy;
class SurvivalPlayer;

namespace pixel_hero {
namespace survival {

class EffectManager;
class EnemyManager;

class SkillExecutor
{
public:
    virtual ~SkillExecutor() = default;

    virtual void execute(const SurvivalPlayer::ActiveSkill& skill,
                         QPointF playerPos,
                         EnemyManager* enemyMgr,
                         EffectManager* fx) = 0;

    static std::unique_ptr<SkillExecutor> create(const QString& skillId);
};

// ---- 子类 ----

class FireballExecutor : public SkillExecutor
{
public:
    void execute(const SurvivalPlayer::ActiveSkill& skill, QPointF playerPos,
                 EnemyManager* enemyMgr, EffectManager* fx) override;
};

class LightningExecutor : public SkillExecutor
{
public:
    void execute(const SurvivalPlayer::ActiveSkill& skill, QPointF playerPos,
                 EnemyManager* enemyMgr, EffectManager* fx) override;
};

class FrostNovaExecutor : public SkillExecutor
{
public:
    void execute(const SurvivalPlayer::ActiveSkill& skill, QPointF playerPos,
                 EnemyManager* enemyMgr, EffectManager* fx) override;
};

} // namespace survival
} // namespace pixel_hero

#endif // SKILLEXECUTOR_H
