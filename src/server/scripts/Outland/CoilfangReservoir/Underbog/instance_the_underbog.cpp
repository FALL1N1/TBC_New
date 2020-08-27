#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "Map.h"
#include "the_underbog.h"

class instance_the_underbog : public InstanceMapScript
{
public:
    instance_the_underbog() : InstanceMapScript(TheUndebogScriptName, 546) { }

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_the_underbog_InstanceMapScript(map);
    }

    struct instance_the_underbog_InstanceMapScript : public InstanceScript
    {
        instance_the_underbog_InstanceMapScript(Map* map) : InstanceScript(map) { }
    };
};

void AddSC_instance_the_underbog()
{
    new instance_the_underbog();
}
