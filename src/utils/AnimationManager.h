#ifndef ANIMATIONMANAGER_H
#define ANIMATIONMANAGER_H

#include <QObject>
#include <QString>
#include <QPixmap>
#include <QMap>

class AnimationManager : public QObject
{
    Q_OBJECT

public:
    static AnimationManager* instance();

    void loadAnimation(const QString& name, const QString& path, int frameCount, int frameWidth, int frameHeight);
    QPixmap getFrame(const QString& name, int frameIndex);
    int getFrameCount(const QString& name);

private:
    AnimationManager(QObject *parent = nullptr);
    ~AnimationManager();

    static AnimationManager* m_instance;
    QMap<QString, QList<QPixmap>> m_animations;
};

#endif // ANIMATIONMANAGER_H