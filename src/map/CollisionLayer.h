#ifndef COLLISIONLAYER_H
#define COLLISIONLAYER_H

#include <QObject>
#include <QRectF>
#include <QList>

class CollisionLayer : public QObject
{
    Q_OBJECT

public:
    CollisionLayer(QObject *parent = nullptr);
    ~CollisionLayer();

    void addCollisionRect(const QRectF& rect);
    void removeCollisionRect(const QRectF& rect);
    bool checkCollision(const QRectF& rect);
    void clear();

private:
    QList<QRectF> m_collisionRects;
};

#endif // COLLISIONLAYER_H