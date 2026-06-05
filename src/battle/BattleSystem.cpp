#include "BattleSystem.h"

BattleSystem::BattleSystem(QObject *parent)
    : QObject(parent), m_player(nullptr), m_enemy(nullptr), m_isBattleActive(false)
{
}

BattleSystem::~BattleSystem()
{
}

void BattleSystem::startBattle(Player* player, Enemy* enemy)
{
    m_player = player;
    m_enemy = enemy;
    m_isBattleActive = true;
    emit battleStarted();
}

void BattleSystem::playerAttack(Player* player, Enemy* enemy)
{
    if (!m_isBattleActive) return;

    const Player* constPlayer = player;
    int baseAttack = constPlayer->attack();
    int damage = baseAttack + (player->getEquippedWeapon() ? player->getEquippedWeapon()->attackBonus() : 0);
    enemy->takeDamage(damage);
    emit enemyAttacked(damage);

    if (!enemy->isAlive()) {
        endBattle();
    }
}

void BattleSystem::enemyAttack(Enemy* enemy, Player* player)
{
    if (!m_isBattleActive) return;

    int damage = enemy->attack();
    player->takeDamage(damage);
    emit playerAttacked(damage);

    if (player->health() <= 0) {
        endBattle();
    }
}

void BattleSystem::useSkill(Player* player, Skill* skill, Enemy* enemy)
{
    if (!m_isBattleActive) return;

    skill->execute(player);
    
    if (skill->targetType() == Skill::ENEMY) {
        int damage = skill->damage();
        enemy->takeDamage(damage);
        emit enemyAttacked(damage);

        if (!enemy->isAlive()) {
            endBattle();
        }
    }
}

bool BattleSystem::isBattleOver()
{
    return !m_isBattleActive;
}

void BattleSystem::endBattle()
{
    bool playerWon = m_enemy && !m_enemy->isAlive();
    m_isBattleActive = false;
    emit battleEnded(playerWon);
}