#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <QObject>
#include <QString>
#include <QPixmap>
#include <QMap>

class ResourceManager : public QObject
{
    Q_OBJECT

public:
    static ResourceManager* instance();

    QPixmap loadImage(const QString& path);

    void preloadResources();

private:
    ResourceManager(QObject *parent = nullptr);
    ~ResourceManager();

    static ResourceManager* m_instance;
    QMap<QString, QPixmap> m_images;
};

#endif // RESOURCEMANAGER_H