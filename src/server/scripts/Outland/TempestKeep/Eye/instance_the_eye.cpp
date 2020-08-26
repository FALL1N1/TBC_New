#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "the_eye.h"

class instance_the_eye : public InstanceMapScript
{
    public:
        instance_the_eye() : InstanceMapScript("instance_the_eye", 550) { }

        struct instance_the_eye_InstanceMapScript : public InstanceScript
        {
            instance_the_eye_InstanceMapScript(Map* map) : InstanceScript(map) {}

            ObjectGuid ThaladredTheDarkenerGUID;
            ObjectGuid LordSanguinarGUID;
            ObjectGuid GrandAstromancerCapernianGUID;
            ObjectGuid MasterEngineerTelonicusGUID;
            ObjectGuid AlarGUID;
            ObjectGuid KaelthasGUID;
            ObjectGuid BridgeWindowGUID;
            ObjectGuid KaelStateRightGUID;
            ObjectGuid KaelStateLeftGUID;

            void Initialize()
            {
                SetBossNumber(MAX_ENCOUNTER);
                AlarGUID.Clear();
                KaelthasGUID.Clear();
                ThaladredTheDarkenerGUID.Clear();
                LordSanguinarGUID.Clear();
                GrandAstromancerCapernianGUID.Clear();
                MasterEngineerTelonicusGUID.Clear();
                BridgeWindowGUID.Clear();
                KaelStateRightGUID.Clear();
                KaelStateLeftGUID.Clear();
            }

            void OnCreatureCreate(Creature* creature)
            {
                InstanceScript::OnCreatureCreate(creature);

                /*switch (creature->GetEntry())
                {
                    case NPC_ALAR:
                        AlarGUID.Set(creature->GetGUID());
                        break;
                    case NPC_KAELTHAS:
                        KaelthasGUID.Set(creature->GetGUID());
                        break;
                    case NPC_THALADRED:
                        ThaladredTheDarkenerGUID.Set(creature->GetGUID());
                        break;
                    case NPC_TELONICUS:
                        MasterEngineerTelonicusGUID.Set(creature->GetGUID());
                        break;
                    case NPC_CAPERNIAN:
                        GrandAstromancerCapernianGUID.Set(creature->GetGUID());
                        break;
                    case NPC_LORD_SANGUINAR:
                        LordSanguinarGUID.Set(creature->GetGUID());
                        break;
                }*/
            }

            void OnGameObjectCreate(GameObject* gobject)
            {
                switch (gobject->GetEntry())
                {
                    case GO_BRIDGE_WINDOW:
                        BridgeWindowGUID = gobject->GetGUID();
                        break;
                    case GO_KAEL_STATUE_RIGHT:
                        KaelStateRightGUID = gobject->GetGUID();
                        break;
                    case GO_KAEL_STATUE_LEFT:
                        KaelStateLeftGUID = gobject->GetGUID();
                        break;
                }
            }

            uint64 GetData64(uint32 identifier) const
            {
                switch (identifier)
                {
                    case GO_BRIDGE_WINDOW:        return BridgeWindowGUID;
                    case GO_KAEL_STATUE_RIGHT:    return KaelStateRightGUID;
                    case GO_KAEL_STATUE_LEFT:    return KaelStateLeftGUID;
                    case NPC_ALAR:                return AlarGUID;
                    case NPC_KAELTHAS:            return KaelthasGUID;
                    case DATA_KAEL_ADVISOR1:    return ThaladredTheDarkenerGUID;
                    case DATA_KAEL_ADVISOR2:    return LordSanguinarGUID;
                    case DATA_KAEL_ADVISOR3:    return GrandAstromancerCapernianGUID;
                    case DATA_KAEL_ADVISOR4:    return MasterEngineerTelonicusGUID;
                }
                return 0;
            }  
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const
        {
            return new instance_the_eye_InstanceMapScript(map);
        }
};

class spell_the_eye_countercharge : public SpellScriptLoader
{
    public:
        spell_the_eye_countercharge() : SpellScriptLoader("spell_the_eye_countercharge") { }

        class spell_the_eye_counterchargeScript : public AuraScript
        {
            PrepareAuraScript(spell_the_eye_counterchargeScript);

            bool PrepareProc(ProcEventInfo& eventInfo)
            {
                // xinef: prevent charge drop
                PreventDefaultAction();
                return true;
            }

            void Register()
            {
                DoCheckProc += AuraCheckProcFn(spell_the_eye_counterchargeScript::PrepareProc);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_the_eye_counterchargeScript();
        }
};

void AddSC_instance_the_eye()
{
    new instance_the_eye();
    new spell_the_eye_countercharge();
}
