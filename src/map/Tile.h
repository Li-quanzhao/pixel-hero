#ifndef TILE_H
#define TILE_H

#include <QGraphicsPixmapItem>

class Tile : public QGraphicsPixmapItem
{
public:
    enum TileType {
        FLOOR,
        WALL,
        WATER,
        GRASS,
        LAVA,
        DOOR
    };

    Tile(TileType type, int x, int y, QGraphicsItem *parent = nullptr);
    ~Tile();

    TileType tileType() const;
    bool isPassable() const;
    bool isCollidable() const;

    void setType(TileType type);

private:
    TileType m_type;
    bool m_isPassable;
    bool m_isCollidable;

    void updateTileProperties();
};

#endif // TILE_H