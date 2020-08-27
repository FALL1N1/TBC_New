#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "Player.h"

/*######
## npc_ragged_john
######*/

#define GOSSIP_HELLO    "Official buisness, John. I need some information about Marsha Windsor. Tell me about the last time you saw him."
#define GOSSIP_SELECT1  "So what did you do?"
#define GOSSIP_SELECT2  "Start making sense, dwarf. I don't want to have anything to do with your cracker, your pappy, or any sort of 'discreditin'."
#define GOSSIP_SELECT3  "Ironfoe?"
#define GOSSIP_SELECT4  "Interesting... continue John."
#define GOSSIP_SELECT5  "So that's how Windsor died..."
#define GOSSIP_SELECT6  "So how did he die?"
#define GOSSIP_SELECT7  "Ok so where the hell is he? Wait a minute! Are you drunk?"
#define GOSSIP_SELECT8  "WHY is he in Blackrock Depths?"
#define GOSSIP_SELECT9  "300? So the Dark Irons killed him and dragged him into the Depths?"
#define GOSSIP_SELECT10 "Ahh... Ironfoe"
#define GOSSIP_SELECT11 "Thanks, Ragged John. Your story was very uplifting and informative"

enum RaggedJohn
{
    QUEST_THE_TRUE_MASTERS        = 4224,
    QUEST_MOTHERS_MILK            = 4866,
    SPELL_MOTHERS_MILK            = 16468,
    SPELL_WICKED_MILKING          = 16472
};

class npc_ragged_john : public CreatureScript
{
public:
    npc_ragged_john() : CreatureScript("npc_ragged_john") { }

    struct npc_ragged_johnAI : public ScriptedAI
    {
        npc_ragged_johnAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset() { }

        void MoveInLineOfSight(Unit* who)
        {
            if (who->HasAura(SPELL_MOTHERS_MILK))
            {
                if (who->GetTypeId() == TYPEID_PLAYER && me->IsWithinDistInMap(who, 15) && who->isInAccessiblePlaceFor(me))
                {
                    DoCast(who, SPELL_WICKED_MILKING);
                    if (Player* player = who->ToPlayer())
                        player->AreaExploredOrEventHappens(QUEST_MOTHERS_MILK);
                }
            }

            ScriptedAI::MoveInLineOfSight(who);
        }

        void EnterCombat(Unit* /*who*/) { }
    };

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF:
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_SELECT1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                SendGossipMenuFor(player, 2714, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+1:
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_SELECT2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                SendGossipMenuFor(player, 2715, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_SELECT3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                SendGossipMenuFor(player, 2716, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+3:
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_SELECT4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
                SendGossipMenuFor(player, 2717, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+4:
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_SELECT5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
                SendGossipMenuFor(player, 2718, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+5:
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_SELECT6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
                SendGossipMenuFor(player, 2719, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+6:
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_SELECT7, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
                SendGossipMenuFor(player, 2720, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+7:
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_SELECT8, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);
                SendGossipMenuFor(player, 2721, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+8:
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_SELECT9, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9);
                SendGossipMenuFor(player, 2722, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+9:
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_SELECT10, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);
                SendGossipMenuFor(player, 2723, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+10:
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_SELECT11, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
                SendGossipMenuFor(player, 2725, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+11:
                CloseGossipMenuFor(player);
                player->AreaExploredOrEventHappens(QUEST_THE_TRUE_MASTERS);
                break;
        }
        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_THE_TRUE_MASTERS) == QUEST_STATUS_INCOMPLETE)
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_HELLO, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

        SendGossipMenuFor(player, 2713, creature->GetGUID());
        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ragged_johnAI(creature);
    }
};

void AddSC_burning_steppes()
{
    new npc_ragged_john();
}
