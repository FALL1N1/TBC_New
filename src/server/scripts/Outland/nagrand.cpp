#include "ScriptPCH.h"
#include "EscortAI.h"

class mob_shattered_rumbler : public CreatureScript
{
public:
    mob_shattered_rumbler() : CreatureScript("mob_shattered_rumbler") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new mob_shattered_rumblerAI(pCreature);
    }

    struct mob_shattered_rumblerAI : public ScriptedAI
    {
        bool Spawn;

        mob_shattered_rumblerAI(Creature* pCreature) : ScriptedAI(pCreature) { }

        void Reset()
        {
            Spawn = false;
        }

        void EnterCombat(Unit* /*pWho*/) { }

        void SpellHit(Unit* pHitter, const SpellInfo* pSpell)
        {
            if(pSpell->Id == 32001 && !Spawn)
            {
                float x = me->GetPositionX();
                float y = me->GetPositionY();
                float z = me->GetPositionZ();

                pHitter->SummonCreature(18181, x+(0.7f * (rand()%30)), y+(rand()%5), z, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000);
                pHitter->SummonCreature(18181, x+(rand()%5), y-(rand()%5), z, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000);
                pHitter->SummonCreature(18181, x-(rand()%5), y+(0.5f *(rand()%60)), z, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000);
                me->SetDeathState(CORPSE);
                Spawn = true;
            }
            return;
        }
    };
};

/*######
## mob_lump
######*/

#define SPELL_VISUAL_SLEEP  16093
#define SPELL_SPEAR_THROW   32248

#define LUMP_SAY0 -1000190
#define LUMP_SAY1 -1000191

#define LUMP_DEFEAT -1000192

#define GOSSIP_HL "I need answers, ogre!"
#define GOSSIP_SL1 "Why are Boulderfist out this far? You know that this is Kurenai territory."
#define GOSSIP_SL2 "And you think you can just eat anything you want? You're obviously trying to eat the Broken of Telaar."
#define GOSSIP_SL3 "This means war, Lump! War I say!"

class mob_lump : public CreatureScript
{
public:
    mob_lump() : CreatureScript("mob_lump") { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
    {
        ClearGossipMenuFor(player);
        switch(uiAction)
        {
            case GOSSIP_ACTION_INFO_DEF:
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_SL1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                SendGossipMenuFor(player, 9353, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+1:
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_SL2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                SendGossipMenuFor(player, 9354, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_SL3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                SendGossipMenuFor(player, 9355, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+3:
                SendGossipMenuFor(player, 9356, creature->GetGUID());
                player->TalkedToCreature(18354, creature->GetGUID());
                break;
        }
        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if(player->GetQuestStatus(9918) == QUEST_STATUS_INCOMPLETE)
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_HL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

        SendGossipMenuFor(player, 9352, creature->GetGUID());

        return true;
    }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new mob_lumpAI(pCreature);
    }

    struct mob_lumpAI : public ScriptedAI
    {
        mob_lumpAI(Creature* pCreature) : ScriptedAI(pCreature)
        {
            bReset = false;
        }

        uint32 Reset_Timer;
        uint32 Spear_Throw_Timer;
        bool bReset;

        void Reset()
        {
            Reset_Timer = 60000;
            Spear_Throw_Timer = 2000;

            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }

        void AttackedBy(Unit* pAttacker)
        {
            if(me->GetVictim())
                return;

            if(me->IsFriendlyTo(pAttacker))
                return;

            AttackStart(pAttacker);
        }

        void DamageTaken(Unit* done_by, uint32 & damage)
        {
            if(done_by->GetTypeId() == TYPEID_PLAYER && me->HealthBelowPctDamaged(30, damage))
                if(Player* plr = done_by->ToPlayer())
            {
                if(!bReset && plr->GetQuestStatus(9918) == QUEST_STATUS_INCOMPLETE)
                {
                    //Take 0 damage
                    damage = 0;

                    plr->AttackStop();
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    me->RemoveAllAuras();
                    me->GetThreatManager().ResetAllThreat();
                    me->CombatStop(true);
                    me->SetFaction(1080);               //friendly
                    me->SetStandState(UNIT_STAND_STATE_SIT);
                    DoScriptText(LUMP_DEFEAT, me);

                    bReset = true;
                }
            }
        }

        void EnterCombat(Unit* /*pWho*/)
        {
            if(me->HasAura(SPELL_VISUAL_SLEEP))
                me->RemoveAura(SPELL_VISUAL_SLEEP);

            if(!me->IsStandState())
                 me->SetStandState(UNIT_STAND_STATE_STAND);

            DoScriptText(RAND(LUMP_SAY0, LUMP_SAY1), me);
        }

        void UpdateAI(const uint32 diff)
        {
            //check if we waiting for a reset
            if(bReset)
            {
                if(Reset_Timer <= diff)
                {
                    EnterEvadeMode();
                    bReset = false;
                    me->SetFaction(1711);               //hostile
                    return;
                }
                else Reset_Timer -= diff;
            }

            //Return since we have no target
            if(!UpdateVictim())
                return;

            //Spear_Throw_Timer
            if(Spear_Throw_Timer <= diff)
            {
                DoCast(me->GetVictim(), SPELL_SPEAR_THROW);
                Spear_Throw_Timer = 20000;
            } else Spear_Throw_Timer -= diff;

            DoMeleeAttackIfReady();
        }
    };
};

/*####
# mob_sunspring_villager - should be done with ACID
####*/

class mob_sunspring_villager : public CreatureScript
{
public:
    mob_sunspring_villager() : CreatureScript("mob_sunspring_villager") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new mob_sunspring_villagerAI(pCreature);
    }

    struct mob_sunspring_villagerAI : public ScriptedAI
    {
        mob_sunspring_villagerAI(Creature* pCreature) : ScriptedAI(pCreature) { }

        void Reset()
        {
            me->SetUInt32Value(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
            me->SetStandState(UNIT_STAND_STATE_DEAD);
        }

        void EnterCombat(Unit* /*pWho*/) { }

        void SpellHit(Unit* /*pCaster*/, const SpellInfo* spell)
        {
            if(spell->Id == 32146)
            {
                // me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                me->RemoveCorpse();
            }
        }
    };
};

/*######
## npc_altruis_the_sufferer
######*/

#define GOSSIP_HATS1 "I see twisted steel and smell sundered earth."
#define GOSSIP_HATS2 "Well...?"
#define GOSSIP_HATS3 "[PH] Story about Illidan's Pupil"

#define GOSSIP_SATS1 "Legion?"
#define GOSSIP_SATS2 "And now?"
#define GOSSIP_SATS3 "How do you see them now?"
#define GOSSIP_SATS4 "Forge camps?"
#define GOSSIP_SATS5 "Ok."
#define GOSSIP_SATS6 "[PH] Story done"

class npc_altruis_the_sufferer : public CreatureScript
{
public:
    npc_altruis_the_sufferer() : CreatureScript("npc_altruis_the_sufferer") { }

    bool OnQuestAccept(Player* player, Creature* /*pCreature*/, Quest const* /*quest*/)
    {
        if(!player->GetQuestRewardStatus(9991))              //Survey the Land, q-id 9991
        {
            CloseGossipMenuFor(player);
            player->ActivateTaxiPathTo(532);                  //TaxiPath 532
        }
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
    {
        ClearGossipMenuFor(player);
        switch(uiAction)
        {
            case GOSSIP_ACTION_INFO_DEF+10:
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_SATS1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
                SendGossipMenuFor(player, 9420, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+11:
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_SATS2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 12);
                SendGossipMenuFor(player, 9421, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+12:
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_SATS3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 13);
                SendGossipMenuFor(player, 9422, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+13:
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_SATS4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 14);
                SendGossipMenuFor(player, 9423, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+14:
                SendGossipMenuFor(player, 9424, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+20:
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_SATS5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 21);
                SendGossipMenuFor(player, 9427, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+21:
                CloseGossipMenuFor(player);
                player->AreaExploredOrEventHappens(9991);
                break;
            case GOSSIP_ACTION_INFO_DEF+30:
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_SATS6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 31);
                SendGossipMenuFor(player, 384, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+31:
                CloseGossipMenuFor(player);
                player->AreaExploredOrEventHappens(10646);
                break;
        }
        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if(creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        //gossip before obtaining Survey the Land
        if(player->GetQuestStatus(9991) == QUEST_STATUS_NONE)
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_HATS1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+10);

        //gossip when Survey the Land is incomplete (technically, after the flight)
        if(player->GetQuestStatus(9991) == QUEST_STATUS_INCOMPLETE)
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_HATS2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+20);

        //wowwiki.com/Varedis
        if(player->GetQuestStatus(10646) == QUEST_STATUS_INCOMPLETE)
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_HATS3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+30);

        SendGossipMenuFor(player, 9419, creature->GetGUID());

        return true;
    }
};

/*######
## npc_greatmother_geyah
######*/

#define GOSSIP_HGG1 "Hello, Greatmother. Garrosh told me that you wanted to speak with me."
#define GOSSIP_HGG2 "Garrosh is beyond redemption, Greatmother. I fear that in helping the Mag'har, I have convinced Garrosh that he is unfit to lead."

#define GOSSIP_SGG1 "You raised all of the orcs here, Greatmother?"
#define GOSSIP_SGG2 "Do you believe that?"
#define GOSSIP_SGG3 "What can be done? I have tried many different things. I have done my best to help the people of Nagrand. Each time I have approached Garrosh, he has dismissed me."
#define GOSSIP_SGG4 "Left? How can you choose to leave?"
#define GOSSIP_SGG5 "What is this duty?"
#define GOSSIP_SGG6 "Is there anything I can do for you, Greatmother?"
#define GOSSIP_SGG7 "I have done all that I could, Greatmother. I thank you for your kind words."
#define GOSSIP_SGG8 "Greatmother, you are the mother of Durotan?"
#define GOSSIP_SGG9 "Greatmother, I never had the honor. Durotan died long before my time, but his heroics are known to all on my world. The orcs of Azeroth reside in a place known as Durotar, named after your son. And ... (You take a moment to breathe and think through what you are about to tell the Greatmother.)"
#define GOSSIP_SGG10 "It is my Warchief, Greatmother. The leader of my people. From my world. He ... He is the son of Durotan. He is your grandchild."
#define GOSSIP_SGG11 "I will return to Azeroth at once, Greatmother."

//all the textId's for the below is unknown, but i do believe the gossip item texts are proper.
class npc_greatmother_geyah : public CreatureScript
{
public:
    npc_greatmother_geyah() : CreatureScript("npc_greatmother_geyah") { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
    {
        ClearGossipMenuFor(player);
        switch(uiAction)
        {
            case GOSSIP_ACTION_INFO_DEF + 1:
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_SGG1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                SendGossipMenuFor(player, player->GetGossipTextId(creature), creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 2:
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_SGG2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                SendGossipMenuFor(player, player->GetGossipTextId(creature), creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 3:
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_SGG3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
                SendGossipMenuFor(player, player->GetGossipTextId(creature), creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 4:
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_SGG4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
                SendGossipMenuFor(player, player->GetGossipTextId(creature), creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 5:
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_SGG5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
                SendGossipMenuFor(player, player->GetGossipTextId(creature), creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 6:
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_SGG6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
                SendGossipMenuFor(player, player->GetGossipTextId(creature), creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 7:
                player->AreaExploredOrEventHappens(10044);
                CloseGossipMenuFor(player);
                break;
            case GOSSIP_ACTION_INFO_DEF + 10:
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_SGG7, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
                SendGossipMenuFor(player, player->GetGossipTextId(creature), creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 11:
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_SGG8, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 12);
                SendGossipMenuFor(player, player->GetGossipTextId(creature), creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 12:
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_SGG9, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 13);
                SendGossipMenuFor(player, player->GetGossipTextId(creature), creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 13:
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_SGG10, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 14);
                SendGossipMenuFor(player, player->GetGossipTextId(creature), creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 14:
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_SGG11, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 15);
                SendGossipMenuFor(player, player->GetGossipTextId(creature), creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 15:
                player->AreaExploredOrEventHappens(10172);
                CloseGossipMenuFor(player);
                break;
        }
        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if(creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if(player->GetQuestStatus(10044) == QUEST_STATUS_INCOMPLETE)
        {
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_HGG1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            SendGossipMenuFor(player, player->GetGossipTextId(creature), creature->GetGUID());
        }
        else if(player->GetQuestStatus(10172) == QUEST_STATUS_INCOMPLETE)
        {
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_HGG2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);
            SendGossipMenuFor(player, player->GetGossipTextId(creature), creature->GetGUID());
        }
        else

            SendGossipMenuFor(player, player->GetGossipTextId(creature), creature->GetGUID());

        return true;
    }
};

/*######
## npc_lantresor_of_the_blade
######*/

#define GOSSIP_HLB "I have killed many of your ogres, Lantresor. I have no fear."
#define GOSSIP_SLB1 "Should I know? You look like an orc to me."
#define GOSSIP_SLB2 "And the other half?"
#define GOSSIP_SLB3 "I have heard of your kind, but I never thought to see the day when I would meet a half-breed."
#define GOSSIP_SLB4 "My apologies. I did not mean to offend. I am here on behalf of my people."
#define GOSSIP_SLB5 "My people ask that you pull back your Boulderfist ogres and cease all attacks on our territories. In return, we will also pull back our forces."
#define GOSSIP_SLB6 "We will fight you until the end, then, Lantresor. We will not stand idly by as you pillage our towns and kill our people."
#define GOSSIP_SLB7 "What do I need to do?"

class npc_lantresor_of_the_blade : public CreatureScript
{
public:
    npc_lantresor_of_the_blade() : CreatureScript("npc_lantresor_of_the_blade") { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
    {
        ClearGossipMenuFor(player);
        switch(uiAction)
        {
            case GOSSIP_ACTION_INFO_DEF:
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_SLB1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                SendGossipMenuFor(player, 9362, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+1:
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_SLB2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                SendGossipMenuFor(player, 9363, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_SLB3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                SendGossipMenuFor(player, 9364, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+3:
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_SLB4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
                SendGossipMenuFor(player, 9365, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+4:
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_SLB5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
                SendGossipMenuFor(player, 9366, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+5:
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_SLB6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
                SendGossipMenuFor(player, 9367, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+6:
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_SLB7, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
                SendGossipMenuFor(player, 9368, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+7:
                SendGossipMenuFor(player, 9369, creature->GetGUID());
                if(player->GetQuestStatus(10107) == QUEST_STATUS_INCOMPLETE)
                    player->AreaExploredOrEventHappens(10107);
                if(player->GetQuestStatus(10108) == QUEST_STATUS_INCOMPLETE)
                    player->AreaExploredOrEventHappens(10108);
                break;
        }
        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if(creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if(player->GetQuestStatus(10107) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(10108) == QUEST_STATUS_INCOMPLETE)
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_HLB, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

        SendGossipMenuFor(player, 9361, creature->GetGUID());

        return true;
    }
};

/*#####
## npc_maghar_captive
#####*/

enum eMagharCaptive
{
    SAY_MAG_START               = -1000482,
    SAY_MAG_NO_ESCAPE           = -1000483,
    SAY_MAG_MORE                = -1000484,
    SAY_MAG_MORE_REPLY          = -1000485,
    SAY_MAG_LIGHTNING           = -1000486,
    SAY_MAG_SHOCK               = -1000487,
    SAY_MAG_COMPLETE            = -1000488,

    SPELL_CHAIN_LIGHTNING       = 16006,
    SPELL_EARTHBIND_TOTEM       = 15786,
    SPELL_FROST_SHOCK           = 12548,
    SPELL_HEALING_WAVE          = 12491,

    QUEST_TOTEM_KARDASH_H       = 9868,

    NPC_MURK_RAIDER             = 18203,
    NPC_MURK_BRUTE              = 18211,
    NPC_MURK_SCAVENGER          = 18207,
    NPC_MURK_PUTRIFIER          = 18202
};

static float m_afAmbushA[]= {-1568.805786f, 8533.873047f, 1.958f};
static float m_afAmbushB[]= {-1491.554321f, 8506.483398f, 1.248f};

class npc_maghar_captive : public CreatureScript
{
public:
    npc_maghar_captive() : CreatureScript("npc_maghar_captive") { }

    bool OnQuestAccept(Player* player, Creature* creature, const Quest* pQuest)
    {
        if(pQuest->GetQuestId() == QUEST_TOTEM_KARDASH_H)
        {
            if(npc_maghar_captiveAI* pEscortAI = dynamic_cast<npc_maghar_captiveAI*>(creature->AI()))
            {
                creature->SetStandState(UNIT_STAND_STATE_STAND);
                creature->SetFaction(232);

                pEscortAI->Start(true, false, player->GetGUID(), pQuest);

                DoScriptText(SAY_MAG_START, creature);

                creature->SummonCreature(NPC_MURK_RAIDER, m_afAmbushA[0]+2.5f, m_afAmbushA[1]-2.5f, m_afAmbushA[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
                creature->SummonCreature(NPC_MURK_PUTRIFIER, m_afAmbushA[0]-2.5f, m_afAmbushA[1]+2.5f, m_afAmbushA[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
                creature->SummonCreature(NPC_MURK_BRUTE, m_afAmbushA[0], m_afAmbushA[1], m_afAmbushA[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
            }
        }
        return true;
    }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_maghar_captiveAI(pCreature);
    }

    struct npc_maghar_captiveAI : public EscortAI
    {
        npc_maghar_captiveAI(Creature* pCreature) : EscortAI(pCreature) { Reset(); }

        uint32 m_uiChainLightningTimer;
        uint32 m_uiHealTimer;
        uint32 m_uiFrostShockTimer;

        void Reset()
        {
            m_uiChainLightningTimer = 1000;
            m_uiHealTimer = 0;
            m_uiFrostShockTimer = 6000;
        }

        void EnterCombat(Unit* /*pWho*/)
        {
            DoCast(me, SPELL_EARTHBIND_TOTEM, false);
        }

        void WaypointReached(uint32 uiPointId, uint32)
        {
            switch(uiPointId)
            {
                case 7:
                    DoScriptText(SAY_MAG_MORE, me);

                    if(Creature* pTemp = me->SummonCreature(NPC_MURK_PUTRIFIER, m_afAmbushB[0], m_afAmbushB[1], m_afAmbushB[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000))
                        DoScriptText(SAY_MAG_MORE_REPLY, pTemp);

                    me->SummonCreature(NPC_MURK_PUTRIFIER, m_afAmbushB[0]-2.5f, m_afAmbushB[1]-2.5f, m_afAmbushB[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);

                    me->SummonCreature(NPC_MURK_SCAVENGER, m_afAmbushB[0]+2.5f, m_afAmbushB[1]+2.5f, m_afAmbushB[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
                    me->SummonCreature(NPC_MURK_SCAVENGER, m_afAmbushB[0]+2.5f, m_afAmbushB[1]-2.5f, m_afAmbushB[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
                    break;
                case 16:
                    DoScriptText(SAY_MAG_COMPLETE, me);

                    if(Player* player = GetPlayerForEscort())
                        player->GroupEventHappens(QUEST_TOTEM_KARDASH_H, me);

                    SetRun();
                    break;
            }
        }

        void JustSummoned(Creature* summoned)
        {
            if(summoned->GetEntry() == NPC_MURK_BRUTE)
                DoScriptText(SAY_MAG_NO_ESCAPE, summoned);

            if(summoned->IsTotem())
                return;

            summoned->RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING);
            summoned->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
            summoned->AI()->AttackStart(me);

        }

        void SpellHitTarget(Unit* /*pTarget*/, const SpellInfo* pSpell)
        {
            if(pSpell->Id == SPELL_CHAIN_LIGHTNING)
            {
                if(rand()%10)
                    return;

                DoScriptText(SAY_MAG_LIGHTNING, me);
            }
        }

        void UpdateAI(const uint32 diff)
        {
            EscortAI::UpdateAI(diff);
            if(!me->GetVictim())
                return;

            if(m_uiChainLightningTimer <= diff)
            {
                DoCast(me->GetVictim(), SPELL_CHAIN_LIGHTNING);
                m_uiChainLightningTimer = urand(7000, 14000);
            }
            else
                m_uiChainLightningTimer -= diff;

            if(HealthBelowPct(30))
            {
                if(m_uiHealTimer <= diff)
                {
                    DoCast(me, SPELL_HEALING_WAVE);
                    m_uiHealTimer = 5000;
                }
                else
                    m_uiHealTimer -= diff;
            }

            if(m_uiFrostShockTimer <= diff)
            {
                DoCast(me->GetVictim(), SPELL_FROST_SHOCK);
                m_uiFrostShockTimer = urand(7500, 15000);
            }
            else
                m_uiFrostShockTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };
};

/*######
## npc_creditmarker_visist_with_ancestors
######*/

class npc_creditmarker_visit_with_ancestors : public CreatureScript
{
public:
    npc_creditmarker_visit_with_ancestors() : CreatureScript("npc_creditmarker_visit_with_ancestors") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_creditmarker_visit_with_ancestorsAI(pCreature);
    }

    struct npc_creditmarker_visit_with_ancestorsAI : public ScriptedAI
    {
        npc_creditmarker_visit_with_ancestorsAI(Creature* pCreature) : ScriptedAI(pCreature) { }

        void Reset() { }

        void EnterCombat(Unit* /*pWho*/) { }

        void MoveInLineOfSight(Unit* who)
        {
            if(!who)
                return;

            if(who->GetTypeId() == TYPEID_PLAYER)
                if(Player* plr = who->ToPlayer())
            {
                if(plr->GetQuestStatus(10085) == QUEST_STATUS_INCOMPLETE)
                {
                    uint32 creditMarkerId = me->GetEntry();
                    if((creditMarkerId >= 18840) && (creditMarkerId <= 18843))
                    {
                        // 18840: Sunspring, 18841: Laughing, 18842: Garadar, 18843: Bleeding
                        if(!plr->GetReqKillOrCastCurrentCount(10085, creditMarkerId))
                            plr->KilledMonsterCredit(creditMarkerId, me->GetGUID());
                    }
                }
            }
        }
    };
};

/*######
## mob_sparrowhawk
######*/

#define SPELL_SPARROWHAWK_NET 39810
#define SPELL_ITEM_CAPTIVE_SPARROWHAWK 39812

class mob_sparrowhawk : public CreatureScript
{
public:
    mob_sparrowhawk() : CreatureScript("mob_sparrowhawk") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new mob_sparrowhawkAI(pCreature);
    }

    struct mob_sparrowhawkAI : public ScriptedAI
    {
        mob_sparrowhawkAI(Creature* pCreature) : ScriptedAI(pCreature) { }

        uint32 Check_Timer;
        ObjectGuid PlayerGUID;
        bool fleeing;

        void Reset()
        {
            me->RemoveAurasDueToSpell(SPELL_SPARROWHAWK_NET);
            Check_Timer = 1000;
            PlayerGUID.Clear();
            fleeing = false;
        }
        void AttackStart(Unit* who)
        {
            ScriptedAI::AttackStart(who);
        }

        void EnterCombat(Unit* /*pWho*/) { }

        void MoveInLineOfSight(Unit* who)
        {
            if(!who || PlayerGUID)
                return;
            if(Player* plr = who->ToPlayer())
            if(!PlayerGUID && who->GetTypeId() == TYPEID_PLAYER && me->IsWithinDistInMap(who, 30) && plr->GetQuestStatus(10987) == QUEST_STATUS_INCOMPLETE)
            {
                PlayerGUID.Set(who->GetGUID());
                return;
            }

            ScriptedAI::MoveInLineOfSight(who);
        }

        void UpdateAI(const uint32 diff)
        {
            if(Check_Timer <= diff)
            {
                if(PlayerGUID)
                {
                    if(fleeing && me->GetMotionMaster()->GetCurrentMovementGeneratorType() != FLEEING_MOTION_TYPE)
                        fleeing = false;

                    Player* player = ObjectAccessor::GetPlayer(*me, PlayerGUID);
                    if(player && me->IsWithinDistInMap(player, 30))
                    {
                        if(!fleeing)
                        {
                            me->GetThreatManager().ResetAllThreat();
                            me->GetMotionMaster()->MoveFleeing(player);
                            fleeing = true;
                        }
                    }
                    else if(fleeing)
                    {
                        // me->GetMotionMaster()->MovementExpired(false);
                        PlayerGUID.Clear();
                        fleeing = false;
                    }
                }
                Check_Timer = 1000;
            } else Check_Timer -= diff;

            ScriptedAI::UpdateAI(diff);
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if(caster->GetTypeId() == TYPEID_PLAYER)
                if(Player* plr = caster->ToPlayer())
            {
                if(spell->Id == SPELL_SPARROWHAWK_NET && plr->GetQuestStatus(10987) == QUEST_STATUS_INCOMPLETE)
                {
                    DoCast(caster, SPELL_ITEM_CAPTIVE_SPARROWHAWK, true);
                    // @todo me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                    me->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
                }
            }
            return;
        }
    };
};

enum CorkiData
{
    // first quest
    QUEST_HELP                                    = 9923,
    NPC_CORKI                                     = 18445,
    NPC_CORKI_CREDIT_1                            = 18369,
    GO_CORKIS_PRISON                              = 182349,
    CORKI_SAY_THANKS                              = -1800071,
    // 2nd quest
    QUEST_CORKIS_GONE_MISSING_AGAIN               = 9924,
    NPC_CORKI_2                                   = 20812,
    GO_CORKIS_PRISON_2                            = 182350,
    CORKI_SAY_PROMISE                             = -1800072,
    // 3rd quest
    QUEST_CHOWAR_THE_PILLAGER                     = 9955,
    NPC_CORKI_3                                   = 18369,
    NPC_CORKI_CREDIT_3                            = 18444,
    GO_CORKIS_PRISON_3                            = 182521,
    CORKI_SAY_LAST                                = -1800073
};

class go_corkis_prison : public GameObjectScript
{
public:
    go_corkis_prison() : GameObjectScript("go_corkis_prison") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if(go->GetEntry() == GO_CORKIS_PRISON)
        {
            if(Creature* corki = go->FindNearestCreature(NPC_CORKI, 25, true))
            {
                go->SetGoState(GO_STATE_READY);
                corki->GetMotionMaster()->MovePoint(1, go->GetPositionX()+5, go->GetPositionY(), go->GetPositionZ());
                if(player)
                    player->KilledMonsterCredit(NPC_CORKI_CREDIT_1);
            }
        }
        if(go->GetEntry() == GO_CORKIS_PRISON_2)
        {
            if(Creature* corki = go->FindNearestCreature(NPC_CORKI_2, 25, true))
            {
                go->SetGoState(GO_STATE_READY);
                corki->GetMotionMaster()->MovePoint(1, go->GetPositionX()-5, go->GetPositionY(), go->GetPositionZ());
                if(player)
                    player->KilledMonsterCredit(NPC_CORKI_2);
            }
        }
        if(go->GetEntry() == GO_CORKIS_PRISON_3)
        {
            if(Creature* corki = go->FindNearestCreature(NPC_CORKI, 25, true))
            {
                go->SetGoState(GO_STATE_READY);
                corki->GetMotionMaster()->MovePoint(1, go->GetPositionX()+4, go->GetPositionY(), go->GetPositionZ());
                if(player)
                    player->KilledMonsterCredit(NPC_CORKI_CREDIT_3);
            }
        }
        return true;
    }
};

class npc_corki : public CreatureScript
{
public:
  npc_corki() : CreatureScript("npc_corki") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_corkiAI(pCreature);
    }

    struct npc_corkiAI : public ScriptedAI
    {
        npc_corkiAI(Creature* pCreature): ScriptedAI(pCreature) { }

        uint32 Say_Timer;
        bool ReleasedFromCage;

        void Reset()
        {
            ReleasedFromCage = false;
        }

        void UpdateAI(uint32 const diff)
        {
            if(Say_Timer <= diff && ReleasedFromCage)
            {
                me->ForcedDespawn();
                ReleasedFromCage = false;
            } else Say_Timer -= diff;
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if(type == POINT_MOTION_TYPE && id == 1)
            {
                Say_Timer = 5000;
                ReleasedFromCage = true;
                switch(me->GetEntry())
                {
                    case NPC_CORKI:
                        DoScriptText(CORKI_SAY_THANKS, me);
                        break;
                    case NPC_CORKI_2:
                        DoScriptText(CORKI_SAY_PROMISE, me);
                        break;
                    case NPC_CORKI_3:
                        DoScriptText(CORKI_SAY_LAST, me);
                        break;
                    default:
                        break;
                }
            }
        };
    };
};

void AddSC_nagrand()
{
    new mob_shattered_rumbler;
    new mob_lump;
    new mob_sunspring_villager;
    new npc_altruis_the_sufferer;
    new npc_greatmother_geyah;
    new npc_lantresor_of_the_blade;
    new npc_maghar_captive;
    new npc_creditmarker_visit_with_ancestors;
    new mob_sparrowhawk;
    new npc_corki;
    new go_corkis_prison;
}
