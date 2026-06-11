#include "PickupManager.h"
#include "PickupItem.h"
#include "SurvivalPlayer.h"
#include "utils/GameData.h"
#include "config/GameConfig.h"
#include <QLineF>
#include <QRandomGenerator>

namespace pixel_hero {
namespace survival {

PickupManager::PickupManager(QGraphicsScene* scene, QObject* parent)
    : QObject(parent)
    , m_scene(scene)
{
}

PickupManager::~PickupManager()
{
    clear();
}

// ========== 掉落表 ==========

const PickupManager::DropEntry& PickupManager::dropEntry(int enemyType)
{
    // 索引对应 Enemy::EnemyType 枚举
    static const DropEntry table[] = {
        // GOBLIN(0):     min, max, xpW, goldW
        { 1, 2, 4, 4 },
        // SLIME(1):
        { 1, 2, 5, 4 },
        // SKELETON(2):
        { 2, 3, 3, 3 },
        // BAT(3):
        { 1, 2, 5, 5 },
        // GOBLIN_ELITE(4):
        { 3, 4, 2, 2 },
        // DRAGON(5):
        { 5, 6, 2, 2 },
    };
    return table[enemyType];
}

// ========== 公共方法 ==========

void PickupManager::spawnPickups(QPointF pos, int enemyType, SurvivalPlayer* player)
{
    if (!m_scene || !player) return;

    const DropEntry& entry = dropEntry(enemyType);
    int count = QRandomGenerator::global()->bounded(entry.minCount, entry.maxCount + 1);

    for (int i = 0; i < count; i++) {
        if (m_pickups.size() >= pixel_hero::config::MAX_PICKUPS) return;
        PickupItem::PickupType type = randomType(entry);
        int value = randomValue(type);
        // 随机偏移，避免重叠
        float ox = QRandomGenerator::global()->bounded(16) - 8.0f;
        float oy = QRandomGenerator::global()->bounded(16) - 8.0f;
        createPickup(QPointF(pos.x() + ox, pos.y() + oy), type, value);
    }

    // 血瓶额外判定：life_drain 技能
    if (m_pickups.size() >= pixel_hero::config::MAX_PICKUPS) return;
    int skillLvl = player->skillLevel("life_drain");
    if (skillLvl > 0) {
        const SkillData* sd = GameData::instance()->getSkillById("life_drain");
        if (sd && skillLvl <= sd->levels.size()) {
            float chance = sd->levels[skillLvl - 1].extra / 100.0f;
            if (QRandomGenerator::global()->generateDouble() < chance) {
                float ox = QRandomGenerator::global()->bounded(16) - 8.0f;
                float oy = QRandomGenerator::global()->bounded(16) - 8.0f;
                createPickup(QPointF(pos.x() + ox, pos.y() + oy),
                             PickupItem::HP, pixel_hero::config::HP_DROP_RECOVERY);
            }
        }
    }
}

void PickupManager::update(qreal dt, SurvivalPlayer* player)
{
    magnetizePickups(dt, player);
    updateLifetimes(dt);
}

void PickupManager::clear()
{
    for (PickupItem* item : m_pickups) {
        if (item->scene()) m_scene->removeItem(item);
        delete item;
    }
    m_pickups.clear();
}

// ========== 内部方法 ==========

PickupItem* PickupManager::createPickup(QPointF pos, PickupItem::PickupType type, int value)
{
    if (m_pickups.size() >= pixel_hero::config::MAX_PICKUPS)
        return nullptr;

    PickupItem* item = new PickupItem(type, value);
    item->setPos(pos);
    m_scene->addItem(item);
    m_pickups.append(item);
    return item;
}

PickupItem::PickupType PickupManager::randomType(const DropEntry& entry)
{
    int total = entry.xpWeight + entry.goldWeight;
    int roll = QRandomGenerator::global()->bounded(total);
    return (roll < entry.xpWeight) ? PickupItem::XP : PickupItem::GOLD;
}

int PickupManager::randomValue(PickupItem::PickupType type)
{
    switch (type) {
    case PickupItem::XP:
        return 10 + QRandomGenerator::global()->bounded(16);   // 10-25
    case PickupItem::GOLD:
        return 5 + QRandomGenerator::global()->bounded(16);    // 5-20
    default:
        return pixel_hero::config::HP_DROP_RECOVERY;
    }
}

void PickupManager::collectPickup(PickupItem* item, SurvivalPlayer* player)
{
    if (!player) return;

    switch (item->pickupType()) {
    case PickupItem::HP: {
        int newHp = qMin(player->health() + item->value(), player->maxHealth());
        player->setHealth(newHp);
        break;
    }
    case PickupItem::XP:
        player->addExp(item->value());
        break;
    case PickupItem::GOLD:
        player->addGold(item->value());
        break;
    }
    item->markCollected();
}

void PickupManager::magnetizePickups(qreal dt, SurvivalPlayer* player)
{
    if (!player) return;

    QPointF playerPos = player->pos();
    int playerHp = player->health();
    int playerMaxHp = player->maxHealth();

    for (PickupItem* item : m_pickups) {
        if (item->isCollected() || item->isExpired()) continue;

        // 满血跳过血瓶
        if (item->pickupType() == PickupItem::HP && playerHp >= playerMaxHp)
            continue;

        qreal dist = QLineF(playerPos, item->pos()).length();
        if (dist > pixel_hero::config::PICKUP_MAGNET_RANGE) continue;

        if (dist < 4.0f) {
            collectPickup(item, player);
        } else {
            QPointF dir = (playerPos - item->pos()) / dist;
            float speed = pixel_hero::config::PICKUP_MAGNET_SPEED * static_cast<float>(dt);
            item->setPos(item->pos() + dir * speed);
        }
    }
}

void PickupManager::updateLifetimes(qreal dt)
{
    for (int i = m_pickups.size() - 1; i >= 0; i--) {
        PickupItem* item = m_pickups[i];
        item->update(static_cast<float>(dt));

        if (item->isExpired() || item->isCollected()) {
            m_scene->removeItem(item);
            m_pickups.removeAt(i);
            delete item;
        }
    }
}

} // namespace survival
} // namespace pixel_hero
