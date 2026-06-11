#ifndef SELECTABLELISTBASE_H
#define SELECTABLELISTBASE_H

#include <QObject>
#include <QGraphicsItem>
#include <QSet>
#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>

/**
 * 可选列表基类 — 统一管理所有UI组件的交互逻辑
 *
 * 提供: 选中状态、导航(跳过禁用项)、键盘/鼠标/滚轮事件、确认机制
 * 子类只需: 实现 paint() + itemRect() + 业务信号
 */
class SelectableListBase : public QObject, public QGraphicsItem
{
    Q_OBJECT

public:
    enum ConfirmMode {
        ClickToConfirm,   // 单击即确认 (菜单类)
        ClickToSelect     // 单击选中，再次点击确认 (卡片类)
    };

    explicit SelectableListBase(int itemCount, QGraphicsItem* parent = nullptr);
    virtual ~SelectableListBase();

    // ---- 生命周期 ----
    virtual void appear();
    virtual void dismiss();
    bool isVisible() const { return m_visible; }

    // ---- 选中状态 ----
    int  selectedIndex() const { return m_selectedIndex; }
    void setSelectedIndex(int index);

    // ---- 导航 (自动跳过禁用项) ----
    void selectPrev();
    void selectNext();

    // ---- 确认 ----
    virtual void confirm();

    // ---- 禁用管理 ----
    void setItemDisabled(int index, bool disabled);
    bool isItemDisabled(int index) const;

    // ---- QGraphicsItem ----
    QRectF boundingRect() const override { return QRectF(0, 0, 800, 600); }

signals:
    void confirmed(int index);
    void selectionChanged(int index);

protected:
    // ---- 子类可配置 ----
    void setWrapAround(bool wrap)   { m_wrapAround = wrap; }
    void setConfirmMode(ConfirmMode mode) { m_confirmMode = mode; }
    void setItemCount(int n);

    int itemCount() const { return m_itemCount; }

    // ---- 子类必须实现 ----
    virtual QRectF itemRect(int index) const = 0;

    // ---- 子类可重写的钩子 ----
    virtual void onConfirm() {}          // 确认时的业务逻辑
    virtual bool handleExtraKey(int key); // 处理额外的按键，返回 true 表示已处理

    // ---- 事件处理 (子类可重写) ----
    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void wheelEvent(QGraphicsSceneWheelEvent* event) override;

private:
    int         m_itemCount;
    int         m_selectedIndex;
    bool        m_visible;
    bool        m_wrapAround;
    ConfirmMode m_confirmMode;
    QSet<int>   m_disabledItems;
};

#endif // SELECTABLELISTBASE_H
