#include "Player.h"
#include "Enemy.h"

Player::Player(QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent)
    , m_level(1), m_exp(0), m_health(100), m_maxHealth(100)
    , m_attack(10), m_defense(5), m_speed(3), m_gold(0)
    , m_equippedWeapon(nullptr), m_equippedArmor(nullptr)
    , m_isAttacking(false)
    , m_attackCooldown(0.5), m_currentAttackCooldown(0)
{
    QPixmap playerPixmap(":/resources/images/entities/player.png");
    if (playerPixmap.isNull()) {
        playerPixmap = QPixmap(48, 48);
        playerPixmap.fill(QColor(0x4a, 0x90, 0xa4));
    }
    setPixmap(playerPixmap);
    setOffset(-boundingRect().width() / 2, -boundingRect().height());
}

Player::~Player()
{
    delete m_equippedWeapon;
    delete m_equippedArmor;
    qDeleteAll(m_skills);
    qDeleteAll(m_inventory);
}

// ---- 属性 get/set ----
int  Player::level()    const { return m_level; }
int  Player::exp()      const { return m_exp; }
int  Player::health()   const { return m_health; }
int  Player::maxHealth()const { return m_maxHealth; }
int  Player::attack()   const { return m_attack; }
int  Player::defense()  const { return m_defense; }
int  Player::speed()    const { return m_speed; }
int  Player::gold()     const { return m_gold; }
void Player::setLevel(int v)    { m_level = v; }
void Player::setExp(int v)      { m_exp = v; }
void Player::setHealth(int v)   { m_health = qMax(0, qMin(v, m_maxHealth)); }
void Player::setMaxHealth(int v){ m_maxHealth = v; }
void Player::setAttack(int v)   { m_attack = v; }
void Player::setDefense(int v)  { m_defense = v; }
void Player::setSpeed(int v)    { m_speed = v; }
void Player::setGold(int v)     { m_gold = qMax(0, v); }
void Player::applySaveData(int lvl, int e, int hp, int mhp, int atk, int def, int g)
    { m_level=lvl; m_exp=e; m_health=hp; m_maxHealth=mhp; m_attack=atk; m_defense=def; m_gold=g; }

// ---- 装备 ----
Weapon* Player::getEquippedWeapon() const { return m_equippedWeapon; }
Armor*  Player::getEquippedArmor()  const { return m_equippedArmor; }
void Player::equipWeapon(Weapon* w) { if (m_equippedWeapon) addItemToInventory(m_equippedWeapon); m_equippedWeapon = w; }
void Player::unequipWeapon()        { if (m_equippedWeapon) addItemToInventory(m_equippedWeapon); m_equippedWeapon = nullptr; }
void Player::equipArmor(Armor* a)   { if (m_equippedArmor) addItemToInventory(m_equippedArmor); m_equippedArmor = a; }
void Player::unequipArmor()         { if (m_equippedArmor) addItemToInventory(m_equippedArmor); m_equippedArmor = nullptr; }

// ---- 背包 ----
void Player::addItemToInventory(Item* item)    { m_inventory.append(item); }
void Player::removeItemFromInventory(int index){ if (index>=0&&index<m_inventory.size()){delete m_inventory.takeAt(index);} }
Item* Player::itemAt(int index) const          { return (index>=0&&index<m_inventory.size())?m_inventory[index]:nullptr; }
int   Player::itemCount() const                { return m_inventory.size(); }
QList<Item*>& Player::inventory()              { return m_inventory; }

// ---- Update ----
void Player::update(qreal deltaTime)
{
    if (m_currentAttackCooldown > 0) m_currentAttackCooldown -= deltaTime;
}

void Player::attackEnemy(Enemy* enemy)
{
    if (m_currentAttackCooldown > 0) return;
    int total = m_attack + (m_equippedWeapon ? m_equippedWeapon->attackBonus() : 0);
    enemy->takeDamage(total);
    m_currentAttackCooldown = m_attackCooldown;
}

void Player::takeDamage(int damage)
{
    int totalDef = m_defense + (m_equippedArmor ? m_equippedArmor->defenseBonus() : 0);
    int final = qMax(0, damage - totalDef);
    m_health -= final;
    if (m_health < 0) m_health = 0;
}

void Player::useSkill(int skillIndex)
{
    if (skillIndex >= 0 && skillIndex < m_skills.size())
        m_skills[skillIndex]->execute(this);
}

void Player::addExp(int amount) { m_exp += amount; checkLevelUp(); }

Player::MoveState& Player::moveState() { return m_moveState; }
void Player::setMoveUp(bool v)    { m_moveState.moveUp = v; }
void Player::setMoveDown(bool v)  { m_moveState.moveDown = v; }
void Player::setMoveLeft(bool v)  { m_moveState.moveLeft = v; }
void Player::setMoveRight(bool v) { m_moveState.moveRight = v; }
void Player::doAttack() { m_isAttacking = true; }

void Player::checkLevelUp()
{
    int need = m_level * 100;
    while (m_exp >= need) {
        m_exp -= need;
        m_level++;
        m_maxHealth += 20; m_health = m_maxHealth;
        m_attack += 3; m_defense += 2;
        need = m_level * 100;
    }
}
