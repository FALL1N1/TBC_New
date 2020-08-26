#include "ScriptPCH.h"
#include "EscortAI.h"

enum eAeranas
{
    SAY_SUMMON              = -1000138,
    SAY_FREE                = -1000139,

    FACTION_HOSTILE         = 16, 

    SPELL_ENVELOPING_WINDS  = 15535,
    SPELL_SHOCK             = 12553,

    C_AERANAS               = 17085
};

class npc_aeranas : public CreatureScript
{
public:
    npc_aeranas() : CreatureScript("npc_aeranas") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_aeranasAI(pCreature);
    }

    struct npc_aeranasAI : public ScriptedAI
    {
        npc_aeranasAI(Creature* pCreature) : ScriptedAI(pCreature) { }

        uint32 Faction_Timer;
        uint32 EnvelopingWinds_Timer;
        uint32 Shock_Timer;

        void Reset()
        {
            Faction_Timer = 8000;
            EnvelopingWinds_Timer = 9000;
            Shock_Timer = 5000;

            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
            me->SetFaction(FACTION_FRIENDLY);

            DoScriptText(SAY_SUMMON, me);
        }

        void UpdateAI(const uint32 diff)
        {
            if(Faction_Timer)
            {
                if(Faction_Timer <= diff)
                {
                    me->SetFaction(FACTION_HOSTILE);
                    Faction_Timer = 0;
                } else Faction_Timer -= diff;
            }

            if(!UpdateVictim())
                return;

            if(HealthBelowPct(30))
            {
                me->SetFaction(FACTION_FRIENDLY);
                me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                me->RemoveAllAuras(); 
                me->GetThreatManager().ResetAllThreat();
                me->CombatStop(true);
                DoScriptText(SAY_FREE, me);
                return;
            }

            if(Shock_Timer <= diff)
            {
                DoCast(me->GetVictim(), SPELL_SHOCK);
                Shock_Timer = 10000;
            } else Shock_Timer -= diff;

            if(EnvelopingWinds_Timer <= diff)
            {
                DoCast(me->GetVictim(), SPELL_ENVELOPING_WINDS);
                EnvelopingWinds_Timer = 25000;
            } else EnvelopingWinds_Timer -= diff;

            DoMeleeAttackIfReady();
        }
    };
};

enum eAncestralWolf
{
    EMOTE_WOLF_LIFT_HEAD            = -1000496,
    EMOTE_WOLF_HOWL                 = -1000497,
    SAY_WOLF_WELCOME                = -1000498,

    SPELL_ANCESTRAL_WOLF_BUFF       = 29981,

    NPC_RYGA                        = 17123
};

class npc_ancestral_wolf : public CreatureScript
{
public:
    npc_ancestral_wolf() : CreatureScript("npc_ancestral_wolf") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_ancestral_wolfAI(pCreature);
    }

    struct npc_ancestral_wolfAI : public EscortAI
    {
        npc_ancestral_wolfAI(Creature* pCreature) : EscortAI(pCreature)
        {
            if (pCreature->GetOwner() && pCreature->GetOwner()->GetTypeId() == TYPEID_PLAYER)
                Start(false, false, pCreature->GetOwner()->GetGUID());
            else
                ;// sLog->outError("TRINITY: npc_ancestral_wolf can not obtain owner or owner is not a player.");

            pCreature->SetSpeedRate(MOVE_WALK, 1.5f);
            Reset();
        }

        Unit* pRyga;

        void Reset()
        {
            pRyga = NULL;
            DoCast(me, SPELL_ANCESTRAL_WOLF_BUFF, true);
        }

        void MoveInLineOfSight(Unit* who)
        {
            if(!pRyga && who->GetTypeId() == TYPEID_UNIT && who->GetEntry() == NPC_RYGA && me->IsWithinDistInMap(who, 15.0f))
                pRyga = who;

            EscortAI::MoveInLineOfSight(who);
        }

        void WaypointReached(uint32 uiPointId, uint32)
        {
            switch(uiPointId)
            {
                case 0:
                    DoScriptText(EMOTE_WOLF_LIFT_HEAD, me);
                    break;
                case 2:
                    DoScriptText(EMOTE_WOLF_HOWL, me);
                    break;
                case 50:
                    if(pRyga && pRyga->IsAlive() && !pRyga->IsInCombat())
                        DoScriptText(SAY_WOLF_WELCOME, pRyga);
                    break;
            }
        }
    };
};

class go_haaleshi_altar : public GameObjectScript
{
public:
    go_haaleshi_altar() : GameObjectScript("go_haaleshi_altar") { }

    bool OnGossipHello(Player* /*pPlayer*/, GameObject* pGo)
    {
        pGo->SummonCreature(C_AERANAS, -1321.79f, 4043.80f, 116.24f, 1.25f, TEMPSUMMON_TIMED_DESPAWN, 180000);
        return false;
    }
};

#define GOSSIP_NALADU_ITEM1 "Why don't you escape?"

enum eNaladu
{
    GOSSIP_TEXTID_NALADU1   = 9788
};

class npc_naladu : public CreatureScript
{
public:
    npc_naladu() : CreatureScript("npc_naladu") { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
    {
        ClearGossipMenuFor(player); 
        if(uiAction == GOSSIP_ACTION_INFO_DEF+1)
            SendGossipMenuFor(player, GOSSIP_TEXTID_NALADU1, creature->GetGUID());

        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if(creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_NALADU_ITEM1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        SendGossipMenuFor(player, player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }
};

#define GOSSIP_TEXT_REDEEM_MARKS        "I have marks to redeem!"
#define GOSSIP_TRACY_PROUDWELL_ITEM1    "I heard that your dog Fei Fei took Klatu's prayer beads..."
#define GOSSIP_TRACY_PROUDWELL_ITEM2    "<back>"

enum eTracy
{
    GOSSIP_TEXTID_TRACY_PROUDWELL1       = 10689,
    QUEST_DIGGING_FOR_PRAYER_BEADS       = 10916
};

class npc_tracy_proudwell : public CreatureScript
{
public:
    npc_tracy_proudwell() : CreatureScript("npc_tracy_proudwell") { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
    {
        ClearGossipMenuFor(player);
        switch(uiAction)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_TRACY_PROUDWELL_ITEM2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                SendGossipMenuFor(player, GOSSIP_TEXTID_TRACY_PROUDWELL1, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                SendGossipMenuFor(player, player->GetGossipTextId(creature), creature->GetGUID());
                break;
            case GOSSIP_ACTION_TRADE:
                player->GetSession()->SendListInventory(creature->GetGUID());
                break;
        }

        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if(creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if(creature->IsVendor())
            AddGossipItemFor(player, GOSSIP_ICON_VENDOR, GOSSIP_TEXT_REDEEM_MARKS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

        if(player->GetQuestStatus(QUEST_DIGGING_FOR_PRAYER_BEADS) == QUEST_STATUS_INCOMPLETE)
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_TRACY_PROUDWELL_ITEM1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        SendGossipMenuFor(player, player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }
};

#define GOSSIP_TROLLBANE_ITEM1      "Tell me of the Sons of Lothar."
#define GOSSIP_TROLLBANE_ITEM2      "<more>"
#define GOSSIP_TROLLBANE_ITEM3      "Tell me of your homeland."

enum eTrollbane
{
    GOSSIP_TEXTID_TROLLBANE1        = 9932,
    GOSSIP_TEXTID_TROLLBANE2        = 9933,
    GOSSIP_TEXTID_TROLLBANE3        = 8772
};

class npc_trollbane : public CreatureScript
{
public:
    npc_trollbane() : CreatureScript("npc_trollbane") { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
    {
        ClearGossipMenuFor(player);
        switch(uiAction)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_TROLLBANE_ITEM2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                SendGossipMenuFor(player, GOSSIP_TEXTID_TROLLBANE1, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                SendGossipMenuFor(player, GOSSIP_TEXTID_TROLLBANE2, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+3:
                SendGossipMenuFor(player, GOSSIP_TEXTID_TROLLBANE3, creature->GetGUID());
                break;
        }

        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if(creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_TROLLBANE_ITEM1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_TROLLBANE_ITEM3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
        SendGossipMenuFor(player, player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }
};

enum eWoundedBloodElf
{
    SAY_ELF_START               = -1000117,
    SAY_ELF_SUMMON1             = -1000118,
    SAY_ELF_RESTING             = -1000119,
    SAY_ELF_SUMMON2             = -1000120,
    SAY_ELF_COMPLETE            = -1000121,
    SAY_ELF_AGGRO               = -1000122,

    QUEST_ROAD_TO_FALCON_WATCH  = 9375
};

class npc_wounded_blood_elf : public CreatureScript
{
public:
    npc_wounded_blood_elf() : CreatureScript("npc_wounded_blood_elf") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if(quest->GetQuestId() == QUEST_ROAD_TO_FALCON_WATCH)
        {
            if(EscortAI* pEscortAI = CAST_AI(npc_wounded_blood_elf::npc_wounded_blood_elfAI, creature->AI()))
                pEscortAI->Start(true, false, player->GetGUID());

            // Change faction so mobs attack
            creature->SetFaction(775);
        }

        return true;
    }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_wounded_blood_elfAI(pCreature);
    }

    struct npc_wounded_blood_elfAI : public EscortAI
    {
        npc_wounded_blood_elfAI(Creature* pCreature) : EscortAI(pCreature) { }

        void WaypointReached(uint32 i, uint32)
        {
            Player* player = GetPlayerForEscort();

            if(!player)
                return;

            switch(i)
            {
            case 0:
                DoScriptText(SAY_ELF_START, me, player);
                break;
            case 9:
                DoScriptText(SAY_ELF_SUMMON1, me, player);
                // Spawn two Haal'eshi Talonguard
                DoSpawnCreature(16967, -15, -15, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                DoSpawnCreature(16967, -17, -17, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                break;
            case 13:
                DoScriptText(SAY_ELF_RESTING, me, player);
                break;
            case 14:
                DoScriptText(SAY_ELF_SUMMON2, me, player);
                // Spawn two Haal'eshi Windwalker
                DoSpawnCreature(16966, -15, -15, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                DoSpawnCreature(16966, -17, -17, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                break;
            case 27:
                DoScriptText(SAY_ELF_COMPLETE, me, player);
                // Award quest credit
                player->GroupEventHappens(QUEST_ROAD_TO_FALCON_WATCH, me);
                break;
            }
        }

        void Reset() { }

        void EnterCombat(Unit* /*pWho*/)
        {
            if(HasEscortState(STATE_ESCORT_ESCORTING))
                DoScriptText(SAY_ELF_AGGRO, me);
        }

        void JustSummoned(Creature* summoned)
        {
            summoned->AI()->AttackStart(me);
        }
    };
};

enum eFelGuard
{
    SPELL_SUMMON_POO                              = 37688,

    NPC_DERANGED_HELBOAR                          = 16863
};

class npc_fel_guard_hound : public CreatureScript
{
public:
    npc_fel_guard_hound() : CreatureScript("npc_fel_guard_hound") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_fel_guard_houndAI(pCreature);
    }

    struct npc_fel_guard_houndAI : public ScriptedAI
    {
        npc_fel_guard_houndAI(Creature* pCreature): ScriptedAI(pCreature) { }

        uint32 uiCheckTimer;
        ObjectGuid uiHelboarGUID;

        void Reset()
        {
            uiCheckTimer = 5000; //check for creature every 5 sec
            uiHelboarGUID.Clear();
        }

        void MovementInform(uint32 uiType, uint32 uiId)
        {
            if(uiType != POINT_MOTION_TYPE || uiId != 1)
                return;

            if(Creature* pHelboar = ObjectAccessor::GetCreature(*me, uiHelboarGUID))
            {
                pHelboar->RemoveCorpse();
                DoCast(SPELL_SUMMON_POO);

                if(Player* owner = me->GetCharmerOrOwnerPlayerOrPlayerItself())
                    me->GetMotionMaster()->MoveFollow(owner, 0.0f, 0.0f);
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if(uiCheckTimer <= diff)
            {
                if(Creature* pHelboar = me->FindNearestCreature(NPC_DERANGED_HELBOAR, 10.0f, false))
                {
                    if(pHelboar->GetGUID() != uiHelboarGUID && me->GetMotionMaster()->GetCurrentMovementGeneratorType() != POINT_MOTION_TYPE && !me->FindCurrentSpellBySpellId(SPELL_SUMMON_POO))
                    {
                        uiHelboarGUID.Set(pHelboar->GetGUID());
                        me->GetMotionMaster()->MovePoint(1, pHelboar->GetPositionX(), pHelboar->GetPositionY(), pHelboar->GetPositionZ());
                    }
                }
                uiCheckTimer = 5000;
            } else uiCheckTimer -= diff;

            if(!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };
};

void AddSC_hellfire_peninsula()
{
    new npc_aeranas;
    new npc_ancestral_wolf;
    new go_haaleshi_altar;
    new npc_naladu;
    new npc_tracy_proudwell;
    new npc_trollbane;
    new npc_wounded_blood_elf;
    new npc_fel_guard_hound;
}
