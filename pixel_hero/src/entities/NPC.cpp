#include "NPC.h"

NPC::NPC(NPCType type, QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent), m_type(type), m_currentDialogueIndex(0)
{
    // 尝试加载NPC图片
    QString imagePath;
    switch (type) {
    case VILLAGER:    imagePath = ":/resources/images/entities/villager.png"; break;
    case BLACKSMITH:  imagePath = ":/resources/images/entities/blacksmith.png"; break;
    case SHOPKEEPER:  imagePath = ":/resources/images/entities/shopkeeper.png"; break;
    case QUEST_GIVER: imagePath = ":/resources/images/entities/quest_giver.png"; break;
    }

    QPixmap npcPixmap = QPixmap(imagePath);
    if (npcPixmap.isNull()) {
        npcPixmap = QPixmap(40, 40);
    }
    
    switch (type) {
    case VILLAGER:
        m_name = QString("村民");
        m_dialogues << QString("你好啊，冒险者！") << QString("最近村里不太太平...") << QString("小心森林里的怪物。");
        if (npcPixmap.isNull() || npcPixmap.width() != 40) npcPixmap.fill(QColor(0xf5, 0x9e, 0x0b));
        break;
    case BLACKSMITH:
        m_name = QString("铁匠");
        m_dialogues << QString("欢迎来到我的铁匠铺！") << QString("需要打造什么武器吗？") << QString("我这里有最好的装备！");
        if (npcPixmap.isNull() || npcPixmap.width() != 40) npcPixmap.fill(QColor(0x6b, 0x72, 0x80));
        break;
    case SHOPKEEPER:
        m_name = QString("商人");
        m_dialogues << QString("欢迎光临！") << QString("看看有什么需要的？") << QString("价格公道，童叟无欺！");
        if (npcPixmap.isNull() || npcPixmap.width() != 40) npcPixmap.fill(QColor(0x8b, 0x5c, 0xbf));
        break;
    case QUEST_GIVER:
        m_name = QString("任务使者");
        m_dialogues << QString("我有一个任务给你...") << QString("去消灭森林里的哥布林！") << QString("完成后会有丰厚的奖励！");
        if (npcPixmap.isNull() || npcPixmap.width() != 40) npcPixmap.fill(QColor(0x06, 0x5f, 0x46));
        break;
    }
    
    setPixmap(npcPixmap);

    setOffset(-boundingRect().width() / 2, -boundingRect().height());
    initShopData();
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

void NPC::initShopData()
{
    switch (m_type) {
    case BLACKSMITH:
        m_shopItemIds << "sword_002" << "sword_003" << "armor_002" << "armor_003";
        break;
    case SHOPKEEPER:
        m_shopItemIds << "potion_hp_small" << "potion_hp" << "potion_hp_large"
                      << "potion_mp" << "scroll_fireball" << "scroll_lightning"
                      << "key_copper" << "key_silver" << "key_gold";
        break;
    default:
        break;
    }
}

bool NPC::hasShop() const
{
    return !m_shopItemIds.isEmpty();
}

QStringList NPC::shopItemIds() const
{
    return m_shopItemIds;
}