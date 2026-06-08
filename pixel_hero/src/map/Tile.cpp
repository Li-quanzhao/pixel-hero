#include "Tile.h"

Tile::Tile(TileType type, int x, int y, QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent), m_type(type)
{
    setPos(x * 32, y * 32);
    updateTileProperties();
}

Tile::~Tile()
{
}

Tile::TileType Tile::tileType() const { return m_type; }
bool Tile::isPassable() const { return m_isPassable; }
bool Tile::isCollidable() const { return m_isCollidable; }

void Tile::setType(TileType type)
{
    m_type = type;
    updateTileProperties();
}

void Tile::updateTileProperties()
{
    // 尝试加载对应瓦片图片
    QString imagePath;
    switch (m_type) {
    case FLOOR: imagePath = ":/resources/images/tiles/floor.png"; break;
    case WALL:  imagePath = ":/resources/images/tiles/wall.png";  break;
    case WATER: imagePath = ":/resources/images/tiles/water.png"; break;
    case GRASS: imagePath = ":/resources/images/tiles/grass.png"; break;
    case LAVA:  imagePath = ":/resources/images/tiles/lava.png";  break;
    case DOOR:  imagePath = ":/resources/images/tiles/door.png";  break;
    }

    QPixmap tilePixmap(imagePath);
    if (tilePixmap.isNull()) {
        tilePixmap = QPixmap(32, 32);
    }

    switch (m_type) {
    case FLOOR:
        m_isPassable = true; m_isCollidable = false;
        if (tilePixmap.isNull() || tilePixmap.width() != 32) tilePixmap.fill(QColor(0x4a, 0x55, 0x68));
        break;
    case WALL:
        m_isPassable = false; m_isCollidable = true;
        if (tilePixmap.isNull() || tilePixmap.width() != 32) tilePixmap.fill(QColor(0x2d, 0x37, 0x48));
        break;
    case WATER:
        m_isPassable = false; m_isCollidable = true;
        if (tilePixmap.isNull() || tilePixmap.width() != 32) tilePixmap.fill(QColor(0x31, 0x82, 0xce));
        break;
    case GRASS:
        m_isPassable = true; m_isCollidable = false;
        if (tilePixmap.isNull() || tilePixmap.width() != 32) tilePixmap.fill(QColor(0x38, 0xa1, 0x69));
        break;
    case LAVA:
        m_isPassable = false; m_isCollidable = true;
        if (tilePixmap.isNull() || tilePixmap.width() != 32) tilePixmap.fill(QColor(0xe5, 0x3e, 0x3e));
        break;
    case DOOR:
        m_isPassable = true; m_isCollidable = false;
        if (tilePixmap.isNull() || tilePixmap.width() != 32) tilePixmap.fill(QColor(0x8b, 0x45, 0x13));
        break;
    }
    
    setPixmap(tilePixmap);
}