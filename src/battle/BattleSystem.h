#ifndef BATTLESYSTEM_H
#define BATTLESYSTEM_H

#include <QObject>
#include "Player.h"
#include "Enemy.h"
#include "Skill.h"

class BattleSystem : public QObject
{
    Q_OBJECT

public:
    BattleSystem(QObject *parent = nullptr);
    ~BattleSystem();

    void startBattle(Player* player, Enemy* enemy);
    void playerAttack(Player* player, Enemy* enemy);
    void enemyAttack(Enemy* enemy, Player* player);
    void useSkill(Player* player, Skill* skill, Enemy* enemy);
    bool isBattleOver();
    void endBattle();

signals:
    void battleStarted();
    void playerAttacked(int damage);
    void enemyAttacked(int damage);
    void battleEnded(bool playerWon);

private:
    Player *m_player;
    Enemy *m_enemy;
    bool m_isBattleActive;
};

#endif // BATTLESYSTEM_H