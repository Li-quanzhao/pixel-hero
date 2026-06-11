#ifndef PICKUPMANAGER_H
#define PICKUPMANAGER_H

#include <QObject>
#include <QList>
#include <QPointF>
#include <QGraphicsScene>
#include "PickupItem.h"

class SurvivalPlayer;

namespace pixel_hero {
namespace survival {

class PickupManager : public QObject
{
    Q_OBJECT
public:
    PickupManager(QGraphicsScene* scene, QObject* parent = nullptr);
    ~PickupManager();

    void spawnPickups(QPointF pos, int enemyType, SurvivalPlayer* player);
    void update(qreal dt, SurvivalPlayer* player);
    void clear();

    int pickupCount() const { return m_pickups.size(); }

private:
    QGraphicsScene* m_scene;
    QList<PickupItem*> m_pickups;

    struct DropEntry {
        int minCount;
        int maxCount;
        int xpWeight;
        int goldWeight;
    };
    static const DropEntry& dropEntry(int enemyType);

    PickupItem* createPickup(QPointF pos, PickupItem::PickupType type, int value);
    PickupItem::PickupType randomType(const DropEntry& entry);
    int randomValue(PickupItem::PickupType type);
    void collectPickup(PickupItem* item, SurvivalPlayer* player);
    void magnetizePickups(qreal dt, SurvivalPlayer* player);
    void updateLifetimes(qreal dt);
};

} // namespace survival
} // namespace pixel_hero

#endif // PICKUPMANAGER_H
