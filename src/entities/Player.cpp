#include "Player.h"
#include "Enemy.h"

Player::Player(QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent),
      m_level(1), m_exp(0), m_health(100), m_maxHealth(100),
      m_attack(10), m_defense(5), m_speed(3),
      m_moveUp(false), m_moveDown(false), m_moveLeft(false), m_moveRight(false),
      m_isAttacking(false), m_currentAttackCooldown(0), m_attackCooldown(0.5),
      m_equippedWeapon(nullptr), m_equippedArmor(nullptr)
{
    QPixmap playerPixmap(48, 48);
    playerPixmap.fill(QColor(0x4a, 0x90, 0xa4));
    setPixmap(playerPixmap);
    setOffset(-boundingRect().width() / 2, -boundingRect().height());
}

Player::~Player()
{
    delete m_equippedWeapon;
    delete m_equippedArmor;
    qDeleteAll(m_skills);
}

int Player::level() const { return m_level; }
int Player::exp() const { return m_exp; }
int Player::health() const { return m_health; }
int Player::maxHealth() const { return m_maxHealth; }
int Player::attack() const { return m_attack; }
int Player::defense() const { return m_defense; }
int Player::speed() const { return m_speed; }

void Player::setLevel(int level) { m_level = level; }
void Player::setExp(int exp) { m_exp = exp; }
void Player::setHealth(int health) { m_health = qMax(0, qMin(health, m_maxHealth)); }
void Player::setMaxHealth(int maxHealth) { m_maxHealth = maxHealth; }
void Player::setAttack(int attack) { m_attack = attack; }
void Player::setDefense(int defense) { m_defense = defense; }
void Player::setSpeed(int speed) { m_speed = speed; }

void Player::update(qreal deltaTime)
{
    if (m_currentAttackCooldown > 0) {
        m_currentAttackCooldown -= deltaTime;
    }

    move(deltaTime);
}

void Player::attackEnemy(Enemy* enemy)
{
    if (m_currentAttackCooldown > 0) return;

    int totalAttack = m_attack;
    if (m_equippedWeapon) {
        totalAttack += m_equippedWeapon->attackBonus();
    }

    enemy->takeDamage(totalAttack);
    m_currentAttackCooldown = m_attackCooldown;
}

void Player::takeDamage(int damage)
{
    int totalDefense = m_defense;
    if (m_equippedArmor) {
        totalDefense += m_equippedArmor->defenseBonus();
    }

    int finalDamage = qMax(0, damage - totalDefense);
    m_health -= finalDamage;

    if (m_health <= 0) {
        m_health = 0;
    }
}

void Player::useSkill(int skillIndex)
{
    if (skillIndex >= 0 && skillIndex < m_skills.size()) {
        m_skills[skillIndex]->execute(this);
    }
}

void Player::addExp(int amount)
{
    m_exp += amount;
    checkLevelUp();
}

Weapon* Player::getEquippedWeapon() const { return m_equippedWeapon; }

void Player::setMoveUp(bool value) { m_moveUp = value; }
void Player::setMoveDown(bool value) { m_moveDown = value; }
void Player::setMoveLeft(bool value) { m_moveLeft = value; }
void Player::setMoveRight(bool value) { m_moveRight = value; }

void Player::attack()
{
    m_isAttacking = true;
}

void Player::move(qreal deltaTime)
{
    qreal dx = 0;
    qreal dy = 0;

    if (m_moveUp) dy -= m_speed * deltaTime * 60;
    if (m_moveDown) dy += m_speed * deltaTime * 60;
    if (m_moveLeft) dx -= m_speed * deltaTime * 60;
    if (m_moveRight) dx += m_speed * deltaTime * 60;

    if (dx != 0 || dy != 0) {
        setPos(x() + dx, y() + dy);
    }
}

void Player::checkLevelUp()
{
    int expNeeded = m_level * 100;
    while (m_exp >= expNeeded) {
        m_exp -= expNeeded;
        m_level++;
        m_maxHealth += 20;
        m_health = m_maxHealth;
        m_attack += 3;
        m_defense += 2;
        expNeeded = m_level * 100;
    }
}