
#ifndef DEF_OLD_HILLSBRAD_H
#define DEF_OLD_HILLSBRAD_H

#include "CreatureAIImpl.h"

#define OHScriptName "instance_old_hillsbrad"
#define DataHeader "OH"
  
#define TYPE_BARREL_DIVERSION 1
#define TYPE_THRALL_EVENT     2
#define TYPE_THRALL_PART1     3
#define TYPE_THRALL_PART2     4
#define TYPE_THRALL_PART3     5
#define TYPE_THRALL_PART4     6
#define DATA_THRALL           7
#define DATA_TARETHA          8
#define DATA_EPOCH            9
#define WORLD_STATE_OH        2436 
 
template <class AI, class T>
inline AI* GetOldHillsbradAI(T* obj)
{
    return GetInstanceAI<AI>(obj, OHScriptName);
}

#endif
