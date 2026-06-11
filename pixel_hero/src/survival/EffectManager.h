#ifndef EFFECTMANAGER_H
#define EFFECTMANAGER_H

#include <QObject>
#include <QPointF>
#include <QGraphicsScene>
#include <QPointer>

namespace pixel_hero {
namespace survival {

class EffectManager : public QObject
{
    Q_OBJECT
public:
    explicit EffectManager(QGraphicsScene* scene, QObject* parent = nullptr);

    void showSlash(QPointF pos);
    void showFireball(QPointF from, QPointF to);
    void showLightning(QPointF from, QPointF to);
    void showFrostNova(QPointF center, float radius);
    void showFrostProjectile(QPointF from, QPointF to);
    void showFrostExplosion(QPointF center, float radius);

private:
    QPointer<QGraphicsScene> m_scene;
};

} // namespace survival
} // namespace pixel_hero

#endif // EFFECTMANAGER_H
