#ifndef SAVELOADUI_H
#define SAVELOADUI_H

#include <QObject>
#include <QGraphicsItem>
#include <QPainter>
#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>
#include <QWheelEvent>
#include <QList>
#include "SurvivalSaveData.h"

class SaveLoadUI : public QObject, public QGraphicsItem
{
    Q_OBJECT

public:
    enum Mode { SAVE_MODE, LOAD_MODE };

    struct SlotPreview {
        bool   occupied;
        SurvivalSaveData data;
    };

    SaveLoadUI(Mode mode, const QList<SlotPreview>& previews,
               QGraphicsItem* parent = nullptr);
    ~SaveLoadUI();

    void appear();
    void dismiss();

    void prev();
    void next();
    void confirm();
    void del();

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

signals:
    void saveRequested(int slot);
    void loadRequested(int slot);
    void deleteRequested(int slot);
    void cancelled();

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void wheelEvent(QGraphicsSceneWheelEvent* event) override;

private:
    Mode  m_mode;
    QList<SlotPreview> m_slots;
    bool  m_visible;
    int   m_selectedIndex;

    static constexpr int SLOT_X   = 40;
    static constexpr int SLOT_W   = 720;
    static constexpr int SLOT_H   = 100;
    static constexpr int SLOT_GAP = 4;
    static constexpr int START_Y  = 54;

    QRectF slotRect(int index) const;
    int slotAtPos(const QPointF& pos) const;
    void drawOneSlot(QPainter* p, int index, const QRectF& r, bool selected);
    QString skillSummary(const SurvivalSaveData& d) const;
};

#endif // SAVELOADUI_H
