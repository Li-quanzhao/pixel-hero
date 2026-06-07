#ifndef NPC_H
#define NPC_H

#include <QGraphicsPixmapItem>
#include <QString>
#include <QList>

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

private:
    NPCType m_type;
    QString m_name;
    QList<QString> m_dialogues;
    int m_currentDialogueIndex;
};

#endif // NPC_H