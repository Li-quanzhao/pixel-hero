#include "ResourceManager.h"

ResourceManager* ResourceManager::m_instance = nullptr;

ResourceManager::ResourceManager(QObject *parent)
    : QObject(parent)
{
}

ResourceManager::~ResourceManager()
{
}

ResourceManager* ResourceManager::instance()
{
    if (!m_instance) {
        m_instance = new ResourceManager();
    }
    return m_instance;
}

QPixmap ResourceManager::loadImage(const QString& path)
{
    if (m_images.contains(path)) {
        return m_images[path];
    }

    QPixmap pixmap(path);
    if (!pixmap.isNull()) {
        m_images[path] = pixmap;
    }

    return pixmap;
}

void ResourceManager::preloadResources()
{
}