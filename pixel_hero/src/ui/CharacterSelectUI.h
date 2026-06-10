#ifndef CHARACTERSELECTUI_H
#define CHARACTERSELECTUI_H

#include <QObject>
#include <QGraphicsItem>
#include <QString>
#include <QList>
#include <QColor>
#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>

class CharacterSelectUI : public QObject, public QGraphicsItem
{
    Q_OBJECT

public:
    struct CharacterInfo {
        QString id;
        QString name;
        int     hp;
        int     atk;
        int     def;
        int     spd;
        QColor  color;
        QString desc;
    };

    CharacterSelectUI(const QList<CharacterInfo>& characters,
                      QGraphicsItem* parent = nullptr);
    ~CharacterSelectUI();

    void appear();
    void dismiss();

    void prev();
    void next();
    void confirm();
    int  selectedIndex() const { return m_selectedIndex; }

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

signals:
    void characterSelected(int index);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void wheelEvent(QGraphicsSceneWheelEvent* event) override;

private:
    QList<CharacterInfo> m_characters;
    bool  m_visible;
    int   m_selectedIndex;

    static constexpr int CARD_W   = 160;
    static constexpr int CARD_H   = 260;
    static constexpr int CARD_GAP = 24;
    static constexpr int INFO_Y   = 370;

    QRectF cardRect(int index) const;
    void   drawStatBar(QPainter* p, qreal x, qreal y, int w,
                       const QString& label, int val, int maxVal,
                       const QColor& barColor) const;
};

#endif // CHARACTERSELECTUI_H
