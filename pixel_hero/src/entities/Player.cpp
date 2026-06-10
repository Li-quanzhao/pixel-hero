#include "Player.h"
#include "Enemy.h"
#include <cmath>

Player::Player(QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent)
    , m_level(1), m_exp(0), m_health(100), m_maxHealth(100)
    , m_attack(10), m_defense(3), m_speed(3)
    , m_attackCooldown(0.5), m_currentAttackCooldown(0)
    , m_invincible(false), m_invincibleTimer(0), m_blinkTimer(0)
{
    QPixmap pixmap(48, 48);
    pixmap.fill(QColor(0x4a, 0x90, 0xa4));
    setPixmap(pixmap);
    setOffset(-boundingRect().width() / 2, -boundingRect().height());
}

Player::~Player()
{
}

// ---- 属性 ----
int  Player::level()    const { return m_level; }
int  Player::exp()      const { return m_exp; }
int  Player::health()   const { return m_health; }
int  Player::maxHealth()const { return m_maxHealth; }
int  Player::attack()   const { return m_attack; }
int  Player::defense()  const { return m_defense; }
int  Player::speed()    const { return m_speed; }
void Player::setLevel(int v)      { m_level = v; }
void Player::setExp(int v)        { m_exp = v; }
void Player::setHealth(int v)     { m_health = qMax(0, qMin(v, m_maxHealth)); }
void Player::setMaxHealth(int v)  { m_maxHealth = v; }
void Player::setAttack(int v)     { m_attack = v; }
void Player::setDefense(int v)    { m_defense = v; }
void Player::setSpeed(int v)      { m_speed = v; }

// ---- 战斗 ----
void Player::update(qreal deltaTime)
{
    if (m_currentAttackCooldown > 0) m_currentAttackCooldown -= deltaTime;

    // 无敌计时 + 闪烁
    if (m_invincible) {
        m_invincibleTimer -= (float)deltaTime;
        m_blinkTimer      -= (float)deltaTime;

        if (m_blinkTimer <= 0) {
            m_blinkTimer = BLINK_INTERVAL;
            setVisible(!isVisible());
        }

        if (m_invincibleTimer <= 0) {
            m_invincible = false;
            m_invincibleTimer = 0;
            m_blinkTimer = 0;
            setVisible(true); // 确保恢复正常
        }
    }
}

void Player::attackEnemy(Enemy* enemy)
{
    if (m_currentAttackCooldown > 0) return;
    int totalAtk = m_attack + m_weapon.attackBonus;
    enemy->takeDamage(totalAtk);
    m_currentAttackCooldown = m_attackCooldown / m_weapon.speedMod;
}

void Player::takeDamage(int damage)
{
    if (m_invincible) return;

    // 减伤率 = def * 5%，上限80%
    float dr = fminf(0.8f, m_defense * 0.05f);
    int final = qMax(1, (int)(damage * (1.0f - dr)));
    m_health -= final;
    if (m_health < 0) m_health = 0;

    // 进入无敌状态
    if (m_health > 0) {
        m_invincible = true;
        m_invincibleTimer = INVINCIBLE_TIME;
        m_blinkTimer = BLINK_INTERVAL;
        setVisible(false); // 首帧闪烁
    }
}

bool Player::isInvincible() const { return m_invincible; }

void Player::addExp(int amount) { m_exp += amount; checkLevelUp(); }

bool Player::isAlive() const { return m_health > 0; }

// ---- 移动 ----
Player::MoveState& Player::moveState() { return m_moveState; }
void Player::setMoveUp(bool v)    { m_moveState.moveUp = v; }
void Player::setMoveDown(bool v)  { m_moveState.moveDown = v; }
void Player::setMoveLeft(bool v)  { m_moveState.moveLeft = v; }
void Player::setMoveRight(bool v) { m_moveState.moveRight = v; }

// ---- 升级 ----
void Player::checkLevelUp()
{
    int need = m_level * 80;
    while (m_exp >= need) {
        m_exp -= need;
        m_level++;
        m_maxHealth += 20;
        m_health = m_maxHealth;
        m_attack += 3;
        m_defense += 2;
        need = m_level * 80;
    }
}

void Player::applyWeapon(const WeaponData& weapon)
{
    m_weapon = weapon;
    m_currentAttackCooldown = 0;
}

float Player::attackSpeed() const
{
    return 1.0f / m_attackCooldown * m_weapon.speedMod;
}
