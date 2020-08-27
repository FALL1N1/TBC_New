#ifndef SETHEKK_HALLS_H_
#define SETHEKK_HALLS_H_

#include "CreatureAIImpl.h"

#define SHScriptName "instance_sethekk_halls"
#define DataHeader "SH"

uint32 const EncounterCount             = 3;

enum eTypes
{
    DATA_IKISSDOOREVENT 	 = 1,
    DATA_ANZU 	 		     = 2,
	DATA_DARKWEAVER_SYTH	 = 3, 
};

enum eIds
{
    NPC_VOICE_OF_THE_RAVEN_GOD    = 21851,
    NPC_ANZU                      = 23035,
	NPC_BROOD_OF_ANZU             = 23132,

    GO_IKISS_DOOR                 = 177203,
    GO_THE_TALON_KINGS_COFFER     = 187372,
};

template <class AI, class T>
inline AI* GetSethekkHallsAI(T* obj)
{
    return GetInstanceAI<AI>(obj, SHScriptName);
}

#endif // SETHEKK_HALLS_H_
