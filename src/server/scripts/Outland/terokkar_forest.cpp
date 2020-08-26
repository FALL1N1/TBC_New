#include "ScriptPCH.h"
#include "EscortAI.h"
#include "Group.h"

enum eCaptive
{
    GO_VEIL_SKITH_CAGE        = 185202,
    GO_VEIL_SKITH_CAGE2       = 185203,
    GO_VEIL_SKITH_CAGE3       = 185204,
    GO_VEIL_SKITH_CAGE4       = 185205
};

class npc_captive_child : public CreatureScript
{
public:
    npc_captive_child() : CreatureScript("npc_captive_child") { }

    struct npc_captive_childAI : public ScriptedAI
    {
        npc_captive_childAI(Creature* pCreature) : ScriptedAI(pCreature) { }

        uint32 FleeTimer;

        void Reset()
        {
            FleeTimer           = 0;
            GameObject* pCage   = NULL;
            if(pCage = me->FindNearestGameObject(GO_VEIL_SKITH_CAGE, 5.0f))
                pCage->ResetDoorOrButton();
            else if(pCage = me->FindNearestGameObject(GO_VEIL_SKITH_CAGE2, 5.0f))
                pCage->ResetDoorOrButton();
            else if(pCage = me->FindNearestGameObject(GO_VEIL_SKITH_CAGE3, 5.0f))
                pCage->ResetDoorOrButton();
            else if(pCage = me->FindNearestGameObject(GO_VEIL_SKITH_CAGE4, 5.0f))
                pCage->ResetDoorOrButton();
        }

        void UpdateAI(const uint32 diff)
        {
            if(FleeTimer)
            {
                if(FleeTimer <= diff)
                    me->ForcedDespawn();
                else
                    FleeTimer -= diff;
            }
        }
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_captive_childAI(pCreature);
    }
};

enum SPrisoner
{
    SAY_START                     = -1000716,
    SAY_DONT_LIKE                 = -1000717,
    SAY_COMPLETE                  = -1000718,
    GO_PRISONER_CAGE              = 185952,
    QUEST_ESCAPE_FROM_SKETTIS     = 11085
};

class npc_skyguard_prisoner : public CreatureScript
{
public:
    npc_skyguard_prisoner(): CreatureScript ("npc_skyguard_prisoner") { }

    bool OnQuestAccept(Player* pPlayer, Creature* pCreature, Quest const * pQuest)
    {
        if(pQuest->GetQuestId() == QUEST_ESCAPE_FROM_SKETTIS)
        {
            if(GameObject* pGo = pCreature->FindNearestGameObject(GO_PRISONER_CAGE, 10.0f))
                pGo->UseDoorOrButton();

            if(npc_skyguard_prisoner::npc_skyguard_prisonerAI* pEscortAI = CAST_AI(npc_skyguard_prisoner::npc_skyguard_prisonerAI, pCreature->AI()))
                pEscortAI->StartEvent(pPlayer, pQuest);
        }
        return true;
    }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_skyguard_prisonerAI(pCreature);
    }

    struct npc_skyguard_prisonerAI : public EscortAI
    {
        npc_skyguard_prisonerAI(Creature* pCreature) : EscortAI(pCreature) { }

        void Reset() { }

        uint32 CalculateWaypointID()
        {
            switch(me->GetGUID())
            {
                case 1189307:   return 1;
                case 1189309:   return 2;
                default:        return 3;
            }
        }

        void StartEvent(Player* pPlayer, const Quest* pQuest)
        {
            switch(CalculateWaypointID())
            {
                case 1:
                    AddWaypoint(0, -4108.25f, 3032.18f, 344.799f, 3000);
                    AddWaypoint(1, -4114.41f, 3036.73f, 344.039f);
                    AddWaypoint(2, -4126.41f, 3026.07f, 344.156f);
                    AddWaypoint(3, -4145.17f, 3029.69f, 337.423f);
                    AddWaypoint(4, -4173.69f, 3035.72f, 343.346f);
                    AddWaypoint(5, -4173.70f, 3047.37f, 343.888f);
                    AddWaypoint(6, -4183.47f, 3060.62f, 344.157f, 3000);
                    AddWaypoint(7, -4179.13f, 3090.20f, 323.971f, 30000);
                    Start(false, false, pPlayer->GetGUID(), pQuest);
                    break;
                case 2:
                    AddWaypoint(0, -3718.81f, 3787.24f, 302.890f, 3000);
                    AddWaypoint(1, -3714.44f, 3780.35f, 302.075f);
                    AddWaypoint(2, -3698.33f, 3788.04f, 302.171f);
                    AddWaypoint(3, -3679.36f, 3780.25f, 295.077f);
                    AddWaypoint(4, -3654.82f, 3770.43f, 301.291f);
                    AddWaypoint(5, -3656.07f, 3757.31f, 301.985f);
                    AddWaypoint(6, -3648.83f, 3743.07f, 302.173f, 3000);
                    AddWaypoint(7, -3659.16f, 3714.94f, 281.576f, 30000);
                    Start(false, false, pPlayer->GetGUID(), pQuest);
                    break;
                case 3:
                    AddWaypoint(0, -3671.51f, 3385.36f, 312.956f, 3000);
                    AddWaypoint(1, -3677.74f, 3379.05f, 312.136f);
                    AddWaypoint(2, -3667.52f, 3366.45f, 312.233f);
                    AddWaypoint(3, -3672.87f, 3343.52f, 304.994f);
                    AddWaypoint(4, -3679.35f, 3319.01f, 311.419f);
                    AddWaypoint(5, -3692.93f, 3318.69f, 312.081f);
                    AddWaypoint(6, -3704.08f, 3309.56f, 312.233f, 3000);
                    AddWaypoint(7, -3733.99f, 3315.77f, 292.093f, 30000);
                    Start(false, false, pPlayer->GetGUID(), pQuest);
                    break;
            }
            return;
        }

        void WaypointReached(uint32 uiPointId, uint32)
        {
            Player* pPlayer = GetPlayerForEscort();
            if(pPlayer)
                switch(uiPointId)
                {
                    case 0:
                        DoScriptText(SAY_START, me);
                        break;
                    case 6:
                        DoScriptText(SAY_DONT_LIKE, me);
                        break;
                    case 7:
                        DoScriptText(SAY_COMPLETE, me);
                        pPlayer->GroupEventHappens(QUEST_ESCAPE_FROM_SKETTIS, me);
                        break;
                }
        }

        void UpdateAI(const uint32 diff)
        {
            EscortAI::UpdateAI(diff);

            if(!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };
};

enum NpcLetoll
{
    SAY_LE_START                    = -1000725,
    SAY_LE_KEEP_SAFE                = -1000726,
    SAY_LE_NORTH                    = -1000727,
    SAY_LE_ARRIVE                   = -1000728,
    SAY_LE_BURIED                   = -1000729,
    SAY_LE_ALMOST                   = -1000730,
    SAY_LE_DRUM                     = -1000731,
    SAY_LE_DRUM_REPLY               = -1000732,
    SAY_LE_DISCOVERY                = -1000733,
    SAY_LE_DISCOVERY_REPLY          = -1000734,
    SAY_LE_NO_LEAVE                 = -1000735,
    SAY_LE_NO_LEAVE_REPLY1          = -1000736,
    SAY_LE_NO_LEAVE_REPLY2          = -1000737,
    SAY_LE_NO_LEAVE_REPLY3          = -1000738,
    SAY_LE_NO_LEAVE_REPLY4          = -1000739,
    SAY_LE_SHUT                     = -1000740,
    SAY_LE_REPLY_HEAR               = -1000741,
    SAY_LE_IN_YOUR_FACE             = -1000742,
    SAY_LE_HELP_HIM                 = -1000743,
    EMOTE_LE_PICK_UP                = -1000744,
    SAY_LE_THANKS                   = -1000745,
    QUEST_DIGGING_BONES             = 10922,
    NPC_RESEARCHER                  = 22464,
    NPC_BONE_SIFTER                 = 22466,
    MAX_RESEARCHER                  = 4
};

class npc_letoll : public CreatureScript
{
public:
    npc_letoll() : CreatureScript("npc_letoll") { }

    bool OnQuestAccept(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
    {
        if(pQuest->GetQuestId() == QUEST_DIGGING_BONES)
        {
            if(npc_letollAI* pEscortAI = dynamic_cast<npc_letollAI*>(pCreature->AI()))
            {
                DoScriptText(SAY_LE_START, pCreature);
                pCreature->SetFaction(113);

                pEscortAI->Start(false, false, pPlayer->GetGUID(), pQuest, true);
            }
        }
        return true;
    }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_letollAI(pCreature);
    }

    struct npc_letollAI : public EscortAI
    {
        npc_letollAI(Creature* pCreature) : EscortAI(pCreature)
        {
            m_uiEventTimer = 5000;
            m_uiEventCount = 0;
            Reset();
        }

        std::list<Creature*> m_lResearchersList;
        uint32 m_uiEventTimer;
        uint32 m_uiEventCount;

        void Reset() { }

        void SetFormation()
        {
            uint32 uiCount = 0;
            for(std::list<Creature*>::iterator itr = m_lResearchersList.begin(); itr != m_lResearchersList.end(); ++itr)
            {
                float fAngle = uiCount < MAX_RESEARCHER ? M_PI/MAX_RESEARCHER - (uiCount*2*M_PI/MAX_RESEARCHER) : 0.0f;

                if((*itr)->IsAlive() && !(*itr)->IsInCombat())
                    (*itr)->GetMotionMaster()->MoveFollow(me, 2.5f, fAngle);

                ++uiCount;
            }
        }

        Creature* GetAvailableResearcher(uint8 uiListNum)
        {
            if(!m_lResearchersList.empty())
            {
                uint8 uiNum = 1;

                for(std::list<Creature*>::iterator itr = m_lResearchersList.begin(); itr != m_lResearchersList.end(); ++itr)
                {
                    if(uiListNum && uiListNum != uiNum)
                    {
                        ++uiNum;
                        continue;
                    }

                    if((*itr)->IsAlive() && (*itr)->IsWithinDistInMap(me, 20.0f))
                        return (*itr);
                }
            }
            return NULL;
        }

        void JustStartedEscort()
        {
            m_uiEventTimer = 5000;
            m_uiEventCount = 0;

            m_lResearchersList.clear();

            float x, y, z;
            me->GetPosition(x, y, z);

            CellCoord pair(Trinity::ComputeCellCoord(x, y)); 
            Cell cell(pair);
            cell.data.Part.reserved = ALL_DISTRICT;
            cell.SetNoCreate();

            Trinity::AllCreaturesOfEntryInRange check(me, NPC_RESEARCHER, 25);
            Trinity::CreatureListSearcher<Trinity::AllCreaturesOfEntryInRange> searcher(me,m_lResearchersList, check);
            TypeContainerVisitor<Trinity::CreatureListSearcher<Trinity::AllCreaturesOfEntryInRange>, GridTypeMapContainer> cSearcher(searcher); 
            cell.Visit(pair, cSearcher, *(me->GetMap()), *me, 25);

            if(!m_lResearchersList.empty())
                SetFormation();
        }

        void WaypointReached(uint32 uiPointId, uint32)
        {
            Player* pPlayer = GetPlayerForEscort();

            if(!pPlayer)
                return;

            switch(uiPointId)
            {
                case 0:
                    if(pPlayer)
                        DoScriptText(SAY_LE_KEEP_SAFE, me, pPlayer);
                    break;
                case 1:
                    DoScriptText(SAY_LE_NORTH, me);
                    break;
                case 10:
                    DoScriptText(SAY_LE_ARRIVE, me);
                    break;
                case 12:
                    DoScriptText(SAY_LE_BURIED, me);
                    SetEscortPaused(true);
                    break;
                case 13:
                    DoScriptText(EMOTE_LE_PICK_UP, me);
                    if(pPlayer)
                    {
                        DoScriptText(SAY_LE_THANKS, me, pPlayer);
                        pPlayer->GroupEventHappens(QUEST_DIGGING_BONES, me);
                    }
                    SetRun();
                    break;
            }
        }

        void Aggro(Unit* pWho)
        {
            if(pWho->IsInCombat() && pWho->GetTypeId() == TYPEID_UNIT && pWho->GetEntry() == NPC_BONE_SIFTER)
                DoScriptText(SAY_LE_HELP_HIM, me);
        }

        void JustSummoned(Creature* pSummoned)
        {
            Player* pPlayer = GetPlayerForEscort();

            if(pPlayer && pPlayer->IsAlive())
                pSummoned->AI()->AttackStart(pPlayer);
            else
                pSummoned->AI()->AttackStart(me);
        }

        void UpdateEscortAI(const uint32 diff)
        {
            if(!UpdateVictim())
            {
                if(HasEscortState(STATE_ESCORT_PAUSED))
                {
                    if(m_uiEventTimer < diff)
                    {
                        m_uiEventTimer = 7000;

                        switch(m_uiEventCount)
                        {
                            case 0:
                                DoScriptText(SAY_LE_ALMOST, me);
                                break;
                            case 1:
                                DoScriptText(SAY_LE_DRUM, me);
                                break;
                            case 2:
                                if(Creature* pResearcher = GetAvailableResearcher(0))
                                    DoScriptText(SAY_LE_DRUM_REPLY, pResearcher);
                                break;
                            case 3:
                                DoScriptText(SAY_LE_DISCOVERY, me);
                                break;
                            case 4:
                                if(Creature* pResearcher = GetAvailableResearcher(0))
                                    DoScriptText(SAY_LE_DISCOVERY_REPLY, pResearcher);
                                break;
                            case 5:
                                DoScriptText(SAY_LE_NO_LEAVE, me);
                                break;
                            case 6:
                                if(Creature* pResearcher = GetAvailableResearcher(1))
                                    DoScriptText(SAY_LE_NO_LEAVE_REPLY1, pResearcher);
                                break;
                            case 7:
                                if(Creature* pResearcher = GetAvailableResearcher(2))
                                    DoScriptText(SAY_LE_NO_LEAVE_REPLY2, pResearcher);
                                break;
                            case 8:
                                if(Creature* pResearcher = GetAvailableResearcher(3))
                                    DoScriptText(SAY_LE_NO_LEAVE_REPLY3, pResearcher);
                                break;
                            case 9:
                                if(Creature* pResearcher = GetAvailableResearcher(4))
                                    DoScriptText(SAY_LE_NO_LEAVE_REPLY4, pResearcher);
                                break;
                            case 10:
                                DoScriptText(SAY_LE_SHUT, me);
                                break;
                            case 11:
                                if(Creature* pResearcher = GetAvailableResearcher(0))
                                    DoScriptText(SAY_LE_REPLY_HEAR, pResearcher);
                                break;
                            case 12:
                                DoScriptText(SAY_LE_IN_YOUR_FACE, me);
                                me->SummonCreature(NPC_BONE_SIFTER, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                                break;
                            case 13:
                                SetEscortPaused(false);
                                break;
                        }

                        ++m_uiEventCount;
                    } else m_uiEventTimer -= diff;
                }

                return;
            }

            DoMeleeAttackIfReady();
        }
    };
};

#define SAY_SUBMIT                      -1000194

#define FACTION_HOSTILE                 45
#define FACTION_FRIENDLY                35
#define QUEST_DONTKILLTHEFATONE         9889

#define SPELL_PULVERIZE                 2676

class item_terrokkar_fumper : public ItemScript
{
public:
    item_terrokkar_fumper() : ItemScript("item_terrokkar_fumper") { }

    bool OnUse(Player* player, Item* /*pItem*/, SpellCastTargets const& /*targets*/)
    {
        player->CastSpell(player, 39238, false);

        if(player->GetQuestStatus(10929) == QUEST_STATUS_INCOMPLETE)
            player->SummonCreature(22482, player->GetPositionX(), player->GetPositionY() - 1.2f, player->GetPositionZ() + 1.0f, 0.0, TEMPSUMMON_DEAD_DESPAWN, 120000);
        return true;
    }
};

class item_terrokkar_fumper_tbbw : public ItemScript
{
public:
    item_terrokkar_fumper_tbbw() : ItemScript("item_terrokkar_fumper_tbbw") { }

    bool OnUse(Player* player, Item* /*pItem*/, SpellCastTargets const& /*targets*/)
    {
        player->CastSpell(player, 39246, false);

        if(player->GetQuestStatus(10930) == QUEST_STATUS_INCOMPLETE)
        {
            switch(irand(0, 1))
            {
                case 1:
                    player->SummonCreature(22038, player->GetPositionX(), player->GetPositionY() - 5.0f, player->GetPositionZ() + 1.0f, 0.0, TEMPSUMMON_DEAD_DESPAWN, 120000);
                    break;
                case 0:
                    player->SummonCreature(22483, player->GetPositionX(), player->GetPositionY() - 1.2f, player->GetPositionZ() + 1.0f, 0.0, TEMPSUMMON_DEAD_DESPAWN, 120000);
                    break;
            }
        }
        return true;
    }
};

class mob_unkor_the_ruthless : public CreatureScript
{
public:
    mob_unkor_the_ruthless() : CreatureScript("mob_unkor_the_ruthless") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new mob_unkor_the_ruthlessAI(pCreature);
    }

    struct mob_unkor_the_ruthlessAI : public ScriptedAI
    {
        mob_unkor_the_ruthlessAI(Creature* pCreature) : ScriptedAI(pCreature) { }

        bool CanDoQuest;
        uint32 UnkorUnfriendly_Timer;
        uint32 Pulverize_Timer;

        void Reset()
        {
            CanDoQuest = false;
            UnkorUnfriendly_Timer = 0;
            Pulverize_Timer = 3000;
            me->SetStandState(UNIT_STAND_STATE_STAND);
            me->SetFaction(FACTION_HOSTILE);
        }

        void EnterCombat(Unit* /*pWho*/) { }

        void DoNice()
        {
            DoScriptText(SAY_SUBMIT, me);
            me->SetFaction(FACTION_FRIENDLY);
            me->SetStandState(UNIT_STAND_STATE_SIT);
            me->RemoveAllAuras();
            me->GetThreatManager().ResetAllThreat();
            me->CombatStop(true);
            UnkorUnfriendly_Timer = 60000;
        }

        void DamageTaken(Unit* done_by, uint32 &damage)
        {
            if(done_by->GetTypeId() == TYPEID_PLAYER)
                if(Player* plr = done_by->ToPlayer())
            {
                if(me->HealthBelowPctDamaged(30, damage))
                {
                    if(Group* pGroup = plr->GetGroup())
                    {
                        for(GroupReference *itr = pGroup->GetFirstMember(); itr != NULL; itr = itr->next())
                        {
                            Player* pGroupie = itr->GetSource();
                            if(pGroupie &&
                                pGroupie->GetQuestStatus(QUEST_DONTKILLTHEFATONE) == QUEST_STATUS_INCOMPLETE &&
                                pGroupie->GetReqKillOrCastCurrentCount(QUEST_DONTKILLTHEFATONE, 18260) == 10)
                            {
                                pGroupie->AreaExploredOrEventHappens(QUEST_DONTKILLTHEFATONE);
                                if(!CanDoQuest)
                                    CanDoQuest = true;
                            }
                        }
                    } else if(plr->GetQuestStatus(QUEST_DONTKILLTHEFATONE) == QUEST_STATUS_INCOMPLETE &&
                        plr->GetReqKillOrCastCurrentCount(QUEST_DONTKILLTHEFATONE, 18260) == 10)
                    {
                        plr->AreaExploredOrEventHappens(QUEST_DONTKILLTHEFATONE);
                        CanDoQuest = true;
                    }
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if(CanDoQuest)
            {
                if(!UnkorUnfriendly_Timer)
                    DoNice();
                else
                {
                    if(UnkorUnfriendly_Timer <= diff)
                    {
                        EnterEvadeMode();
                        return;
                    } else UnkorUnfriendly_Timer -= diff;
                }
            }

            if(!UpdateVictim())
                return;

            if(Pulverize_Timer <= diff)
            {
                DoCast(me, SPELL_PULVERIZE);
                Pulverize_Timer = 9000;
            } else Pulverize_Timer -= diff;

            DoMeleeAttackIfReady();
        }
    };
};

class mob_infested_root_walker : public CreatureScript
{
public:
    mob_infested_root_walker() : CreatureScript("mob_infested_root_walker") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new mob_infested_root_walkerAI(pCreature);
    }

    struct mob_infested_root_walkerAI : public ScriptedAI
    {
        mob_infested_root_walkerAI(Creature* pCreature) : ScriptedAI(pCreature) { }

        void Reset() { }
        void EnterCombat(Unit* /*pWho*/) { }

        void DamageTaken(Unit* done_by, uint32 &damage)
        {
            if(done_by && done_by->GetTypeId() == TYPEID_PLAYER)
                if(me->GetHealth() <= damage)
                    if(rand()%100 < 75)
                        //Summon Wood Mites
                        DoCast(me, 39130, true);
        }
    };
};

class npc_skywing : public CreatureScript
{
public:
    npc_skywing() : CreatureScript("npc_skywing") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_skywingAI(pCreature);
    }

    struct npc_skywingAI : public EscortAI
    {
    public:
        npc_skywingAI(Creature* pCreature) : EscortAI(pCreature) { }

        void WaypointReached(uint32 i, uint32)
        {
            Player* player = GetPlayerForEscort();
            if(!player)
                return;

            switch(i)
            {
                case 8:
                    player->AreaExploredOrEventHappens(10898);
                    break;
            }
        }

        void EnterCombat(Unit* /*pWho*/) { }

        void MoveInLineOfSight(Unit* who)
        {
            if(HasEscortState(STATE_ESCORT_ESCORTING))
                return;

            if(who->GetTypeId() == TYPEID_PLAYER)
                if(Player* plr = who->ToPlayer())
            {
                if(plr->GetQuestStatus(10898) == QUEST_STATUS_INCOMPLETE)
                {
                    if(me->IsWithinDistInMap(plr, 10.0f))
                        Start(false, false, plr->GetGUID());
                }
            }
        }

        void Reset() { }

        void UpdateAI(const uint32 diff)
        {
            EscortAI::UpdateAI(diff);
        }
    };
};

class mob_rotting_forest_rager : public CreatureScript
{
public:
    mob_rotting_forest_rager() : CreatureScript("mob_rotting_forest_rager") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new mob_rotting_forest_ragerAI(pCreature);
    }

    struct mob_rotting_forest_ragerAI : public ScriptedAI
    {
        mob_rotting_forest_ragerAI(Creature* pCreature) : ScriptedAI(pCreature) { }

        void Reset() { }
        void EnterCombat(Unit* /*pWho*/) { }

        void DamageTaken(Unit* done_by, uint32 &damage)
        {
            if(done_by->GetTypeId() == TYPEID_PLAYER)
                if(me->GetHealth() <= damage)
                    if(rand()%100 < 75)
                        //Summon Lots of Wood Mights
                        DoCast(me, 39134, true);
        }
    };
};

#define QUEST_TARGET        22459

const uint32 netherwebVictims[6] =
{
    18470, 16805, 21242, 18452, 22482, 21285
};

class mob_netherweb_victim : public CreatureScript
{
public:
    mob_netherweb_victim() : CreatureScript("mob_netherweb_victim") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new mob_netherweb_victimAI(pCreature);
    }

    struct mob_netherweb_victimAI : public ScriptedAI
    {
        mob_netherweb_victimAI(Creature* pCreature) : ScriptedAI(pCreature) { }

        void Reset() { }
        void EnterCombat(Unit* /*pWho*/) { }
        void MoveInLineOfSight(Unit* /*pWho*/) { }

        void JustDied(Unit* Killer)
        {
            if(Killer->GetTypeId() == TYPEID_PLAYER)
                if(Player* plr = Killer->ToPlayer())
            {
                if(plr->GetQuestStatus(10873) == QUEST_STATUS_INCOMPLETE)
                {
                    if(rand()%100 < 25)
                    {
                        me->SummonCreature(QUEST_TARGET, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                        plr->KilledMonsterCredit(QUEST_TARGET);
                    } else me->SummonCreature(netherwebVictims[rand()%6], 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);

                    if(rand()%100 < 75)
                        me->SummonCreature(netherwebVictims[rand()%6], 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                    me->SummonCreature(netherwebVictims[rand()%6], 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                }
            }
        }
    };
};

#define GOSSIP_FLOON1           "You owe Sim'salabim money. Hand them over or die!"
#define GOSSIP_FLOON2           "Hand over the money or die...again!"

enum eFloon
{
    SAY_FLOON_ATTACK        = -1000195,

    SPELL_SILENCE           = 6726,
    SPELL_FROSTBOLT         = 9672,
    SPELL_FROST_NOVA        = 11831,

    FACTION_HOSTILE_FL      = 1738,
    QUEST_CRACK_SKULLS      = 10009
};

class npc_floon : public CreatureScript
{
public:
    npc_floon() : CreatureScript("npc_floon") { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
    {
        ClearGossipMenuFor(player);
        if(uiAction == GOSSIP_ACTION_INFO_DEF)
        {
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_FLOON2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            SendGossipMenuFor(player, 9443, creature->GetGUID());
        }
        if(uiAction == GOSSIP_ACTION_INFO_DEF+1)
        {
            CloseGossipMenuFor(player);
            creature->SetFaction(FACTION_HOSTILE_FL);
            DoScriptText(SAY_FLOON_ATTACK, creature, player);
            creature->AI()->AttackStart(player);
        }
        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if(player->GetQuestStatus(QUEST_CRACK_SKULLS) == QUEST_STATUS_INCOMPLETE)
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_FLOON1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

        SendGossipMenuFor(player, 9442, creature->GetGUID());
        return true;
    }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_floonAI(pCreature);
    }

    struct npc_floonAI : public ScriptedAI
    {
        npc_floonAI(Creature* pCreature) : ScriptedAI(pCreature)
        {
            m_uiNormFaction = pCreature->GetFaction();
        }

        uint32 m_uiNormFaction;
        uint32 Silence_Timer;
        uint32 Frostbolt_Timer;
        uint32 FrostNova_Timer;

        void Reset()
        {
            Silence_Timer = 2000;
            Frostbolt_Timer = 4000;
            FrostNova_Timer = 9000;
            if(me->GetFaction() != m_uiNormFaction)
                me->SetFaction(m_uiNormFaction);
        }

        void EnterCombat(Unit* /*pWho*/) { }

        void UpdateAI(const uint32 diff)
        {
            if(!UpdateVictim())
                return;

            if(Silence_Timer <= diff)
            {
                DoCast(me->GetVictim(), SPELL_SILENCE);
                Silence_Timer = 30000;
            } else Silence_Timer -= diff;

            if(FrostNova_Timer <= diff)
            {
                DoCast(me, SPELL_FROST_NOVA);
                FrostNova_Timer = 20000;
            } else FrostNova_Timer -= diff;

            if(Frostbolt_Timer <= diff)
            {
                DoCast(me->GetVictim(), SPELL_FROSTBOLT);
                Frostbolt_Timer = 5000;
            } else Frostbolt_Timer -= diff;

            DoMeleeAttackIfReady();
        }
    };
};

enum eIslaStarmaneData
{
    SAY_PROGRESS_1  = -1000571,
    SAY_PROGRESS_2  = -1000572,
    SAY_PROGRESS_3  = -1000573,
    SAY_PROGRESS_4  = -1000574,

    QUEST_EFTW_H    = 10052,
    QUEST_EFTW_A    = 10051,
    GO_CAGE         = 182794,
    SPELL_CAT       = 32447,
};

class npc_isla_starmane : public CreatureScript
{
public:
    npc_isla_starmane() : CreatureScript("npc_isla_starmane") { }

    struct npc_isla_starmaneAI : public EscortAI
    {
        npc_isla_starmaneAI(Creature* pCreature) : EscortAI(pCreature) { }

        void WaypointReached(uint32 i, uint32)
        {
            Player* pPlayer = GetPlayerForEscort();

            if(!pPlayer)
                return;

            switch(i)
            {
                case 0:
                {
                    if(GameObject* pCage = me->FindNearestGameObject(GO_CAGE, 10))
                        pCage->SetGoState(GO_STATE_ACTIVE);
                }
                    break;
                case 2: DoScriptText(SAY_PROGRESS_1, me, pPlayer); break;
                case 5: DoScriptText(SAY_PROGRESS_2, me, pPlayer); break;
                case 6: DoScriptText(SAY_PROGRESS_3, me, pPlayer); break;
                case 29:DoScriptText(SAY_PROGRESS_4, me, pPlayer);
                    if(pPlayer->GetTeam() == ALLIANCE)
                        pPlayer->GroupEventHappens(QUEST_EFTW_A, me);
                    else if(pPlayer->GetTeam() == HORDE)
                        pPlayer->GroupEventHappens(QUEST_EFTW_H, me);
                    me->SetInFront(pPlayer); break;
                case 30: me->HandleEmoteCommand(EMOTE_ONESHOT_WAVE); break;
                case 31: DoCast(me, SPELL_CAT);
                    me->RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING); break;
            }
        }

        void Reset()
        {
            me->RestoreFaction();
        }

        void JustDied(Unit* /*pKiller*/)
        {
            if(Player* player = GetPlayerForEscort())
            {
                if(player->GetTeam() == ALLIANCE)
                    player->FailQuest(QUEST_EFTW_A);
                else if(player->GetTeam() == HORDE)
                    player->FailQuest(QUEST_EFTW_H);
            }
        }
    };

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if(quest->GetQuestId() == QUEST_EFTW_H || quest->GetQuestId() == QUEST_EFTW_A)
        {
            CAST_AI(EscortAI, (creature->AI()))->Start(true, false, player->GetGUID());
            creature->SetFaction(113);
        }
        return true;
    }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_isla_starmaneAI(pCreature);
    }
};

#define GOSSIP_S_DARKSCREECHER_AKKARAI         "Summon Darkscreecher Akkarai"
#define GOSSIP_S_KARROG         "Summon Karrog"
#define GOSSIP_S_GEZZARAK_THE_HUNTRESS         "Summon Gezzarak the Huntress"
#define GOSSIP_S_VAKKIZ_THE_WINDRAGER         "Summon Vakkiz the Windrager"

class go_skull_pile : public GameObjectScript
{
public:
    go_skull_pile() : GameObjectScript("go_skull_pile") { }

    bool OnGossipSelect(Player* player, GameObject* pGo, uint32 uiSender, uint32 uiAction)
    {
        ClearGossipMenuFor(player);
        switch(uiSender)
        {
            case GOSSIP_SENDER_MAIN:    SendActionMenu(player, pGo, uiAction); break;
        }
        return true;
    }

    bool OnGossipHello(Player* player, GameObject* pGo)
    {
        if((player->GetQuestStatus(11885) == QUEST_STATUS_INCOMPLETE) || player->GetQuestRewardStatus(11885))
        {
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_S_DARKSCREECHER_AKKARAI, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_S_KARROG, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_S_GEZZARAK_THE_HUNTRESS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_S_VAKKIZ_THE_WINDRAGER, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
        }

        SendGossipMenuFor(player, pGo->GetGOInfo()->questgiver.gossipID, pGo->GetGUID());
        return true;
    }

    void SendActionMenu(Player* player, GameObject* /*pGo*/, uint32 uiAction)
    {
        switch(uiAction)
        {
            case GOSSIP_ACTION_INFO_DEF + 1:
                player->CastSpell(player, 40642, false);
                break;
            case GOSSIP_ACTION_INFO_DEF + 2:
                player->CastSpell(player, 40640, false);
                break;
            case GOSSIP_ACTION_INFO_DEF + 3:
                player->CastSpell(player, 40632, false);
                break;
            case GOSSIP_ACTION_INFO_DEF + 4:
                player->CastSpell(player, 40644, false);
                break;
        }
    }
};

enum eSlim
{
    FACTION_CONSORTIUM  = 933
};

class npc_slim : public CreatureScript
{
public:
    npc_slim() : CreatureScript("npc_slim") { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
    {
        ClearGossipMenuFor(player);
        if(uiAction == GOSSIP_ACTION_TRADE)
            player->GetSession()->SendListInventory(creature->GetGUID());

        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if(creature->IsVendor() && player->GetReputationRank(FACTION_CONSORTIUM) >= REP_FRIENDLY)
        {
            AddGossipItemFor(player, GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);
            SendGossipMenuFor(player, 9896, creature->GetGUID());
        }
        else
            SendGossipMenuFor(player, 9895, creature->GetGUID());

        return true;
    }
};

enum eAkuno
{
    QUEST_ESCAPING_THE_TOMB = 10887,
    NPC_CABAL_SKRIMISHER    = 21661
};

class npc_akuno : public CreatureScript
{
public:
    npc_akuno() : CreatureScript("npc_akuno") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* pQuest)
    {
        if(pQuest->GetQuestId() == QUEST_ESCAPING_THE_TOMB)
        {
            if(npc_akunoAI* pEscortAI = CAST_AI(npc_akuno::npc_akunoAI, creature->AI()))
                pEscortAI->Start(false, false, player->GetGUID());

            if(player->GetTeamId() == TEAM_ALLIANCE)
                creature->SetFaction(10);
            else
                creature->SetFaction(33);
        }
        return true;
    }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_akunoAI(pCreature);
    }

    struct npc_akunoAI : public EscortAI
    {
        npc_akunoAI(Creature* pCreature) : EscortAI(pCreature) { }

        void WaypointReached(uint32 i, uint32)
        {
            Player* player = GetPlayerForEscort();

            if(!player)
                return;

            switch(i)
            {
                case 3:
                    me->SummonCreature(NPC_CABAL_SKRIMISHER, -2795.99f, 5420.33f, -34.53f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
                    me->SummonCreature(NPC_CABAL_SKRIMISHER, -2793.55f, 5412.79f, -34.53f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
                    break;
                case 11:
                    if(player && player->GetTypeId() == TYPEID_PLAYER)
                        player->GroupEventHappens(QUEST_ESCAPING_THE_TOMB, me);
                    break;
            }
        }

        void JustSummoned(Creature* summon)
        {
            summon->AI()->AttackStart(me);
        }
    };
};

class go_mana_bomb : public GameObjectScript
{
public:
    go_mana_bomb() : GameObjectScript("go_mana_bomb") { }

    bool OnGossipHello(Player* pPlayer, GameObject* /*pGo*/)
    {
        if(pPlayer->GetQuestStatus(10446) == QUEST_STATUS_INCOMPLETE || pPlayer->GetQuestStatus(10447) == QUEST_STATUS_INCOMPLETE)
            pPlayer->KilledMonsterCredit(21039);
        return true;
    }
};

#define GREK_OPTION      "Enchant Your weapon please"
class npc_grek : public CreatureScript
{
public:
    npc_grek() : CreatureScript("npc_grek") {}

    bool OnGossipHello(Player* player, Creature* pCreature)
    {
        if(player->GetQuestStatus(10201) == QUEST_STATUS_INCOMPLETE)
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, GREK_OPTION, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        SendGossipMenuFor(player, player->GetGossipTextId(pCreature), pCreature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
    {
        if(uiAction==GOSSIP_ACTION_INFO_DEF+1)
        {
            player->KilledMonsterCredit(19606);
            CloseGossipMenuFor(player);
        }
        return true;
    }
};

enum DarkStoneofTerrok
{
    QUEST_DARKSTONE_OF_TERRROK      = 10839,
    NPC_TERROKAR_QUEST_TARGET       = 22288
};

class rod_of_purification : public ItemScript
{
public:
    rod_of_purification() : ItemScript("rod_of_purification") { }

    bool OnUse(Player* pPlayer, Item* /*item*/, SpellCastTargets const& /*targets*/)
    {
        if(pPlayer->GetQuestStatus(QUEST_DARKSTONE_OF_TERRROK) == QUEST_STATUS_INCOMPLETE)
        {
            if(Creature* pCreature = pPlayer->FindNearestCreature(NPC_TERROKAR_QUEST_TARGET, 5.0f, true))
                pPlayer->CompleteQuest(QUEST_DARKSTONE_OF_TERRROK);
        }
        return false;
    }
};

void AddSC_terokkar_forest()
{
    new npc_letoll;
    new npc_captive_child;
    new npc_skyguard_prisoner;
    new item_terrokkar_fumper;
    new item_terrokkar_fumper_tbbw;
    new mob_unkor_the_ruthless;
    new mob_infested_root_walker;
    new mob_rotting_forest_rager;
    new mob_netherweb_victim;
    new npc_floon;
    new npc_isla_starmane;
    new go_skull_pile;
    new npc_skywing;
    new npc_slim;
    new npc_akuno;
    new go_mana_bomb;
    new npc_grek;
    new rod_of_purification;
}
