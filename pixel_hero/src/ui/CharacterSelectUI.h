#ifndef CHARACTERSELECTUI_H
#define CHARACTERSELECTUI_H

#include <QObject>
#include <QGraphicsItem>
#include <QString>
#include <QList>
#include <QColor>
#include "SelectableListBase.h"

class CharacterSelectUI : public SelectableListBase
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

    // 键盘操作 (保留为兼容方法，GameWindow eventFilter 调用)
    void prev()    { selectPrev(); }
    void next()    { selectNext(); }

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

signals:
    void characterSelected(int index);

protected:
    QRectF itemRect(int index) const override;
    void onConfirm() override;

private:
    QList<CharacterInfo> m_characters;

    static constexpr int CARD_W   = 160;
    static constexpr int CARD_H   = 260;
    static constexpr int CARD_GAP = 24;
    static constexpr int INFO_Y   = 370;

    void drawStatBar(QPainter* p, qreal x, qreal y, int w,
                     const QString& label, int val, int maxVal,
                     const QColor& barColor) const;
};

#endif // CHARACTERSELECTUI_H
