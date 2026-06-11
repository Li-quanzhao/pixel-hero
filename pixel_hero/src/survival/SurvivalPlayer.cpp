#include "SurvivalPlayer.h"
#include "entities/Enemy.h"
#include <QPainter>

QList<SurvivalPlayer::CharacterConfig> SurvivalPlayer::availableCharacters()
{
    return {
        {"warrior", "战士",    120, 12, 5, 3, QColor(0x4a, 0x90, 0xa4),
         "HP 120  攻击 12\n防御 5  速度 3\n均衡型，适合新手"},
        {"archer",  "弓箭手",   80, 15, 2, 4, QColor(0x22, 0x8b, 0x22),
         "HP 80  攻击 15\n防御 2  速度 4\n高攻速脆皮"},
        {"mage",    "法师",     70, 20, 1, 3, QColor(0x99, 0x32, 0xcc),
         "HP 70  攻击 20\n防御 1  速度 3\n玻璃大炮"}
    };
}

SurvivalPlayer::SurvivalPlayer(QGraphicsItem* parent)
    : Player(parent)
    , m_passiveAttackBoost(0)
    , m_passiveSpeedBoost(0)
    , m_pendingLevelUps(0)
    , m_gold(0)
{
    setHealth(100); setMaxHealth(100);
    setAttack(10); setDefense(3); setSpeed(3);
}

SurvivalPlayer::~SurvivalPlayer() {}

void SurvivalPlayer::applyCharacter(const CharacterConfig& cfg)
{
    setMaxHealth(cfg.hp); setHealth(cfg.hp);
    setAttack(cfg.atk); setDefense(cfg.def); setSpeed(cfg.spd);
    // 加载角色精灵图
    setPixmap(QPixmap(QString(":/sprites/characters/%1").arg(cfg.id)));
    setOffset(-24, -48);
}

// ---- 技能管理 ----
void SurvivalPlayer::addSkill(const QString& id, int level)
{
    const SkillData* sd = GameData::instance()->getSkillById(id);
    if (!sd || level < 1 || level > sd->maxLevel) return;

    // 检查是否已有该技能
    for (auto& as : m_activeSkills) {
        if (as.skillId == id) return;
    }

    const SkillData::LevelData& ld = sd->levels[level - 1];
    ActiveSkill skill;
    skill.skillId        = id;
    skill.level          = level;
    skill.damage         = ld.damage;
    skill.cooldown       = ld.cooldown;
    skill.currentCooldown= 0;
    skill.extra          = ld.extra;
    m_activeSkills.append(skill);

    recalcPassives();
}

void SurvivalPlayer::upgradeSkill(const QString& id, int newLevel)
{
    const SkillData* sd = GameData::instance()->getSkillById(id);
    if (!sd || newLevel > sd->maxLevel) return;

    for (auto& as : m_activeSkills) {
        if (as.skillId == id) {
            const SkillData::LevelData& ld = sd->levels[newLevel - 1];
            as.level   = newLevel;
            as.damage  = ld.damage;
            as.cooldown= ld.cooldown;
            as.extra   = ld.extra;
            recalcPassives();
            return;
        }
    }
    // 不存在则新添加
    addSkill(id, newLevel);
}

int SurvivalPlayer::skillLevel(const QString& id) const
{
    for (const auto& as : m_activeSkills) {
        if (as.skillId == id) return as.level;
    }
    return 0;
}

// ---- 被动加成重算 ----
void SurvivalPlayer::recalcPassives()
{
    m_passiveAttackBoost = 0;
    m_passiveSpeedBoost  = 0;
    for (const auto& as : m_activeSkills) {
        if (as.skillId == "attack_boost") m_passiveAttackBoost += as.extra;
        if (as.skillId == "speed_boost")  m_passiveSpeedBoost  += as.extra;
    }
}

int SurvivalPlayer::attack() const
{
    return Player::attack() + m_passiveAttackBoost;
}

int SurvivalPlayer::speed() const
{
    return Player::speed() + m_passiveSpeedBoost;
}

// ---- 帧更新 ----
void SurvivalPlayer::update(qreal deltaTime)
{
    Player::update(deltaTime);

    // 技能冷却倒计时
    for (auto& as : m_activeSkills) {
        if (as.currentCooldown > 0)
            as.currentCooldown -= deltaTime;
    }
}

void SurvivalPlayer::autoCastSkills()
{
    // 由 SurvivalScene 调用，自动释放冷却完毕的主动技能
}

// ---- 升级检测 ----
void SurvivalPlayer::checkLevelUp()
{
    int need = level() * 80;
    while (exp() >= need) {
        setExp(exp() - need);
        m_pendingLevelUps++;
        need = level() * 80;
    }
}

void SurvivalPlayer::applyPendingLevelUp()
{
    if (m_pendingLevelUps <= 0) return;
    m_pendingLevelUps--;
    setLevel(level() + 1);
    setMaxHealth(maxHealth() + 20);
    setHealth(maxHealth());
    setAttack(Player::attack() + 3);
    setDefense(defense() + 2);
}
