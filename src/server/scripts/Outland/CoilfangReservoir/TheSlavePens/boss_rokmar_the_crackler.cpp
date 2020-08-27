#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "the_slave_pens.h"

enum Spells
{
    SPELL_GRIEVOUS_WOUND            = 31956,
    SPELL_ENSNARING_MOSS            = 31948,
    SPELL_WATER_SPIT                = 35008,
    SPELL_FRENZY                    = 34970
};

enum Events
{
    EVENT_GRIEVOUS_WOUND            = 1,
    EVENT_ENSNARING_MOSS            = 2,
    EVENT_WATER_SPIT                = 3
};

class boss_rokmar_the_crackler : public CreatureScript
{
    public:
        boss_rokmar_the_crackler() : CreatureScript("boss_rokmar_the_crackler") { }

        struct boss_rokmar_the_cracklerAI : public BossAI
        {
            boss_rokmar_the_cracklerAI(Creature* creature) : BossAI(creature, DATA_MENNU_THE_BETRAYER)
            {
                Initialize();
            }

            void Initialize()
            {
                rokmarFrenzy = false;
            }

            void Reset() override
            {
                _Reset();
                Initialize();
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
            }

            void JustEngagedWith(Unit* who) override
            {
                BossAI::JustEngagedWith(who);
                events.ScheduleEvent(EVENT_GRIEVOUS_WOUND, 10s);
                events.ScheduleEvent(EVENT_ENSNARING_MOSS, 20s);
                events.ScheduleEvent(EVENT_WATER_SPIT, 14s);
            }

            void KilledUnit(Unit* /*victim*/) override { }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_GRIEVOUS_WOUND:
                            DoCastVictim(SPELL_GRIEVOUS_WOUND, true);
                            events.ScheduleEvent(EVENT_GRIEVOUS_WOUND, 20s, 30s);
                            break;
                        case EVENT_ENSNARING_MOSS:
                            DoCastAOE(SPELL_ENSNARING_MOSS);
                            events.ScheduleEvent(EVENT_ENSNARING_MOSS, 20s, 30s);
                            break;
                        case EVENT_WATER_SPIT:
                            DoCastAOE(SPELL_WATER_SPIT);
                            events.ScheduleEvent(EVENT_WATER_SPIT, 14s, 18s);
                            break;
                        default:
                            break;
                    }

                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        return;
                }

                if (HealthBelowPct(10) && !rokmarFrenzy)
                {
                    DoCast(me, SPELL_FRENZY);
                    rokmarFrenzy = true;
                }

                DoMeleeAttackIfReady();
            }

            private:
                bool   rokmarFrenzy;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetSlavePensAI<boss_rokmar_the_cracklerAI>(creature);
        }
};

void AddSC_boss_rokmar_the_crackler()
{
    new boss_rokmar_the_crackler();
}
