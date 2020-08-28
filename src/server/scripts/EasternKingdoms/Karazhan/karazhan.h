
#ifndef DEF_KARAZHAN_H
#define DEF_KARAZHAN_H

#include "CreatureAIImpl.h"

#define KZScriptName "instance_karazhan"
#define DataHeader "KZ"

uint32 const EncounterCount = 12;

enum KZDataTypes
{
    DATA_ATTUMEN                    = 0,
    DATA_MOROES                     = 1,
    DATA_MAIDEN_OF_VIRTUE           = 2,
    DATA_OPTIONAL_BOSS              = 3,
    DATA_OPERA_PERFORMANCE          = 4,
    DATA_CURATOR                    = 5,
    DATA_ARAN                       = 6,
    DATA_TERESTIAN                  = 7,
    DATA_NETHERSPITE                = 8,
    DATA_CHESS_EVENT                = 9,
    DATA_MALCHEZZAR                 = 10,
    DATA_NIGHTBANE                  = 11,

    DATA_OPERA_OZ_DEATHCOUNT        = 14,

    DATA_KILREK                     = 15,
    DATA_GO_CURTAINS                = 18,
    DATA_GO_STAGEDOORLEFT           = 19,
    DATA_GO_STAGEDOORRIGHT          = 20,
    DATA_GO_LIBRARY_DOOR            = 21,
    DATA_GO_MASSIVE_DOOR            = 22,
    DATA_GO_NETHER_DOOR             = 23,
    DATA_GO_GAME_DOOR               = 24,
    DATA_GO_GAME_EXIT_DOOR          = 25,

    DATA_IMAGE_OF_MEDIVH            = 26,
    DATA_MASTERS_TERRACE_DOOR_1     = 27,
    DATA_MASTERS_TERRACE_DOOR_2     = 28,
    DATA_GO_SIDE_ENTRANCE_DOOR      = 29,
    DATA_GO_BLACKENED_URN           = 30,

    DATA_CHESS_ECHO_OF_MEDIVH       = 31,
    CHESS_EVENT_TEAM                = 32,
    DATA_CHESS_REINIT_PIECES        = 33,
    DATA_CHESS_CHECK_PIECES_ALIVE   = 34,
    DATA_IMAGE_OF_MEDIVH_NIGHTBANE  = 35,
    DATA_CHESS_GAME_PHASE           = 36,
};

enum KZOperaEvents
{
    EVENT_OZ                        = 1,
    EVENT_HOOD                      = 2,
    EVENT_RAJ                       = 3
};

enum KarazhanChessNpcs
{
    NPC_MEDIVH   = 16816,
    NPC_PAWN_H   = 17469,
    NPC_PAWN_A   = 17211,
    NPC_KNIGHT_H = 21748,
    NPC_KNIGHT_A = 21664,
    NPC_QUEEN_H  = 21750,
    NPC_QUEEN_A  = 21683,
    NPC_BISHOP_H = 21747,
    NPC_BISHOP_A = 21682,
    NPC_ROOK_H   = 21726,
    NPC_ROOK_A   = 21160,
    NPC_KING_H   = 21752,
    NPC_KING_A   = 21684
};

enum KarazhanChessGamePhase
{
    CHESS_PHASE_NOT_STARTED = 0,
    CHESS_PHASE_PVE_WARMUP = 1, // Medivh has been spoken too but king isn't controlled yet
    CHESS_PHASE_INPROGRESS_PVE = 2,
    CHESS_PHASE_FAILED = 4,
    CHESS_PHASE_PVE_FINISHED = 5,
    CHESS_PHASE_PVP_WARMUP = 6,
    CHESS_PHASE_INPROGRESS_PVP = 7  // Get back to PVE_FINISHED after that
};

enum EchoOfMedivhGossipOptions
{
    MEDIVH_GOSSIP_START_PVE = 1,
    MEDIVH_GOSSIP_RESTART,
    MEDIVH_GOSSIP_START_PVP
};

enum KarazhanChessSpells
{
    SPELL_BISHOP_HEAL_H = 37456,
    SPELL_BISHOP_HEAL_A = 37455,
    SPELL_MOVE_1 = 37146, // 8y
    SPELL_MOVE_2 = 30012, // Unlimited
    SPELL_MOVE_3 = 37144, // 15y
    SPELL_MOVE_4 = 37148, // 20y
    SPELL_MOVE_5 = 37151, // 8y
    SPELL_MOVE_6 = 37152, // 8y
    SPELL_MOVE_7 = 37153, // 8y
    SPELL_CHANGE_FACING = 30284,
    SPELL_MOVE_MARKER = 32261,
    SPELL_CONTROL_PIECE = 30019,
    SPELL_RECENTLY_INGAME = 30529,
    SPELL_HAND_OF_MEDIVH = 39339, // 1st cheat: AOE spell burn cell under enemy chesspieces.
    SPELL_FURY_OF_MEDIVH = 39383, // 2nd cheat: Berserk own chesspieces.
    SPELL_FURY_OF_MEDIVH_FIRE = 39345,
    SPELL_GAME_IN_SESSION = 39331,
    SPELL_MOVE_COOLDOWN = 30543,
    // 3rd cheat: set own creatures to max health
};

enum KarazhanChessMisc
{
    ACTION_PIECE_RESET = 1,
    ACTION_PIECE_RESET_ORIENTATION,

    A_FACTION               = 1690,
    H_FACTION               = 1689,

    GOSSIP_POSSES           = 20028,

    NPC_ATTACK_RADIUS       = 6,
    AGGRO_RANGE             = 6,
    NPC_CHESS_MOVE_TRIGGER  = 22519,
    SEARCH_RANGE            = 5,
    DUST_COVERED_CHEST      = 185119,

    BOTH                    = 536, // HORDE  ALLIANCE
};

enum KarazhanChessTeam
{
    DEAD_ALLIANCE = 0,
    DEAD_HORDE = 1,
};

enum KarazhanChessOrientationType
{
    ORI_SE = 0,    // Horde start
    ORI_SW = 1,
    ORI_NW = 2,    // Alliance start
    ORI_NE = 3,
};

enum KZMiscCreatures
{
    NPC_HYAKISS_THE_LURKER          = 16179,
    NPC_ROKAD_THE_RAVAGER           = 16181,
    NPC_SHADIKITH_THE_GLIDER        = 16180,
    NPC_TERESTIAN_ILLHOOF           = 15688,
    NPC_MOROES                      = 15687,
    NPC_NIGHTBANE                   = 17225,
    NPC_ATTUMEN_UNMOUNTED           = 15550,
    NPC_ATTUMEN_MOUNTED             = 16152,
    NPC_MIDNIGHT                    = 16151,

    // Trash
    NPC_COLDMIST_WIDOW              = 16171,
    NPC_COLDMIST_STALKER            = 16170,
    NPC_SHADOWBAT                   = 16173,
    NPC_VAMPIRIC_SHADOWBAT          = 16175,
    NPC_GREATER_SHADOWBAT           = 16174,
    NPC_PHASE_HOUND                 = 16178,
    NPC_DREADBEAST                  = 16177,
    NPC_SHADOWBEAST                 = 16176,
    NPC_KILREK                      = 17229
};

enum KZGameObjectIds
{
    GO_STAGE_CURTAIN                = 183932,
    GO_STAGE_DOOR_LEFT              = 184278,
    GO_STAGE_DOOR_RIGHT             = 184279,
    GO_PRIVATE_LIBRARY_DOOR         = 184517,
    GO_MASSIVE_DOOR                 = 185521,
    GO_GAMESMAN_HALL_DOOR           = 184276,
    GO_GAMESMAN_HALL_EXIT_DOOR      = 184277,
    GO_NETHERSPACE_DOOR             = 185134,
    GO_MASTERS_TERRACE_DOOR         = 184274,
    GO_MASTERS_TERRACE_DOOR2        = 184280,
    GO_SIDE_ENTRANCE_DOOR           = 184275,
    GO_DUST_COVERED_CHEST           = 185119,
    GO_BLACKENED_URN                = 194092
};

enum KZMisc
{
    OPTIONAL_BOSS_REQUIRED_DEATH_COUNT = 50
};

template <class AI, class T>
inline AI* GetKarazhanAI(T* obj)
{
    return GetInstanceAI<AI>(obj, KZScriptName);
}

#endif
