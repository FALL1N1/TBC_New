#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "InstanceScript.h"
#include "magtheridons_lair.h"

DoorData const doorData[] =
{
    { GO_MAGTHERIDON_DOORS,  TYPE_MAGTHERIDON,   DOOR_TYPE_ROOM },
    { 0,                0,              DOOR_TYPE_ROOM } // END
};

MinionData const minionData[] =
{
    { NPC_HELLFIRE_CHANNELER,   TYPE_MAGTHERIDON }
};

class instance_magtheridons_lair : public InstanceMapScript
{
    public:
        instance_magtheridons_lair() : InstanceMapScript("instance_magtheridons_lair", 544) { }

        struct instance_magtheridons_lair_InstanceMapScript : public InstanceScript
        {
            instance_magtheridons_lair_InstanceMapScript(Map* map) : InstanceScript(map)
            {
                SetBossNumber(MAX_ENCOUNTER);
                LoadDoorData(doorData);
                LoadMinionData(minionData);
            }

            void Initialize()
            {
                _wardersSet.clear();
                _cubesSet.clear();
                _columnSet.clear();
                _magtheridonGUID.Clear();
            }

            void OnCreatureCreate(Creature* creature)
            {
                InstanceScript::OnCreatureCreate(creature);
                switch (creature->GetEntry())
                {
                    case NPC_MAGTHERIDON:
                        _magtheridonGUID = creature->GetGUID();
                        break;
                    case NPC_HELLFIRE_CHANNELER:
                        AddMinion(creature, true);
                        break;
                    case NPC_HELLFIRE_WARDER:
                        _wardersSet.insert(creature->GetGUID());
                        break;
                }
            }

            void OnCreatureRemove(Creature* creature)
            {
                switch (creature->GetEntry())
                {
                    case NPC_HELLFIRE_CHANNELER:
                        AddMinion(creature, false);
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* go)
            {
                InstanceScript::OnGameObjectCreate(go);
                switch (go->GetEntry())
                {
                    case GO_MAGTHERIDON_DOORS:
                        AddDoor(go, true);
                        break;
                    case GO_MANTICRON_CUBE:
                        _cubesSet.insert(go->GetGUID());
                        break;
                    case GO_MAGTHERIDON_HALL:
                    case GO_MAGTHERIDON_COLUMN0:
                    case GO_MAGTHERIDON_COLUMN1:
                    case GO_MAGTHERIDON_COLUMN2:
                    case GO_MAGTHERIDON_COLUMN3:
                    case GO_MAGTHERIDON_COLUMN4:
                    case GO_MAGTHERIDON_COLUMN5:
                        _columnSet.insert(go->GetGUID());
                        break;
                }
            }

            void OnGameObjectRemove(GameObject* go)
            {
                switch (go->GetEntry())
                {
                    case GO_MAGTHERIDON_DOORS:
                        AddDoor(go, false);
                        break;
                    case GO_MANTICRON_CUBE:
                        _cubesSet.erase(go->GetGUID());
                        break;
                    case GO_MAGTHERIDON_HALL:
                    case GO_MAGTHERIDON_COLUMN0:
                    case GO_MAGTHERIDON_COLUMN1:
                    case GO_MAGTHERIDON_COLUMN2:
                    case GO_MAGTHERIDON_COLUMN3:
                    case GO_MAGTHERIDON_COLUMN4:
                    case GO_MAGTHERIDON_COLUMN5:
                        _columnSet.erase(go->GetGUID());
                        break;
                }
            }

            bool SetBossState(uint32 id, EncounterState state)
            {
                if (!InstanceScript::SetBossState(id, state))
                    return false;

                if (id == TYPE_MAGTHERIDON)
                {
                    if (state == IN_PROGRESS)
                    {
                        for (std::set<ObjectGuid>::const_iterator itr = _wardersSet.begin(); itr != _wardersSet.end(); ++itr)
                            if (Creature* warder = instance->GetCreature(*itr))
                                if (warder->IsAlive())
                                {
                                    warder->InterruptNonMeleeSpells(true);
                                    warder->SetInCombatWithZone();
                                }
                    }
                    else
                    {
                        for (std::set<ObjectGuid>::const_iterator itr = _cubesSet.begin(); itr != _cubesSet.end(); ++itr)
                            if (GameObject* cube = instance->GetGameObject(*itr))
                                cube->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);

                        if (state == NOT_STARTED)
                            SetData(DATA_COLLAPSE, GO_READY);
                    }
                }
                return true;
            }

            void SetData(uint32 type, uint32 data)
            {
                switch (type)
                {
                    case DATA_CHANNELER_COMBAT:
                        if (GetBossState(TYPE_MAGTHERIDON) != IN_PROGRESS)
                            if (Creature* magtheridon = instance->GetCreature(_magtheridonGUID))
                                magtheridon->SetInCombatWithZone();
                        break;
                    case DATA_ACTIVATE_CUBES:
                        for (std::set<ObjectGuid>::const_iterator itr = _cubesSet.begin(); itr != _cubesSet.end(); ++itr)
                            if (GameObject* cube = instance->GetGameObject(*itr))
                                cube->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                         
                        break;
                    case DATA_COLLAPSE:
                        for (std::set<ObjectGuid>::const_iterator itr = _columnSet.begin(); itr != _columnSet.end(); ++itr)
                            if (GameObject* column = instance->GetGameObject(*itr))
                                column->SetGoState(GOState(data));
                        break;
                }
            }
             
        private:
            ObjectGuid _magtheridonGUID;
            std::set<ObjectGuid> _wardersSet;
            std::set<ObjectGuid> _cubesSet;
            std::set<ObjectGuid> _columnSet;

        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const
        {
            return new instance_magtheridons_lair_InstanceMapScript(map);
        }
};

void AddSC_instance_magtheridons_lair()
{
    new instance_magtheridons_lair();
}

