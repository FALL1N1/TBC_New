#include "ScriptMgr.h"
#include "Creature.h"
#include "GameObject.h"
#include "InstanceScript.h"
#include "karazhan.h"
#include "Map.h"

/*
0  - Attumen + Midnight (optional)
1  - Moroes
2  - Maiden of Virtue (optional)
3  - Hyakiss the Lurker /  Rokad the Ravager  / Shadikith the Glider
4  - Opera Event
5  - Curator
6  - Shade of Aran (optional)
7  - Terestian Illhoof (optional)
8  - Netherspite (optional)
9  - Chess Event
10 - Prince Malchezzar
11 - Nightbane
*/

const Position OptionalSpawn[] =
{
    { -10960.981445f, -1940.138428f, 46.178097f, 4.12f  }, // Hyakiss the Lurker
    { -10945.769531f, -2040.153320f, 49.474438f, 0.077f }, // Shadikith the Glider
    { -10899.903320f, -2085.573730f, 49.474449f, 1.38f  }  // Rokad the Ravager
};

class instance_karazhan : public InstanceMapScript
{
public:
    instance_karazhan() : InstanceMapScript(KZScriptName, 532) { }

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_karazhan_InstanceMapScript(map);
    }

    struct instance_karazhan_InstanceMapScript : public InstanceScript
    {
        instance_karazhan_InstanceMapScript(Map* map) : InstanceScript(map)
        {
            SetHeaders(DataHeader);
            SetBossNumber(EncounterCount);

            // 1 - OZ, 2 - HOOD, 3 - RAJ, this never gets altered.
            OperaEvent = urand(EVENT_OZ, EVENT_RAJ);
            OzDeathCount = 0;
            OptionalBossCount = 0;
        }

        void Initialize()
        {
            ChessTeam = 0;
            ChessGamePhase = CHESS_PHASE_NOT_STARTED;
        }

        void OnCreatureCreate(Creature* creature) override
        {
            switch (creature->GetEntry())
            {
                case NPC_KILREK:
                    KilrekGUID = creature->GetGUID();
                    break;
                case NPC_TERESTIAN_ILLHOOF:
                    TerestianGUID = creature->GetGUID();
                    break;
                case NPC_MOROES:
                    MoroesGUID = creature->GetGUID();
                    break;
                case NPC_NIGHTBANE:
                    NightbaneGUID = creature->GetGUID();
                    break;
                case NPC_PAWN_H:
                case NPC_KNIGHT_H:
                case NPC_QUEEN_H:
                case NPC_BISHOP_H:
                case NPC_ROOK_H:
                case NPC_KING_H:
                case NPC_PAWN_A:
                case NPC_KNIGHT_A:
                case NPC_QUEEN_A:
                case NPC_BISHOP_A:
                case NPC_ROOK_A:
                case NPC_KING_A:
                    ChessPiecesGUID.insert(creature->GetGUID());
                    creature->SetHealth(creature->GetMaxHealth());
                    break;
                case 22521:
                    MedivhCheatFiresGUID.insert(creature->GetGUID());
                    break;
                case 16816:
                    ImageGUID = creature->GetGUID();
                    break;
                default:
                    break;
            }
        }

        void OnUnitDeath(Unit* unit) override
        {
            Creature* creature = unit->ToCreature();
            if (!creature)
                return;

            switch (creature->GetEntry())
            {
                case NPC_COLDMIST_WIDOW:
                case NPC_COLDMIST_STALKER:
                case NPC_SHADOWBAT:
                case NPC_VAMPIRIC_SHADOWBAT:
                case NPC_GREATER_SHADOWBAT:
                case NPC_PHASE_HOUND:
                case NPC_DREADBEAST:
                case NPC_SHADOWBEAST:
                    if (GetBossState(DATA_OPTIONAL_BOSS) == TO_BE_DECIDED)
                    {
                        ++OptionalBossCount;
                        if (OptionalBossCount == OPTIONAL_BOSS_REQUIRED_DEATH_COUNT)
                        {
                            switch (urand(NPC_HYAKISS_THE_LURKER, NPC_ROKAD_THE_RAVAGER))
                            {
                                case NPC_HYAKISS_THE_LURKER:
                                    instance->SummonCreature(NPC_HYAKISS_THE_LURKER, OptionalSpawn[0]);
                                    break;
                                case NPC_SHADIKITH_THE_GLIDER:
                                    instance->SummonCreature(NPC_SHADIKITH_THE_GLIDER, OptionalSpawn[1]);
                                    break;
                                case NPC_ROKAD_THE_RAVAGER:
                                    instance->SummonCreature(NPC_ROKAD_THE_RAVAGER, OptionalSpawn[2]);
                                    break;
                            }
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        void SetData(uint32 type, uint32 data) override
        {
            switch (type)
            {
                case DATA_OPERA_OZ_DEATHCOUNT:
                    if (data == SPECIAL)
                        ++OzDeathCount;
                    else if (data == IN_PROGRESS)
                        OzDeathCount = 0;
                    break;
                case DATA_CHESS_EVENT:
                    if (data == FAIL || data == DONE || data == NOT_STARTED)
                        DoRemoveAurasDueToSpellOnPlayers(SPELL_GAME_IN_SESSION);
                    else if (data == IN_PROGRESS)
                        DoCastSpellOnPlayers(SPELL_GAME_IN_SESSION);
                    break;
                case CHESS_EVENT_TEAM:
                    ChessTeam = data;
                    break;
                case DATA_CHESS_REINIT_PIECES:
                    for (GuidSet::const_iterator i = ChessPiecesGUID.begin(); i != ChessPiecesGUID.end(); ++i)
                    {
                        if (Creature* piece = instance->GetCreature(*i))
                        {
                            piece->RemoveCharmedBy(piece->GetCharmer());
                            piece->AttackStop();
                            piece->SetDeathState(JUST_RESPAWNED);
                            piece->SetHealth(piece->GetMaxHealth());
                            piece->AI()->DoAction(ACTION_PIECE_RESET);
                            float x, y, z, o;
                            piece->GetHomePosition(x, y, z, o);
                            piece->NearTeleportTo(x, y, z, o);
                            piece->AI()->DoAction(ACTION_PIECE_RESET_ORIENTATION);
                            piece->RemoveAurasDueToSpell(SPELL_HAND_OF_MEDIVH);
                            piece->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        }
                    }

                    for (GuidSet::const_iterator i = MedivhCheatFiresGUID.begin(); i != MedivhCheatFiresGUID.end(); ++i)
                        if (Creature* fire = instance->GetCreature(*i))
                            fire->DespawnOrUnsummon();

                    MedivhCheatFiresGUID.clear();
                    break;
                case DATA_CHESS_CHECK_PIECES_ALIVE:
                    for (GuidSet::const_iterator i = ChessPiecesGUID.begin(); i != ChessPiecesGUID.end(); ++i)
                    {
                        if (Creature* piece = instance->GetCreature(*i))
                        {
                            if (!piece->IsAlive())
                            {
                                piece->SetDeathState(JUST_RESPAWNED);
                                piece->SetHealth(piece->GetMaxHealth());
                            }
                        }
                    }
                    break;
                case DATA_CHESS_GAME_PHASE:
                    ChessGamePhase = data;
                    break;
            }
        }

        bool SetBossState(uint32 type, EncounterState state) override
        {
            if (!InstanceScript::SetBossState(type, state))
                return false;

            switch (type)
            {
                case DATA_OPERA_PERFORMANCE:
                    if (state == DONE)
                    {
                        HandleGameObject(StageDoorLeftGUID, true);
                        HandleGameObject(StageDoorRightGUID, true);
                        if (GameObject* sideEntrance = instance->GetGameObject(SideEntranceDoor))
                            sideEntrance->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED);
                        // @todo
                        //UpdateEncounterStateForKilledCreature(16812);
                    }
                    break; 
                default:
                    break;
            }

            return true;
        } 

        void OnPlayerEnter(Player* player)
        {
            if (GetData(DATA_CHESS_EVENT) != IN_PROGRESS)
                player->RemoveAurasDueToSpell(SPELL_GAME_IN_SESSION);

            player->RemoveAurasDueToSpell(SPELL_CONTROL_PIECE);
        }

        void OnGameObjectCreate(GameObject* go) override
        {
            switch (go->GetEntry())
            {
                case GO_STAGE_CURTAIN:
                    CurtainGUID = go->GetGUID();
                    break;
                case GO_STAGE_DOOR_LEFT:
                    StageDoorLeftGUID = go->GetGUID();
                    if (GetBossState(DATA_OPERA_PERFORMANCE) == DONE)
                        go->SetGoState(GO_STATE_ACTIVE);
                    break;
                case GO_STAGE_DOOR_RIGHT:
                    StageDoorRightGUID = go->GetGUID();
                    if (GetBossState(DATA_OPERA_PERFORMANCE) == DONE)
                        go->SetGoState(GO_STATE_ACTIVE);
                    break;
                case GO_PRIVATE_LIBRARY_DOOR:
                    LibraryDoor = go->GetGUID();
                    break;
                case GO_MASSIVE_DOOR:
                    MassiveDoor = go->GetGUID();
                    break;
                case GO_GAMESMAN_HALL_DOOR:
                    GamesmansDoor = go->GetGUID();
                    break;
                case GO_GAMESMAN_HALL_EXIT_DOOR:
                    GamesmansExitDoor = go->GetGUID();
                    break;
                case GO_NETHERSPACE_DOOR:
                    NetherspaceDoor = go->GetGUID();
                    break;
                case GO_MASTERS_TERRACE_DOOR:
                    MastersTerraceDoor[0] = go->GetGUID();
                    break;
                case GO_MASTERS_TERRACE_DOOR2:
                    MastersTerraceDoor[1] = go->GetGUID();
                    break;
                case GO_SIDE_ENTRANCE_DOOR:
                    SideEntranceDoor = go->GetGUID();
                    if (GetBossState(DATA_OPERA_PERFORMANCE) == DONE)
                        go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED);
                    else
                        go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED);
                    break;
                case GO_DUST_COVERED_CHEST:
                    DustCoveredChest = go->GetGUID();
                    break;
                case GO_BLACKENED_URN:
                    BlackenedUrnGUID = go->GetGUID();
                    break;
            }

            switch (OperaEvent)
            {
                /// @todo Set Object visibilities for Opera based on performance
                case EVENT_OZ:
                    break;

                case EVENT_HOOD:
                    break;

                case EVENT_RAJ:
                    break;
            }
        }

        uint32 GetData(uint32 type) const override
        {
            switch (type)
            {
                case DATA_OPERA_PERFORMANCE:
                    return OperaEvent;
                case DATA_OPERA_OZ_DEATHCOUNT:
                    return OzDeathCount;
                case CHESS_EVENT_TEAM:
                    return ChessTeam;
                case DATA_CHESS_GAME_PHASE:
                    return ChessGamePhase;
            }

            return 0;
        }

        ObjectGuid GetGuidData(uint32 type) const override
        {
            switch (type)
            {
                case DATA_KILREK:
                    return KilrekGUID;
                case DATA_TERESTIAN:
                    return TerestianGUID;
                case DATA_MOROES:
                    return MoroesGUID;
                case DATA_NIGHTBANE:
                    return NightbaneGUID;
                case DATA_GO_STAGEDOORLEFT:
                    return StageDoorLeftGUID;
                case DATA_GO_STAGEDOORRIGHT:
                    return StageDoorRightGUID;
                case DATA_GO_CURTAINS:
                    return CurtainGUID;
                case DATA_GO_LIBRARY_DOOR:
                    return LibraryDoor;
                case DATA_GO_MASSIVE_DOOR:
                    return MassiveDoor;
                case DATA_GO_SIDE_ENTRANCE_DOOR:
                    return SideEntranceDoor;
                case DATA_GO_GAME_DOOR:
                    return GamesmansDoor;
                case DATA_GO_GAME_EXIT_DOOR:
                    return GamesmansExitDoor;
                case DATA_GO_NETHER_DOOR:
                    return NetherspaceDoor;
                case DATA_MASTERS_TERRACE_DOOR_1:
                    return MastersTerraceDoor[0];
                case DATA_MASTERS_TERRACE_DOOR_2:
                    return MastersTerraceDoor[1];
                case DATA_IMAGE_OF_MEDIVH:
                    return ImageGUID;
                case DATA_GO_BLACKENED_URN:
                    return BlackenedUrnGUID;
            }

            return ObjectGuid::Empty;
        }

    private:
        uint32 OperaEvent;
        uint32 OzDeathCount;
        uint32 OptionalBossCount;
        ObjectGuid CurtainGUID;
        ObjectGuid StageDoorLeftGUID;
        ObjectGuid StageDoorRightGUID;
        ObjectGuid KilrekGUID;
        ObjectGuid TerestianGUID;
        ObjectGuid MoroesGUID;
        ObjectGuid NightbaneGUID;
        ObjectGuid LibraryDoor;                 // Door at Shade of Aran
        ObjectGuid MassiveDoor;                 // Door at Netherspite
        ObjectGuid SideEntranceDoor;            // Side Entrance
        ObjectGuid GamesmansDoor;               // Door before Chess
        ObjectGuid GamesmansExitDoor;           // Door after Chess
        ObjectGuid NetherspaceDoor;             // Door at Malchezaar
        ObjectGuid MastersTerraceDoor[2];
        ObjectGuid ImageGUID;
        ObjectGuid DustCoveredChest;
        ObjectGuid BlackenedUrnGUID;
        uint32 ChessTeam;
        uint32 ChessGamePhase;
        GuidSet ChessPiecesGUID;
        GuidSet MedivhCheatFiresGUID;
    };
};

void AddSC_instance_karazhan()
{
    new instance_karazhan();
}
