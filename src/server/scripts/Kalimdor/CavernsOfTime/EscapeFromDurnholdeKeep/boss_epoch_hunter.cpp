/* ScriptData
SDName: Boss_Epoch_Hunter
SD%Complete: 60
SDComment: Missing spawns pre-event, missing speech to be coordinated with rest of escort event.
SDCategory: Caverns of Time, Old Hillsbrad Foothills
EndScriptData */

#include "ScriptPCH.h"
#include "old_hillsbrad.h"

#define SAY_ENTER1                  -1560013
#define SAY_ENTER2                  -1560014
#define SAY_ENTER3                  -1560015
#define SAY_AGGRO1                  -1560016
#define SAY_AGGRO2                  -1560017
#define SAY_SLAY1                   -1560018
#define SAY_SLAY2                   -1560019
#define SAY_BREATH1                 -1560020
#define SAY_BREATH2                 -1560021
#define SAY_DEATH                   -1560022

#define SPELL_SAND_BREATH           31914
#define SPELL_IMPENDING_DEATH       31916
#define SPELL_MAGIC_DISRUPTION_AURA 33834
#define SPELL_WING_BUFFET           31475

class boss_epoch_hunter : public CreatureScript
{
public:
    boss_epoch_hunter() : CreatureScript("boss_epoch_hunter") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new boss_epoch_hunterAI(pCreature);
    }

    struct boss_epoch_hunterAI : public ScriptedAI
    {
        boss_epoch_hunterAI(Creature* pCreature) : ScriptedAI(pCreature)
        {
            pInstance = pCreature->GetInstanceScript();
        }

        InstanceScript* pInstance;

        uint32 SandBreath_Timer;
        uint32 ImpendingDeath_Timer;
        uint32 WingBuffet_Timer;
        uint32 Mda_Timer;

        void Reset()
        {
            SandBreath_Timer = 8000 + rand()%8000;
            ImpendingDeath_Timer = 25000 + rand()%5000;
            WingBuffet_Timer = 35000;
            Mda_Timer = 40000;
        }

        void EnterCombat(Unit* /*pWho*/)
        {
            DoScriptText(RAND(SAY_AGGRO1, SAY_AGGRO2), me);
        }

        void KilledUnit(Unit* /*pVictim*/)
        {
            DoScriptText(RAND(SAY_SLAY1, SAY_SLAY2), me);
        }

        void JustDied(Unit* /*pVictim*/)
        {
            DoScriptText(SAY_DEATH, me);

            if(pInstance && pInstance->GetData(TYPE_THRALL_EVENT) == IN_PROGRESS)
                pInstance->SetData(TYPE_THRALL_PART4, DONE);
        }

        void UpdateAI(const uint32 diff)
        {
            //Return since we have no target
            if(!UpdateVictim())
                return;

            //Sand Breath
            if(SandBreath_Timer <= diff)
            { 
                if(me->IsNonMeleeSpellCast(false))
                    me->InterruptNonMeleeSpells(false);

                DoCast(me->GetVictim(), SPELL_SAND_BREATH);

                DoScriptText(RAND(SAY_BREATH1, SAY_BREATH2), me);

                SandBreath_Timer = 10000 + rand()%10000;
            } else SandBreath_Timer -= diff;

            if(ImpendingDeath_Timer <= diff)
            {
                DoCast(me->GetVictim(), SPELL_IMPENDING_DEATH);
                ImpendingDeath_Timer = 25000+rand()%5000;
            } else ImpendingDeath_Timer -= diff;

            if(WingBuffet_Timer <= diff)
            {
                if(Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                    DoCast(target, SPELL_WING_BUFFET);
                WingBuffet_Timer = 25000+rand()%10000;
            } else WingBuffet_Timer -= diff;

            if(Mda_Timer <= diff)
            {
                DoCast(me, SPELL_MAGIC_DISRUPTION_AURA);
                Mda_Timer = 15000;
            } else Mda_Timer -= diff;

            DoMeleeAttackIfReady();
        }
    };
};

void AddSC_boss_epoch_hunter()
{
    new boss_epoch_hunter;
}
