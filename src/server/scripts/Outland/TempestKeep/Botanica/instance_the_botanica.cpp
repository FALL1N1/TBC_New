/*
REWRITTEN BY XINEF
*/

#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "the_botanica.h"

class instance_the_botanica : public InstanceMapScript
{
    public:
        instance_the_botanica() : InstanceMapScript("instance_the_botanica", 553) { }

        struct instance_the_botanica_InstanceMapScript : public InstanceScript
        {
            instance_the_botanica_InstanceMapScript(Map* map) : InstanceScript(map)
            {
                SetBossNumber(MAX_ENCOUNTER);
            }

            bool SetBossState(uint32 type, EncounterState state)
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                return true;
            }

        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const
        {
            return new instance_the_botanica_InstanceMapScript(map);
        }
};

class spell_botanica_call_of_the_falcon : public SpellScriptLoader
{
public:
    spell_botanica_call_of_the_falcon() : SpellScriptLoader("spell_botanica_call_of_the_falcon") { }

    class spell_botanica_call_of_the_falcon_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_botanica_call_of_the_falcon_AuraScript)

        bool Load()
        {
            _falconSet.clear();
            return true;
        }
        
        void HandleEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            std::list<Creature*> creatureList;
            GetUnitOwner()->GetCreaturesWithEntryInRange(creatureList, 80.0f, NPC_BLOODFALCON);
            for (std::list<Creature*>::const_iterator itr = creatureList.begin(); itr != creatureList.end(); ++itr)
            {
                (*itr)->GetThreatManager().TauntUpdate();
                (*itr)->GetThreatManager().AddThreat(GetUnitOwner(), 10000000.0f);
                _falconSet.insert((*itr)->GetGUID());
            }
        }

        void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            for (std::set<ObjectGuid>::const_iterator itr = _falconSet.begin(); itr != _falconSet.end(); ++itr)
                if (Creature* falcon = ObjectAccessor::GetCreature(*GetUnitOwner(), *itr))
                {
                    falcon->GetThreatManager().TauntUpdate();
                    falcon->GetThreatManager().AddThreat(GetUnitOwner(), -10000000.0f);
                }
        }

        void Register()
        {
            OnEffectApply += AuraEffectApplyFn(spell_botanica_call_of_the_falcon_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            OnEffectRemove += AuraEffectRemoveFn(spell_botanica_call_of_the_falcon_AuraScript::HandleEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }

        private:
            std::set<ObjectGuid> _falconSet;
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_botanica_call_of_the_falcon_AuraScript();
    }
};

class spell_botanica_shift_form : public SpellScriptLoader
{
    public:
        spell_botanica_shift_form() : SpellScriptLoader("spell_botanica_shift_form") { }

        class spell_botanica_shift_form_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_botanica_shift_form_AuraScript);

            bool Load()
            {
                _lastSchool = 0;
                _lastForm = 0;
                _swapTime = 0;
                return true;
            }

            bool CheckProc(ProcEventInfo& eventInfo)
            {
                if (SpellInfo const* spellInfo = eventInfo.GetDamageInfo()->GetSpellInfo())
                {
                    if ((spellInfo->GetSchoolMask() & _lastSchool) && _swapTime > time(NULL))
                        return false;

                    uint32 form = 0;
                    switch (GetFirstSchoolInMask(spellInfo->GetSchoolMask()))
                    {
                        case SPELL_SCHOOL_FIRE:        form = SPELL_FIRE_FORM;        break;
                        case SPELL_SCHOOL_FROST:    form = SPELL_FROST_FORM;    break;
                        case SPELL_SCHOOL_ARCANE:    form = SPELL_ARCANE_FORM;    break;
                        case SPELL_SCHOOL_SHADOW:    form = SPELL_SHADOW_FORM;    break;
                    }

                    if (form)
                    {
                        _swapTime = time(NULL) + 6;
                        _lastSchool = spellInfo->GetSchoolMask();
                        GetUnitOwner()->RemoveAurasDueToSpell(_lastForm);
                        _lastForm = form;
                        GetUnitOwner()->CastSpell(GetUnitOwner(), _lastForm, true);
                    }
                }

                return false;
            }

            void Register()
            {
                DoCheckProc += AuraCheckProcFn(spell_botanica_shift_form_AuraScript::CheckProc);
            }

            private:
                uint32 _lastSchool;
                uint32 _lastForm;
                uint32 _swapTime;
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_botanica_shift_form_AuraScript();
        }
};

void AddSC_instance_the_botanica()
{
    new instance_the_botanica();
    new spell_botanica_call_of_the_falcon();
    new spell_botanica_shift_form();
}
