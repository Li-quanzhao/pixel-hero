#include "SelectableListBase.h"

SelectableListBase::SelectableListBase(int itemCount, QGraphicsItem* parent)
    : QObject(nullptr), QGraphicsItem(parent)
    , m_itemCount(itemCount)
    , m_selectedIndex(0)
    , m_visible(false)
    , m_wrapAround(false)
    , m_confirmMode(ClickToSelect)
{
    setZValue(200);
    setFlag(QGraphicsItem::ItemIsFocusable, true);
    setAcceptHoverEvents(true);
}

SelectableListBase::~SelectableListBase() {}

// ---- 生命周期 ----
void SelectableListBase::appear()
{
    m_visible = true;
    m_selectedIndex = 0;
    // 跳过禁用项
    while (m_disabledItems.contains(m_selectedIndex) && m_selectedIndex < m_itemCount - 1)
        m_selectedIndex++;
    show();
    setFocus();
    update();
}

void SelectableListBase::dismiss()
{
    m_visible = false;
    hide();
    clearFocus();
    update();
}

// ---- 选中 ----
void SelectableListBase::setSelectedIndex(int index)
{
    if (index >= 0 && index < m_itemCount && !m_disabledItems.contains(index)) {
        m_selectedIndex = index;
        update();
    }
}

// ---- 导航 ----
void SelectableListBase::selectPrev()
{
    int idx = m_selectedIndex;
    int count = 0;
    do {
        idx--;
        if (m_wrapAround && idx < 0) idx = m_itemCount - 1;
        else if (!m_wrapAround && idx < 0) idx = 0;
        if (idx == m_selectedIndex) break;
        if (++count > m_itemCount) break; // 安全阀
    } while (m_disabledItems.contains(idx));
    m_selectedIndex = idx;
    emit selectionChanged(m_selectedIndex);
    update();
}

void SelectableListBase::selectNext()
{
    int idx = m_selectedIndex;
    int count = 0;
    do {
        idx++;
        if (m_wrapAround && idx >= m_itemCount) idx = 0;
        else if (!m_wrapAround && idx >= m_itemCount) idx = m_itemCount - 1;
        if (idx == m_selectedIndex) break;
        if (++count > m_itemCount) break; // 安全阀
    } while (m_disabledItems.contains(idx));
    m_selectedIndex = idx;
    emit selectionChanged(m_selectedIndex);
    update();
}

// ---- 确认 ----
void SelectableListBase::confirm()
{
    if (m_disabledItems.contains(m_selectedIndex)) return;
    onConfirm();
    emit confirmed(m_selectedIndex);
}

// ---- 禁用 ----
void SelectableListBase::setItemDisabled(int index, bool disabled)
{
    if (disabled)
        m_disabledItems.insert(index);
    else
        m_disabledItems.remove(index);
    update();
}

bool SelectableListBase::isItemDisabled(int index) const
{
    return m_disabledItems.contains(index);
}

void SelectableListBase::setItemCount(int n)
{
    m_itemCount = n;
    if (m_selectedIndex >= n)
        m_selectedIndex = 0;
}

// ---- 键盘 ----
void SelectableListBase::keyPressEvent(QKeyEvent* event)
{
    int key = event->key();

    // 子类额外按键
    if (handleExtraKey(key)) return;

    switch (key) {
    case Qt::Key_W: case Qt::Key_Up:
    case Qt::Key_A: case Qt::Key_Left:
        selectPrev(); break;
    case Qt::Key_S: case Qt::Key_Down:
    case Qt::Key_D: case Qt::Key_Right:
        selectNext(); break;
    case Qt::Key_Return: case Qt::Key_Enter: case Qt::Key_Space:
        confirm(); break;
    default:
        break;
    }
}

bool SelectableListBase::handleExtraKey(int key)
{
    Q_UNUSED(key);
    return false;
}

// ---- 鼠标 ----
void SelectableListBase::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (!m_visible) return;
    for (int i = 0; i < m_itemCount; i++) {
        if (m_disabledItems.contains(i)) continue;
        if (itemRect(i).contains(event->pos())) {
            if (m_confirmMode == ClickToConfirm) {
                m_selectedIndex = i;
                confirm();
            } else {
                if (m_selectedIndex == i)
                    confirm();        // 已选中项，再次点击确认
                else {
                    m_selectedIndex = i;  // 首次点击，仅选中
                    emit selectionChanged(m_selectedIndex);
                    update();
                }
            }
            return;
        }
    }
}

void SelectableListBase::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (!m_visible) return;
    for (int i = 0; i < m_itemCount; i++) {
        if (m_disabledItems.contains(i)) continue;
        if (itemRect(i).contains(event->pos())) {
            if (m_selectedIndex != i) {
                m_selectedIndex = i;
                emit selectionChanged(m_selectedIndex);
                update();
            }
            return;
        }
    }
}

void SelectableListBase::wheelEvent(QGraphicsSceneWheelEvent* event)
{
    if (!m_visible) return;
    if (event->delta() > 0)
        selectPrev();
    else
        selectNext();
}
