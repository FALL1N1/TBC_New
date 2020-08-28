#include "ScriptMgr.h"
#include "karazhan.h"
#include "ScriptedCreature.h"

enum Spells
{
    SPELL_REPENTANCE    = 29511,
    SPELL_HOLYFIRE      = 29522,
    SPELL_HOLYWRATH     = 32445,
    SPELL_HOLYGROUND    = 29523,
    SPELL_BERSERK       = 26662,
};

enum Yells
{
    SAY_AGGRO           = 0,
    SAY_SLAY            = 1,
    SAY_REPENTANCE      = 2,
    SAY_DEATH           = 3
};

enum Events
{
    EVENT_REPENTANCE    = 1,
    EVENT_HOLYFIRE      = 2,
    EVENT_HOLYWRATH     = 3,
    EVENT_ENRAGE        = 4,
    EVENT_HOLYGROUND    = 5,
};

class boss_maiden_of_virtue : public CreatureScript
{
public:
    boss_maiden_of_virtue() : CreatureScript("boss_maiden_of_virtue") { }

    struct boss_maiden_of_virtueAI : public BossAI
    {
        boss_maiden_of_virtueAI(Creature* creature) : BossAI(creature, DATA_MAIDEN_OF_VIRTUE) { }

        void KilledUnit(Unit* /*Victim*/) override
        {
            if (roll_chance_i(50))
                Talk(SAY_SLAY);
        }

        void JustDied(Unit* /*killer*/) override
        {
            Talk(SAY_DEATH);
            _JustDied();
        }

        void JustEngagedWith(Unit* who) override
        {
            BossAI::JustEngagedWith(who);
            Talk(SAY_AGGRO);

            events.ScheduleEvent(EVENT_REPENTANCE, 28s, 32s);
            events.ScheduleEvent(EVENT_HOLYFIRE, 12s, 20s);
            events.ScheduleEvent(EVENT_HOLYWRATH, 25s, 35s);
            events.ScheduleEvent(EVENT_HOLYGROUND, 25s, 35s);
            events.ScheduleEvent(EVENT_ENRAGE, 10min);
        }

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
                    case EVENT_REPENTANCE:
                        DoCastVictim(SPELL_REPENTANCE);
                        Talk(SAY_REPENTANCE);
                        events.Repeat(Seconds(35));
                        break;
                    case EVENT_HOLYFIRE:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50, true))
                            DoCast(target, SPELL_HOLYFIRE);
                        events.Repeat(Seconds(12), Seconds(20));
                        break;
                    case EVENT_HOLYWRATH:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 80, true))
                            DoCast(target, SPELL_HOLYWRATH);
                        events.Repeat(Seconds(15), Seconds(25));
                        break;
                    case EVENT_ENRAGE:
                        DoCastSelf(SPELL_BERSERK, true);
                        break;
                    case EVENT_HOLYGROUND:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 80, true, true)) { 
                            me->CastSpell(target, SPELL_HOLYGROUND);
                        }
                        events.Repeat(Seconds(15), Seconds(25)); 
                        break;
                    default:
                        break;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return GetKarazhanAI<boss_maiden_of_virtueAI>(creature);
    }
};

void AddSC_boss_maiden_of_virtue()
{
    new boss_maiden_of_virtue();
}
