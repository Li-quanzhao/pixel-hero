#ifndef SAVELOADUI_H
#define SAVELOADUI_H

#include <QObject>
#include <QGraphicsItem>
#include <QPainter>
#include <QList>
#include "SelectableListBase.h"
#include "survival/SurvivalSaveData.h"

class SaveLoadUI : public SelectableListBase
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

    // 键盘操作 (保留原方法名兼容 GameWindow eventFilter)
    void prev() { selectPrev(); }
    void next() { selectNext(); }
    void del();

    void appear() override;  // 重写: LOAD模式下禁用空槽
    void dismiss() override;  // 重写: emit cancelled信号

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

signals:
    void saveRequested(int slot);
    void loadRequested(int slot);
    void deleteRequested(int slot);
    void cancelled();

protected:
    QRectF itemRect(int index) const override;
    void onConfirm() override;
    bool handleExtraKey(int key) override;

private:
    Mode  m_mode;
    QList<SlotPreview> m_slots;

    static constexpr int SLOT_X   = 40;
    static constexpr int SLOT_W   = 720;
    static constexpr int SLOT_H   = 100;
    static constexpr int SLOT_GAP = 4;
    static constexpr int START_Y  = 54;

    void drawOneSlot(QPainter* p, int index, const QRectF& r, bool selected);
    QString skillSummary(const SurvivalSaveData& d) const;
};

#endif // SAVELOADUI_H
