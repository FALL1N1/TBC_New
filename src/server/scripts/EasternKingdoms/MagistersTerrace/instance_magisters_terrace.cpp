/*
REWRITTEN BY XINEF
*/

#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "magisters_terrace.h"

class instance_magisters_terrace : public InstanceMapScript
{
public:
    instance_magisters_terrace() : InstanceMapScript("instance_magisters_terrace", 585) { }

    struct instance_magisters_terrace_InstanceMapScript : public InstanceScript
    {
        instance_magisters_terrace_InstanceMapScript(Map* map) : InstanceScript(map) { }

        uint32 Encounter[MAX_ENCOUNTER];

        ObjectGuid VexallusDoorGUID;
        ObjectGuid SelinDoorGUID;
        ObjectGuid SelinEncounterDoorGUID;
        ObjectGuid DelrissaDoorGUID;
        ObjectGuid KaelDoorGUID;
        ObjectGuid EscapeOrbGUID;

        ObjectGuid DelrissaGUID;
        ObjectGuid KaelGUID;

        void Initialize()
        {
            memset(&Encounter, 0, sizeof(Encounter));
            
            VexallusDoorGUID.Clear();;
            SelinDoorGUID.Clear();
            SelinEncounterDoorGUID.Clear();
            DelrissaDoorGUID.Clear();
            KaelDoorGUID.Clear();
            EscapeOrbGUID.Clear();

            DelrissaGUID.Clear();
            KaelGUID.Clear();
        }

        bool IsEncounterInProgress() const
        {
            for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                if (Encounter[i] == IN_PROGRESS)
                    return true;
            return false;
        }

        uint32 GetData(uint32 identifier) const
        {
            switch (identifier)
            {
                case DATA_SELIN_EVENT:
                case DATA_VEXALLUS_EVENT:
                case DATA_DELRISSA_EVENT:
                case DATA_KAELTHAS_EVENT:
                    return Encounter[identifier];
            }
            return 0;
        }

        void SetData(uint32 identifier, uint32 data)
        {
            switch (identifier)
            {
                case DATA_SELIN_EVENT:
                    HandleGameObject(SelinDoorGUID, data == DONE);
                    HandleGameObject(SelinEncounterDoorGUID, data != IN_PROGRESS);
                    Encounter[identifier] = data;
                    break;
                case DATA_VEXALLUS_EVENT:
                    if (data == DONE)
                        HandleGameObject(VexallusDoorGUID, true);
                    Encounter[identifier] = data;
                    break;
                case DATA_DELRISSA_EVENT:
                    if (data == DONE)
                        HandleGameObject(DelrissaDoorGUID, true);
                    Encounter[identifier] = data;
                    break;
                case DATA_KAELTHAS_EVENT:
                    HandleGameObject(KaelDoorGUID, data != IN_PROGRESS);
                    if (data == DONE)
                        if (GameObject* escapeOrb = instance->GetGameObject(EscapeOrbGUID))
                            escapeOrb->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    Encounter[identifier] = data;
                    break;
            }

            SaveToDB();
        }

        void OnCreatureCreate(Creature* creature)
        {
            switch (creature->GetEntry())
            {
                case NPC_DELRISSA:
                    DelrissaGUID.Set(creature->GetGUID());
                    break;
                case NPC_KAEL_THAS:
                    KaelGUID.Set(creature->GetGUID());
                    break;
                case NPC_PHOENIX:
                case NPC_PHOENIX_EGG:
                    if (Creature* kael = instance->GetCreature(KaelGUID))
                        kael->AI()->JustSummoned(creature);
                    break;
            }
        }

        void OnGameObjectCreate(GameObject* go)
        {
            switch (go->GetEntry())
            {
                case GO_SELIN_DOOR:

                    SelinDoorGUID.Set(go->GetGUID());
                    if (GetData(DATA_SELIN_EVENT) == DONE)
                        HandleGameObject(SelinDoorGUID, true, go); 
                    break;
                case GO_SELIN_ENCOUNTER_DOOR:
                    SelinEncounterDoorGUID.Set(go->GetGUID());
                    break;

                case GO_VEXALLUS_DOOR:

                    VexallusDoorGUID = go->GetGUID();
                    if (GetData(DATA_VEXALLUS_EVENT) == DONE)
                        HandleGameObject(VexallusDoorGUID, true, go); 
                    break;
                
                case GO_DELRISSA_DOOR:

                    DelrissaDoorGUID.Set(go->GetGUID());
                    if (GetData(DATA_DELRISSA_EVENT) == DONE)
                        HandleGameObject(DelrissaDoorGUID, true, go); 
                    break;
                case GO_KAEL_DOOR:
                    KaelDoorGUID.Set(go->GetGUID());
                    break;
                case GO_ESCAPE_ORB:
                    if (GetData(DATA_KAELTHAS_EVENT) == DONE)
                        go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    EscapeOrbGUID.Set(go->GetGUID());
                    break;
            }
        }

        std::string GetSaveData()
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << Encounter[0] << ' ' << Encounter[1] << ' ' << Encounter[2] << ' ' << Encounter[3];

            OUT_SAVE_INST_DATA_COMPLETE;
            return saveStream.str();
        }

        void Load(const char* str)
        {
            if (!str)
            {
                OUT_LOAD_INST_DATA_FAIL;
                return;
            }

            OUT_LOAD_INST_DATA(str);

            std::istringstream loadStream(str);

            for (uint32 i = 0; i < MAX_ENCOUNTER; ++i)
            {
                uint32 tmpState;
                loadStream >> tmpState;
                if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
                    tmpState = NOT_STARTED;
                SetData(i, tmpState);
            }

            OUT_LOAD_INST_DATA_COMPLETE;
        }

        ObjectGuid GetGuidData(uint32 identifier) const
        {
            switch (identifier)
            {
                case NPC_DELRISSA:
                    return DelrissaGUID;
            }
            return ObjectGuid::Empty;
        }
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_magisters_terrace_InstanceMapScript(map);
    }
};

void AddSC_instance_magisters_terrace()
{
    new instance_magisters_terrace();
}
