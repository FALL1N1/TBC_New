#ifndef the_underbog_h__
#define the_underbog_h__

#include "CreatureAIImpl.h"

#define TheUndebogScriptName "instance_the_underbog"

template <class AI, class T>
inline AI* GetTheUnderbogAI(T* obj)
{
    return GetInstanceAI<AI>(obj, TheUndebogScriptName);
}

#endif // the_underbog_h__
