#ifndef UPGRADEUI_H
#define UPGRADEUI_H

#include <QObject>
#include <QGraphicsItem>
#include <QRectF>
#include <QList>
#include "SelectableListBase.h"
#include "utils/GameData.h"

class UpgradeUI : public SelectableListBase
{
    Q_OBJECT

public:
    UpgradeUI(QGraphicsItem* parent = nullptr);
    ~UpgradeUI();

    void showUpgrade(const QList<SkillData>& options);
    void hide();

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

signals:
    void skillSelected(const QString& skillId);
    void skillSkipped();  // Esc跳过/取消升级

protected:
    QRectF itemRect(int index) const override;
    void onConfirm() override;
    bool handleExtraKey(int key) override;

private:
    QList<SkillData> m_options;

    static constexpr int CARD_WIDTH  = 180;
    static constexpr int CARD_HEIGHT = 200;
    static constexpr int CARD_GAP    = 20;
    static constexpr int START_X     = 110;
    static constexpr int START_Y     = 160;
};

#endif // UPGRADEUI_H
