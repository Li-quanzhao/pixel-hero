#include "NPC.h"

NPC::NPC(NPCType type, QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent), m_type(type), m_currentDialogueIndex(0)
{
    QPixmap npcPixmap(40, 40);
    
    switch (type) {
    case VILLAGER:
        m_name = QString("村民");
        m_dialogues << QString("你好啊，冒险者！") << QString("最近村里不太太平...") << QString("小心森林里的怪物。");
        npcPixmap.fill(QColor(0xf5, 0x9e, 0x0b));
        break;
    case BLACKSMITH:
        m_name = QString("铁匠");
        m_dialogues << QString("欢迎来到我的铁匠铺！") << QString("需要打造什么武器吗？") << QString("我这里有最好的装备！");
        npcPixmap.fill(QColor(0x6b, 0x72, 0x80));
        break;
    case SHOPKEEPER:
        m_name = QString("商人");
        m_dialogues << QString("欢迎光临！") << QString("看看有什么需要的？") << QString("价格公道，童叟无欺！");
        npcPixmap.fill(QColor(0x8b, 0x5c, 0xbf));
        break;
    case QUEST_GIVER:
        m_name = QString("任务使者");
        m_dialogues << QString("我有一个任务给你...") << QString("去消灭森林里的哥布林！") << QString("完成后会有丰厚的奖励！");
        npcPixmap.fill(QColor(0x06, 0x5f, 0x46));
        break;
    }
    
    setPixmap(npcPixmap);

    setOffset(-boundingRect().width() / 2, -boundingRect().height());
}

NPC::~NPC()
{
}

QString NPC::name() const { return m_name; }
NPC::NPCType NPC::npcType() const { return m_type; }
QList<QString> NPC::dialogues() const { return m_dialogues; }

void NPC::interact()
{
    m_currentDialogueIndex = (m_currentDialogueIndex + 1) % m_dialogues.size();
}

QString NPC::getCurrentDialogue()
{
    if (m_currentDialogueIndex < m_dialogues.size()) {
        return m_dialogues[m_currentDialogueIndex];
    }
    return QString();
}