#include "ScriptMgr.h"
#include "Creature.h"
#include "InstanceScript.h"
#include "Map.h"
#include "the_slave_pens.h"

MinionData const creatureData[] =
{
    { NPC_AHUNE,                    DATA_AHUNE             },
    { NPC_FROZEN_CORE,              DATA_FROZEN_CORE       },
    { NPC_AHUNE_LOC_BUNNY,          DATA_AHUNE_BUNNY       },
    { NPC_SHAMAN_BONFIRE_BUNNY_000, DATA_BONFIRE_BUNNY_000 },
    { NPC_SHAMAN_BONFIRE_BUNNY_001, DATA_BONFIRE_BUNNY_001 },
    { NPC_SHAMAN_BONFIRE_BUNNY_002, DATA_BONFIRE_BUNNY_002 },
    { NPC_SHAMAN_BEAM_BUNNY_000,    DATA_BEAM_BUNNY_000    },
    { NPC_SHAMAN_BEAM_BUNNY_001,    DATA_BEAM_BUNNY_001    },
    { NPC_SHAMAN_BEAM_BUNNY_002,    DATA_BEAM_BUNNY_002    },
    { NPC_LUMA_SKYMOTHER,           DATA_LUMA_SKYMOTHER    },
    { 0,                            0,                     }
};

class instance_the_slave_pens : public InstanceMapScript
{
public:
    instance_the_slave_pens() : InstanceMapScript(SPScriptName, 547) { }

    struct instance_the_slave_pens_InstanceMapScript : public InstanceScript
    {
        instance_the_slave_pens_InstanceMapScript(Map* map) : InstanceScript(map)
        {
            counter = DATA_FLAMECALLER_000;
            LoadMinionData(creatureData);
        }

        void OnCreatureCreate(Creature* creature) override
        {
            InstanceScript::OnCreatureCreate(creature);

            if (creature->GetEntry() == NPC_EARTHEN_RING_FLAMECALLER)
            {
                switch (counter)
                {
                    case DATA_FLAMECALLER_000:
                        FlameCallerGUIDs[0] = creature->GetGUID();
                        break;
                    case DATA_FLAMECALLER_001:
                        FlameCallerGUIDs[1] = creature->GetGUID();
                        break;
                    case DATA_FLAMECALLER_002:
                        FlameCallerGUIDs[2] = creature->GetGUID();
                        break;
                    default:
                        break;
                }
                ++counter;
            }
        }

        ObjectGuid GetGuidData(uint32 type) const override
        {
            switch (type)
            {
                case DATA_FLAMECALLER_000:
                    return FlameCallerGUIDs[0];
                case DATA_FLAMECALLER_001:
                    return FlameCallerGUIDs[1];
                case DATA_FLAMECALLER_002:
                    return FlameCallerGUIDs[2];
                default:
                    break;
            }
            return ObjectGuid::Empty;
        }

    protected:
        ObjectGuid FlameCallerGUIDs[3];
        uint8 counter;
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_the_slave_pens_InstanceMapScript(map);
    }
};

void AddSC_instance_the_slave_pens()
{
    new instance_the_slave_pens();
}
