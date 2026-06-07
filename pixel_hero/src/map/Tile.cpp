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
    QPixmap tilePixmap(32, 32);
    
    switch (m_type) {
    case FLOOR:
        m_isPassable = true;
        m_isCollidable = false;
        tilePixmap.fill(QColor(0x4a, 0x55, 0x68));
        break;
    case WALL:
        m_isPassable = false;
        m_isCollidable = true;
        tilePixmap.fill(QColor(0x2d, 0x37, 0x48));
        break;
    case WATER:
        m_isPassable = false;
        m_isCollidable = true;
        tilePixmap.fill(QColor(0x31, 0x82, 0xce));
        break;
    case GRASS:
        m_isPassable = true;
        m_isCollidable = false;
        tilePixmap.fill(QColor(0x38, 0xa1, 0x69));
        break;
    case LAVA:
        m_isPassable = false;
        m_isCollidable = true;
        tilePixmap.fill(QColor(0xe5, 0x3e, 0x3e));
        break;
    case DOOR:
        m_isPassable = true;
        m_isCollidable = false;
        tilePixmap.fill(QColor(0x8b, 0x45, 0x13));
        break;
    }
    
    setPixmap(tilePixmap);
}