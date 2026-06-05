#include "CollisionLayer.h"

CollisionLayer::CollisionLayer(QObject *parent)
    : QObject(parent)
{
}

CollisionLayer::~CollisionLayer()
{
    clear();
}

void CollisionLayer::addCollisionRect(const QRectF& rect)
{
    m_collisionRects.append(rect);
}

void CollisionLayer::removeCollisionRect(const QRectF& rect)
{
    m_collisionRects.removeAll(rect);
}

bool CollisionLayer::checkCollision(const QRectF& rect)
{
    for (const QRectF& collisionRect : m_collisionRects) {
        if (rect.intersects(collisionRect)) {
            return true;
        }
    }
    return false;
}

void CollisionLayer::clear()
{
    m_collisionRects.clear();
}