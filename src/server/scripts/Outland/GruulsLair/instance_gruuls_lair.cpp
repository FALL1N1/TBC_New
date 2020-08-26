#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "gruuls_lair.h"

DoorData const doorData[] =
{
    { GO_MAULGAR_DOOR,  DATA_MAULGAR,   DOOR_TYPE_PASSAGE },
    { GO_GRUUL_DOOR,    DATA_GRUUL,     DOOR_TYPE_ROOM },
    { 0,                0,              DOOR_TYPE_ROOM  } // END
};

MinionData const minionData[] =
{
    { NPC_MAULGAR,              DATA_MAULGAR },
    { NPC_KROSH_FIREHAND,       DATA_MAULGAR },
    { NPC_OLM_THE_SUMMONER,     DATA_MAULGAR },
    { NPC_KIGGLER_THE_CRAZED,   DATA_MAULGAR },
    { NPC_BLINDEYE_THE_SEER,    DATA_MAULGAR }
};

class instance_gruuls_lair : public InstanceMapScript
{
    public:
        instance_gruuls_lair() : InstanceMapScript(GLScriptName, 565) { }

        struct instance_gruuls_lair_InstanceMapScript : public InstanceScript
        {
            instance_gruuls_lair_InstanceMapScript(Map* map) : InstanceScript(map)
            {
                SetBossNumber(MAX_ENCOUNTER);
                LoadDoorData(doorData);
                LoadMinionData(minionData);

                _maulgarGUID.Clear();
                _addsKilled = 0;
            }

            void OnCreatureCreate(Creature* creature)
            {
                InstanceScript::OnCreatureCreate(creature);

                switch (creature->GetEntry())
                {
                    case NPC_MAULGAR:
                        _maulgarGUID.Set(creature->GetGUID());
                        // no break;
                    case NPC_KROSH_FIREHAND:
                    case NPC_OLM_THE_SUMMONER:
                    case NPC_KIGGLER_THE_CRAZED:
                    case NPC_BLINDEYE_THE_SEER:
                        AddMinion(creature, true);
                        break;
                }
            }

            void OnCreatureRemove(Creature* creature)
            {
                switch (creature->GetEntry())
                {
                    case NPC_MAULGAR:
                    case NPC_KROSH_FIREHAND:
                    case NPC_OLM_THE_SUMMONER:
                    case NPC_KIGGLER_THE_CRAZED:
                    case NPC_BLINDEYE_THE_SEER:
                        AddMinion(creature, false);
                        break;
                }
            }


            void OnGameObjectCreate(GameObject* go)
            {
                InstanceScript::OnGameObjectCreate(go);

                switch (go->GetEntry())
                {
                    case GO_MAULGAR_DOOR:
                    case GO_GRUUL_DOOR:
                        AddDoor(go, true);
                        break;
                }
            }

            void OnGameObjectRemove(GameObject* go)
            {
                switch (go->GetEntry())
                {
                    case GO_MAULGAR_DOOR:
                    case GO_GRUUL_DOOR:
                        AddDoor(go, false);
                        break;
                }
            }

            bool SetBossState(uint32 id, EncounterState state)
            {
                if (!InstanceScript::SetBossState(id, state))
                    return false;

                if (id == DATA_MAULGAR && state == NOT_STARTED)
                    _addsKilled = 0;
                return true;
            }

            void SetData(uint32 type, uint32 id)
            {
                if (type == DATA_ADDS_KILLED)
                    if (Creature* maulgar = instance->GetCreature(_maulgarGUID))
                        maulgar->AI()->DoAction(++_addsKilled);
            }

            uint32 GetData(uint32 type) const
            {
                if (type == DATA_ADDS_KILLED)
                    return _addsKilled;
                return 0;
            }
              
        protected:
            uint32 _addsKilled;
            ObjectGuid _maulgarGUID;
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const
        {
            return new instance_gruuls_lair_InstanceMapScript(map);
        }
};

void AddSC_instance_gruuls_lair()
{
    new instance_gruuls_lair();
}
