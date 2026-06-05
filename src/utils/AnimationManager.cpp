#include "AnimationManager.h"

AnimationManager* AnimationManager::m_instance = nullptr;

AnimationManager::AnimationManager(QObject *parent)
    : QObject(parent)
{
}

AnimationManager::~AnimationManager()
{
}

AnimationManager* AnimationManager::instance()
{
    if (!m_instance) {
        m_instance = new AnimationManager();
    }
    return m_instance;
}

void AnimationManager::loadAnimation(const QString& name, const QString& path, int frameCount, int frameWidth, int frameHeight)
{
    QPixmap spriteSheet(path);
    QList<QPixmap> frames;

    for (int i = 0; i < frameCount; ++i) {
        QPixmap frame = spriteSheet.copy(i * frameWidth, 0, frameWidth, frameHeight);
        frames.append(frame);
    }

    m_animations[name] = frames;
}

QPixmap AnimationManager::getFrame(const QString& name, int frameIndex)
{
    if (m_animations.contains(name)) {
        QList<QPixmap> frames = m_animations[name];
        if (frameIndex >= 0 && frameIndex < frames.size()) {
            return frames[frameIndex];
        }
    }
    return QPixmap();
}

int AnimationManager::getFrameCount(const QString& name)
{
    if (m_animations.contains(name)) {
        return m_animations[name].size();
    }
    return 0;
}