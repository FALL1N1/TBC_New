#ifndef DEF_MAGTHERIDONS_LAIR_H
#define DEF_MAGTHERIDONS_LAIR_H

#include "CreatureAIImpl.h"

#define MLScriptName "instance_magtheridons_lair"
#define DataHeader "ML"

uint32 const EncounterCount = 1;

enum DataTypes
{
    TYPE_MAGTHERIDON                = 0,
    MAX_ENCOUNTER                    = 1,

    DATA_CHANNELER_COMBAT            = 10,
    DATA_ACTIVATE_CUBES                = 11,
    DATA_COLLAPSE                    = 12
};

enum NpcIds
{
    NPC_MAGTHERIDON                    = 17257,
    NPC_HELLFIRE_CHANNELER            = 17256,
    NPC_HELLFIRE_WARDER                = 18829
};

enum GoIds
{
    GO_MAGTHERIDON_DOORS            = 183847,
    GO_MANTICRON_CUBE                = 181713,

    GO_MAGTHERIDON_HALL                = 184653,
    GO_MAGTHERIDON_COLUMN0            = 184634,
    GO_MAGTHERIDON_COLUMN1            = 184635,
    GO_MAGTHERIDON_COLUMN2            = 184636,
    GO_MAGTHERIDON_COLUMN3            = 184637,
    GO_MAGTHERIDON_COLUMN4            = 184638,
    GO_MAGTHERIDON_COLUMN5            = 184639
};

template <class AI, class T>
inline AI* GetMagtheridonsLairAI(T* obj)
{
    return GetInstanceAI<AI>(obj, MLScriptName);
}

#endif
