#include "EffectManager.h"
#include "config/GameConfig.h"
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsEllipseItem>
#include <QGraphicsPathItem>
#include <QPen>
#include <QBrush>
#include <QLineF>
#include <QRandomGenerator>
#include <QTimer>
#include <algorithm>

namespace pixel_hero {
namespace survival {

EffectManager::EffectManager(QGraphicsScene* scene, QObject* parent)
    : QObject(parent), m_scene(scene)
{
}

static void removeLater(QGraphicsScene* scene, QGraphicsItem* item, int ms, QObject* parent)
{
    QTimer::singleShot(ms, parent, [scene, item]() {
        if (scene && item) { scene->removeItem(item); delete item; }
    });
}

void EffectManager::showSlash(QPointF pos)
{
    if (!m_scene) return;
    auto* slash = m_scene->addEllipse(-8, -8, 16, 16,
        QPen(QColor(255, 255, 200, 200), 2),
        QBrush(QColor(255, 255, 255, 80)));
    slash->setPos(pos);
    removeLater(m_scene, slash, 150, this);
}

void EffectManager::showFireball(QPointF from, QPointF to)
{
    if (!m_scene) return;
    QLineF line(from, to);
    int steps = 5;
    for (int i = 1; i <= steps; ++i) {
        QPointF p = line.pointAt(i / qreal(steps));
        auto* orb = m_scene->addEllipse(-3, -3, 6, 6, Qt::NoPen,
            QBrush(QColor(255, 136 + i * 20, 0, 180)));
        orb->setPos(p);
        removeLater(m_scene, orb, 100 + i * 40, this);
    }
    auto* hit = m_scene->addEllipse(-6, -6, 12, 12, Qt::NoPen,
        QBrush(QColor(255, 200, 0, 200)));
    hit->setPos(to);
    removeLater(m_scene, hit, 200, this);
}

void EffectManager::showLightning(QPointF from, QPointF to)
{
    if (!m_scene) return;
    QPainterPath path;
    path.moveTo(from);
    QLineF line(from, to);
    qreal len = line.length();
    int segs = qMax(2, int(len / 20));
    for (int i = 1; i <= segs; ++i) {
        qreal t = i / qreal(segs);
        QPointF p = line.pointAt(t);
        qreal jitterX = (i % 2 == 0 ? 6 : -6);
        qreal jitterY = (QRandomGenerator::global()->bounded(8) - 4);
        path.lineTo(p + QPointF(jitterX, jitterY));
    }
    auto* bolt = m_scene->addPath(path,
        QPen(QColor(100, 180, 255, 220), 2), QBrush(Qt::NoBrush));
    auto* glow = m_scene->addPath(path,
        QPen(QColor(150, 210, 255, 100), 4), QBrush(Qt::NoBrush));
    removeLater(m_scene, bolt, 250, this);
    removeLater(m_scene, glow, 250, this);
}

void EffectManager::showFrostNova(QPointF center, float radius)
{
    if (!m_scene) return;
    auto* ring1 = m_scene->addEllipse(center.x() - 5, center.y() - 5, 10, 10,
        QPen(QColor(100, 200, 255, 200), 3), QBrush(Qt::NoBrush));
    auto* ring2 = m_scene->addEllipse(center.x() - radius, center.y() - radius,
        radius * 2, radius * 2,
        QPen(QColor(80, 180, 240, 120), 2), QBrush(Qt::NoBrush));
    removeLater(m_scene, ring1, 350, this);
    removeLater(m_scene, ring2, 350, this);
}

} // namespace survival
} // namespace pixel_hero
