#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "arcatraz.h"

DoorData const doorData[] =
{
    { GO_CONTAINMENT_CORE_SECURITY_FIELD_ALPHA, DATA_SOCCOTHRATES,  DOOR_TYPE_PASSAGE },
    { GO_CONTAINMENT_CORE_SECURITY_FIELD_BETA,  DATA_DALLIAH,       DOOR_TYPE_PASSAGE },
    { 0,                                        0,                  DOOR_TYPE_ROOM } // END
};

class instance_arcatraz : public InstanceMapScript
{
    public:
        instance_arcatraz() : InstanceMapScript("instance_arcatraz", 552) { }

        struct instance_arcatraz_InstanceMapScript : public InstanceScript
        {
            instance_arcatraz_InstanceMapScript(Map* map) : InstanceScript(map)
            {
                SetBossNumber(MAX_ENCOUTER);
                LoadDoorData(doorData);

                DalliahGUID.Clear();
                SoccothratesGUID.Clear();
                MellicharGUID.Clear();
                WardensShieldGUID.Clear();

                memset(StasisPodGUIDs, 0, 5 * sizeof(uint64));
            }

            void OnCreatureCreate(Creature* creature)
            {
                switch (creature->GetEntry())
                {
                    case NPC_DALLIAH:
                        DalliahGUID.Set(creature->GetGUID());
                        break;
                    case NPC_SOCCOTHRATES:
                        SoccothratesGUID.Set(creature->GetGUID());
                        break;
                    case NPC_MELLICHAR:
                        MellicharGUID.Set(creature->GetGUID());
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* go)
            {
                InstanceScript::OnGameObjectCreate(go);
                switch (go->GetEntry())
                {
                    case GO_CONTAINMENT_CORE_SECURITY_FIELD_ALPHA:
                    case GO_CONTAINMENT_CORE_SECURITY_FIELD_BETA:
                        AddDoor(go, true);
                        break;
                    case GO_STASIS_POD_ALPHA:
                        StasisPodGUIDs[0].Set(go->GetGUID());
                        break;
                    case GO_STASIS_POD_BETA:
                        StasisPodGUIDs[1].Set(go->GetGUID());
                        break;
                    case GO_STASIS_POD_DELTA:
                        StasisPodGUIDs[2].Set(go->GetGUID());
                        break;
                    case GO_STASIS_POD_GAMMA:
                        StasisPodGUIDs[3].Set(go->GetGUID());
                        break;
                    case GO_STASIS_POD_OMEGA:
                        StasisPodGUIDs[4].Set(go->GetGUID());
                        break;
                    case GO_WARDENS_SHIELD:
                        WardensShieldGUID.Set(go->GetGUID());
                        break;
                    default:
                        break;
                }
            }

            void OnGameObjectRemove(GameObject* go)
            {
                switch (go->GetEntry())
                {
                    case GO_CONTAINMENT_CORE_SECURITY_FIELD_ALPHA:
                    case GO_CONTAINMENT_CORE_SECURITY_FIELD_BETA:
                        AddDoor(go, false);
                        break;
                    default:
                        break;
                }
            }

            void SetData(uint32 type, uint32 data)
            {
                switch (type)
                {
                    case DATA_WARDEN_1:
                    case DATA_WARDEN_2:
                    case DATA_WARDEN_3:
                    case DATA_WARDEN_4:
                    case DATA_WARDEN_5:
                        if (data < FAIL)
                            HandleGameObject(StasisPodGUIDs[type - DATA_WARDEN_1], data == IN_PROGRESS);
                        if (Creature* warden = instance->GetCreature(MellicharGUID))
                            warden->AI()->SetData(type, data);
                        break;
                }
            }

            uint32 GetData(uint32 type) const
            {
                return 0;
            }

            ObjectGuid GetGuidData(uint32 data) const
            {
                switch (data)
                {
                    case DATA_DALLIAH:
                        return DalliahGUID;
                    case DATA_SOCCOTHRATES:
                        return SoccothratesGUID;
                    case DATA_WARDENS_SHIELD:
                        return WardensShieldGUID;
                }
                return ObjectGuid::Empty;
            }

            bool SetBossState(uint32 type, EncounterState state)
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                if (type == DATA_WARDEN_MELLICHAR && state == NOT_STARTED)
                {
                    SetData(DATA_WARDEN_1, NOT_STARTED);
                    SetData(DATA_WARDEN_2, NOT_STARTED);
                    SetData(DATA_WARDEN_3, NOT_STARTED);
                    SetData(DATA_WARDEN_4, NOT_STARTED);
                    SetData(DATA_WARDEN_5, NOT_STARTED);
                    HandleGameObject(WardensShieldGUID, true);
                }

                return true;
            }

        protected:
            ObjectGuid DalliahGUID;
            ObjectGuid SoccothratesGUID;
            ObjectGuid StasisPodGUIDs[5];
            ObjectGuid MellicharGUID;
            ObjectGuid WardensShieldGUID;
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const
        {
            return new instance_arcatraz_InstanceMapScript(map);
        }
};

void AddSC_instance_arcatraz()
{
    new instance_arcatraz();
}

