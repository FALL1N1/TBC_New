#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "EscortAI.h"
#include "Player.h"

/*######
## npc_galen_goodward
######*/

enum Galen
{
    QUEST_GALENS_ESCAPE     = 1393,
    GO_GALENS_CAGE          = 37118,
    SAY_PERIODIC            = 0,
    SAY_QUEST_ACCEPTED      = 1,
    SAY_ATTACKED            = 2,
    SAY_QUEST_COMPLETE      = 3,
    EMOTE_WHISPER           = 4,
    EMOTE_DISAPPEAR         = 5
};

class npc_galen_goodward : public CreatureScript
{
public:
    npc_galen_goodward() : CreatureScript("npc_galen_goodward") { }

    struct npc_galen_goodwardAI : public EscortAI
    {
        npc_galen_goodwardAI(Creature* creature) : EscortAI(creature)
        {
            galensCageGUID.Clear();
            Reset();
        }

        void Reset()
        {
            periodicSay = 6000;
        }

        void EnterCombat(Unit* who)
        {
            if (HasEscortState(STATE_ESCORT_ESCORTING))
                Talk(SAY_ATTACKED, who);
        }

        void sQuestAccept(Player* player, Quest const* quest)
        {
            if (quest->GetQuestId() == QUEST_GALENS_ESCAPE)
            {
                Talk(SAY_QUEST_ACCEPTED, player);
                EscortAI::Start(false, false, player->GetGUID(), quest);
            }
        }

        void WaypointStart(uint32 uiPointId)
        {
            switch (uiPointId)
            {
            case 0:
                {
                    GameObject* cage = NULL;
                    if (galensCageGUID)
                        cage = me->GetMap()->GetGameObject(galensCageGUID);
                    else
                        cage = GetClosestGameObjectWithEntry(me, GO_GALENS_CAGE, INTERACTION_DISTANCE);
                    if (cage)
                    {
                        cage->UseDoorOrButton();
                        galensCageGUID.Set(cage->GetGUID());
                    }
                    break;
                }
            case 21:
                Talk(EMOTE_DISAPPEAR);
                break;
            }
        }

        void WaypointReached(uint32 waypointId, uint32)
        {
            switch (waypointId)
            {
                case 0:
                    if (GameObject* cage = me->GetMap()->GetGameObject(galensCageGUID))
                        cage->ResetDoorOrButton();
                    break;
                case 20:
                    if (Player* player = GetPlayerForEscort())
                    {
                        me->SetFacingToObject(player);
                        Talk(SAY_QUEST_COMPLETE, player);
                        Talk(EMOTE_WHISPER, player);
                        player->GroupEventHappens(QUEST_GALENS_ESCAPE, me);
                    }
                    SetRun(true);
                    break;
            }
        }

        void UpdateAI(uint32 diff)
        {
            EscortAI::UpdateAI(diff);

            if (HasEscortState(STATE_ESCORT_NONE))
                return;

            if (periodicSay < diff)
            {
                if (!HasEscortState(STATE_ESCORT_ESCORTING))
                    Talk(SAY_PERIODIC);
                periodicSay = 15000;
            }
            else
                periodicSay -= diff;

            DoMeleeAttackIfReady();
        }

    private:
        ObjectGuid galensCageGUID;
        uint32 periodicSay;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_galen_goodwardAI(creature);
    }
};

void AddSC_swamp_of_sorrows()
{
    new npc_galen_goodward();
}
