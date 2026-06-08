#ifndef NPC_H
#define NPC_H

#include <QGraphicsPixmapItem>
#include <QString>
#include <QList>
#include <QStringList>

class NPC : public QGraphicsPixmapItem
{
public:
    enum NPCType {
        VILLAGER,
        BLACKSMITH,
        SHOPKEEPER,
        QUEST_GIVER
    };

    NPC(NPCType type, QGraphicsItem *parent = nullptr);
    ~NPC();

    QString name() const;
    NPCType npcType() const;
    QList<QString> dialogues() const;

    void interact();
    QString getCurrentDialogue();

    // 商店功能
    bool hasShop() const;
    QStringList shopItemIds() const;

private:
    NPCType m_type;
    QString m_name;
    QList<QString> m_dialogues;
    int m_currentDialogueIndex;
    QStringList m_shopItemIds;

    void initShopData();
};

#endif // NPC_H