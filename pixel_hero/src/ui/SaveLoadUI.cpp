#include "SaveLoadUI.h"
#include "utils/GameData.h"
#include "survival/SurvivalPlayer.h"
#include <QPainter>
#include <QGraphicsScene>

// ---- 角色色块颜色 ----
static QColor charColor(const SurvivalSaveData& d) {
    if (d.characterId == "warrior")   return QColor(0xcc, 0x55, 0x44);
    if (d.characterId == "archer")    return QColor(0x44, 0x88, 0xcc);
    if (d.characterId == "mage")      return QColor(0xaa, 0x55, 0xcc);
    return QColor(0x88, 0x88, 0x88);
}

SaveLoadUI::SaveLoadUI(Mode mode, const QList<SlotPreview>& previews,
                       QGraphicsItem* parent)
    : SelectableListBase(5, parent)
    , m_mode(mode), m_slots(previews)
{
    setWrapAround(true);
    setConfirmMode(ClickToSelect);
}

SaveLoadUI::~SaveLoadUI() {}

// ---- 生命周期 (重写: LOAD模式下禁用空槽) ----
void SaveLoadUI::appear()
{
    // LOAD模式: 将空槽标记为禁用
    if (m_mode == LOAD_MODE) {
        for (int i = 0; i < m_slots.size(); ++i) {
            setItemDisabled(i, !m_slots[i].occupied);
        }
    }
    SelectableListBase::appear();
}

void SaveLoadUI::dismiss()
{
    emit cancelled();
    SelectableListBase::dismiss();
}

// ---- 删除 ----
void SaveLoadUI::del()
{
    if (!isVisible() || m_mode != LOAD_MODE) return;
    const auto& sp = m_slots[selectedIndex()];
    if (sp.occupied)
        emit deleteRequested(selectedIndex());
}

// ---- 确认 ----
void SaveLoadUI::onConfirm()
{
    const auto& sp = m_slots[selectedIndex()];
    if (m_mode == SAVE_MODE) {
        emit saveRequested(selectedIndex());
    } else {
        if (sp.occupied)
            emit loadRequested(selectedIndex());
    }
}

// ---- 额外按键 ----
bool SaveLoadUI::handleExtraKey(int key)
{
    if (key == Qt::Key_Delete || key == Qt::Key_Backspace) {
        del();
        return true;
    }
    if (key == Qt::Key_Escape) {
        emit cancelled();
        return true;
    }
    return false;
}

// ---- 几何 ----
QRectF SaveLoadUI::itemRect(int index) const
{
    int y = START_Y + index * (SLOT_H + SLOT_GAP);
    return QRectF(SLOT_X, y, SLOT_W, SLOT_H);
}

QString SaveLoadUI::skillSummary(const SurvivalSaveData& d) const
{
    if (d.skills.isEmpty()) return QStringLiteral("无技能");
    QStringList parts;
    for (const auto& s : d.skills) {
        const SkillData* sd = GameData::instance()->getSkillById(s.first);
        QString name = sd ? sd->name : s.first;
        parts.append(QString("%1 Lv.%2").arg(name).arg(s.second));
    }
    return parts.join("  ");
}

// ==================== 绘制 ====================
void SaveLoadUI::paint(QPainter* painter, const QStyleOptionGraphicsItem*,
                       QWidget*)
{
    if (!isVisible()) return;

    // 背景遮罩
    painter->setBrush(QColor(0x12, 0x12, 0x24, 220));
    painter->drawRect(0, 0, 800, 600);

    // 标题
    QFont titleFont("Arial", 18, QFont::Bold);
    painter->setFont(titleFont);
    painter->setPen(QColor(0xff, 0xcc, 0x00));
    QString title = (m_mode == SAVE_MODE)
                    ? QStringLiteral("— 保存游戏 —") : QStringLiteral("— 读取存档 —");
    painter->drawText(QRectF(0, 6, 800, 30), Qt::AlignCenter, title);

    // 提示
    QFont hintFont("Arial", 8);
    painter->setFont(hintFont);
    painter->setPen(QColor(0x66, 0x66, 0x88));
    QString hint = (m_mode == SAVE_MODE)
        ? QStringLiteral("↑↓ / 点击选择   Enter 保存   Esc 返回")
        : QStringLiteral("↑↓ / 点击选择   Enter 读取   Del 删除   Esc 返回");
    painter->drawText(QRectF(0, 30, 800, 16), Qt::AlignCenter, hint);

    // 绘制5个槽
    for (int i = 0; i < m_slots.size(); ++i) {
        drawOneSlot(painter, i, itemRect(i), i == selectedIndex());
    }
}

void SaveLoadUI::drawOneSlot(QPainter* p, int index, const QRectF& r, bool selected)
{
    const auto& sp = m_slots[index];
    bool disabled = isItemDisabled(index);
    QColor bgColor = selected ? QColor(0x2a, 0x30, 0x48)
                   : disabled   ? QColor(0x12, 0x14, 0x1e)
                   : QColor(0x1a, 0x1e, 0x2e);
    QColor borderColor = selected ? QColor(0xff, 0xcc, 0x00)
                       : disabled ? QColor(0x30, 0x34, 0x40)
                       : QColor(0x40, 0x48, 0x58);

    // === 卡片背景 ===
    p->setPen(QPen(borderColor, selected ? 2 : 1));
    p->setBrush(bgColor);
    p->drawRoundedRect(r, 6, 6);

    double rx = r.left() + 10;
    double ry = r.top() + 8;

    // === 空槽 ===
    if (!sp.occupied) {
        QFont emptyFont("Arial", 12);
        p->setFont(emptyFont);
        p->setPen(disabled ? QColor(0x40, 0x40, 0x50) : QColor(0x55, 0x55, 0x66));
        p->drawText(QRectF(rx + 32, ry, r.width() - 44, r.height() - 16),
                    Qt::AlignCenter,
                    m_mode == SAVE_MODE ? QStringLiteral("— 点击此处保存 —")
                                        : QStringLiteral("— 空存档槽 —"));
        return;
    }

    const auto& d = sp.data;

    // === 角色名 ===
    QString charName = d.characterId;
    auto chars = SurvivalPlayer::availableCharacters();
    for (const auto& c : chars) {
        if (c.id == d.characterId) { charName = c.name; break; }
    }

    // === 武器名 ===
    QString wpnName = d.weaponId;
    const WeaponData* wpn = GameData::instance()->getWeaponById(d.weaponId);
    if (wpn) wpnName = wpn->name;

    // === 计算时间 ===
    int totalSec = static_cast<int>(d.elapsedTime);
    int minutes = totalSec / 60;
    int seconds = totalSec % 60;
    QString timeStr = QString("%1:%2")
        .arg(minutes)
        .arg(seconds, 2, 10, QChar('0'));

    // === 左侧角色色块 ===
    QRectF avatarRect(rx, ry + (r.height() - 50) / 2, 50, 50);
    p->setPen(Qt::NoPen);
    p->setBrush(charColor(d));
    p->drawRoundedRect(avatarRect, 8, 8);

    // 色块上的角色名首字
    QFont avatarFont("Arial", 14, QFont::Bold);
    p->setFont(avatarFont);
    p->setPen(Qt::white);
    p->drawText(avatarRect, Qt::AlignCenter, charName.left(2));

    // === 信息区 (色块右侧) ===
    double ix = rx + 62;
    double iw = r.right() - ix - 10;

    // 第1行: 槽号 + 角色/武器 + Lv
    {
        QFont l1Font("Arial", 10, QFont::Bold);
        p->setFont(l1Font);
        p->setPen(QColor(0x88, 0x88, 0x99));
        p->drawText(QRectF(ix, ry, 32, 18), Qt::AlignLeft, QString("#%1").arg(index + 1));

        p->setPen(Qt::white);
        p->drawText(QRectF(ix + 32, ry, 180, 18), Qt::AlignLeft,
                    QString("%1 / %2").arg(charName, wpnName));

        QFont lvFont("Arial", 9);
        p->setFont(lvFont);
        p->setPen(QColor(0xff, 0xcc, 0x00));
        p->drawText(QRectF(r.right() - 80, ry, 70, 18), Qt::AlignRight,
                    QString("Lv.%1").arg(d.playerLevel));
    }

    // 第2行: HP条
    double hpx = ix + 32;
    double hpy = ry + 20;
    double hpw = 180;
    double hph = 8;
    double hpRatio = qMin(1.0, (double)d.playerHealth / d.playerMaxHealth);

    p->setPen(Qt::NoPen);
    p->setBrush(QColor(0x33, 0x22, 0x22));
    p->drawRoundedRect(QRectF(hpx, hpy, hpw, hph), 3, 3);

    QColor hpColor = hpRatio > 0.5 ? QColor(0x44, 0xcc, 0x44)
                   : hpRatio > 0.25 ? QColor(0xcc, 0xaa, 0x22)
                   : QColor(0xcc, 0x44, 0x33);
    p->setBrush(hpColor);
    p->drawRoundedRect(QRectF(hpx, hpy, hpw * hpRatio, hph), 3, 3);

    QFont hpFont("Arial", 7);
    p->setFont(hpFont);
    p->setPen(QColor(0xaa, 0xaa, 0xbb));
    p->drawText(QRectF(hpx + hpw + 6, hpy, 80, hph), Qt::AlignVCenter | Qt::AlignLeft,
                QString("HP %1/%2").arg(d.playerHealth).arg(d.playerMaxHealth));

    // 第3行: 波次 + 击杀 + 时长
    {
        QFont sFont("Arial", 8);
        p->setFont(sFont);
        p->setPen(QColor(0x99, 0x99, 0xbb));
        p->drawText(QRectF(ix, ry + 32, iw, 14), Qt::AlignLeft,
                    QString("波次: %1    击杀: %2    时长: %3")
                        .arg(d.currentWave)
                        .arg(d.totalKills)
                        .arg(timeStr));
    }

    // 第4行: 技能
    {
        QFont sFont("Arial", 8);
        p->setFont(sFont);
        p->setPen(QColor(0x77, 0x77, 0x99));
        QString skills = skillSummary(d);
        QFontMetrics fm(sFont);
        if (fm.horizontalAdvance(skills) > iw) {
            skills = fm.elidedText(skills, Qt::ElideRight, static_cast<int>(iw));
        }
        p->drawText(QRectF(ix, ry + 48, iw, 14), Qt::AlignLeft, skills);
    }

    // 第5行: 保存时间
    if (d.savedAt.isValid()) {
        QFont tsFont("Arial", 7);
        p->setFont(tsFont);
        p->setPen(QColor(0x55, 0x55, 0x77));
        p->drawText(QRectF(ix, ry + 64, iw, 12), Qt::AlignLeft,
                    d.savedAt.toString("yyyy-MM-dd hh:mm:ss"));
    }
}
