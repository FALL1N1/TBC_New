#ifndef DEF_STEAM_VAULT_H
#define DEF_STEAM_VAULT_H

#include "CreatureAIImpl.h"

#define SteamVaultScriptName "instance_steam_vault"
#define DataHeader "SV"

uint32 const EncounterCount = 3;

enum SVDataTypes
{
    DATA_HYDROMANCER_THESPIA   = 0,
    DATA_MEKGINEER_STEAMRIGGER = 1,
    DATA_WARLORD_KALITHRESH    = 2,

    // Additional Data
    DATA_DISTILLER              = 3,
    DATA_ACCESS_PANEL_HYDRO     = 4,
    DATA_ACCESS_PANEL_MEK       = 5,
    DATA_DOOR_CONTROLLER        = 6,
    DATA_MAIN_DOOR              = 7
};

enum SVCreatureIds
{
    NPC_HYDROMANCER_THESPIA      = 17797,
    NPC_MEKGINEER_STEAMRIGGER    = 17796,
    NPC_WARLORD_KALITHRESH       = 17798,
    NPC_COILFANG_DOOR_CONTROLLER = 20926
};

enum SVGameObjectIds
{
    GO_MAIN_CHAMBERS_DOOR = 183049,
    GO_ACCESS_PANEL_HYDRO = 184125,
    GO_ACCESS_PANEL_MEK   = 184126
};

enum SVMisc
{
    ACTION_OPEN_DOOR               = 1,
    CONTROLLER_TEXT_ACESS_USED     = 0,
    CONTROLLER_TEXT_MAIN_DOOR_OPEN = 1
};

template <class AI, class T>
inline AI* GetSteamVaultAI(T* obj)
{
    return GetInstanceAI<AI>(obj, SteamVaultScriptName);
}

#define RegisterSteamVaultAI(ai_name) RegisterCreatureAIWithFactory(ai_name, GetSteamVaultAI)

#endif
