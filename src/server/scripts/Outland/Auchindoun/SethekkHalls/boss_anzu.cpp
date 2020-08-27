#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "sethekk_halls.h"

enum Anzu
{
    SAY_ANZU_INTRO1 = 0,
    SAY_ANZU_INTRO2 = 1,
    SAY_SUMMON = 2,

    SPELL_PARALYZING_SCREECH = 40184,
    SPELL_SPELL_BOMB = 40303,
    SPELL_CYCLONE = 40321,
    SPELL_BANISH_SELF = 42354,
    SPELL_SHADOWFORM = 40973,

    EVENT_SPELL_SCREECH = 1,
    EVENT_SPELL_BOMB = 2,
    EVENT_SPELL_CYCLONE = 3,
    EVENT_ANZU_HEALTH1 = 4,
    EVENT_ANZU_HEALTH2 = 5
};

class boss_anzu : public CreatureScript
{
public:
    boss_anzu() : CreatureScript("boss_anzu") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_anzuAI(creature);
    }

    struct boss_anzuAI : public ScriptedAI
    {
        boss_anzuAI(Creature* creature) : ScriptedAI(creature), summons(me)
        {
            talkTimer = 1;
            me->SetUInt32Value(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->AddAura(SPELL_SHADOWFORM, me);
        }

        EventMap events;
        SummonList summons;
        uint32 talkTimer;

        void Reset()
        {
            summons.DespawnAll();
            if (InstanceScript* instance = me->GetInstanceScript())
                if (instance->GetData(DATA_ANZU) != DONE)
                    instance->SetData(DATA_ANZU, NOT_STARTED);
        }

        void JustSummoned(Creature* summon)
        {
            summons.Summon(summon);
            summon->AI()->AttackStart(me->GetVictim());
        }

        void SummonedCreatureDies(Creature* summon, Unit*)
        {
            summons.Despawn(summon);
            summons.RemoveNotExisting();
            if (summons.empty())
                me->RemoveAurasDueToSpell(SPELL_BANISH_SELF);
        }

        void EnterCombat(Unit* /*who*/)
        {
            events.Reset();
            events.ScheduleEvent(EVENT_SPELL_SCREECH, 14000);
            events.ScheduleEvent(EVENT_SPELL_BOMB, 5000);
            events.ScheduleEvent(EVENT_SPELL_CYCLONE, 8000);
            events.ScheduleEvent(EVENT_ANZU_HEALTH1, 2000);
            events.ScheduleEvent(EVENT_ANZU_HEALTH2, 2001);

            if (InstanceScript* instance = me->GetInstanceScript())
                instance->SetData(DATA_ANZU, IN_PROGRESS);
        }

        void JustDied(Unit* /*killer*/)
        {
            if (InstanceScript* instance = me->GetInstanceScript())
                instance->SetData(DATA_ANZU, DONE);
        }

        void SummonBroods()
        {
            Talk(SAY_SUMMON);
            me->CastSpell(me, SPELL_BANISH_SELF, true);
            for (uint8 i = 0; i < 5; ++i)
                me->SummonCreature(23132 /*NPC_BROOD_OF_ANZU*/, me->GetPositionX() + 20 * cos((float)i), me->GetPositionY() + 20 * sin((float)i), me->GetPositionZ() + 25.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
        }

        void UpdateAI(uint32 diff)
        {
            if (talkTimer)
            {
                talkTimer += diff;
                if (talkTimer >= 1000 && talkTimer < 10000)
                {
                    Talk(SAY_ANZU_INTRO1);
                    talkTimer = 10000;
                }
                else if (talkTimer >= 16000)
                {
                    me->SetUInt32Value(UNIT_FIELD_FLAGS, 0);
                    me->RemoveAurasDueToSpell(SPELL_SHADOWFORM);
                    Talk(SAY_ANZU_INTRO2);
                    talkTimer = 0;
                }
            }

            if (!UpdateVictim())
                return;

            events.Update(diff);
            if (me->HasUnitState(UNIT_STATE_CASTING | UNIT_STATE_STUNNED))
                return;

            switch (events.GetEvent())
            {
            case EVENT_SPELL_SCREECH:
                me->CastSpell(me, SPELL_PARALYZING_SCREECH, false);
                events.Repeat(23000);
                events.DelayEvents(3000);
                break;
            case EVENT_SPELL_BOMB:
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0f, true))
                    me->CastSpell(target, SPELL_SPELL_BOMB, false);
                events.Repeat(urand(16000, 24500));
                events.DelayEvents(3000);
                break;
            case EVENT_SPELL_CYCLONE:
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 45.0f, true))
                    me->CastSpell(target, SPELL_CYCLONE, false);
                events.Repeat(urand(22000, 27000));
                events.DelayEvents(3000);
                break;
            case EVENT_ANZU_HEALTH1:
                if (me->HealthBelowPct(66))
                {
                    SummonBroods();
                    events.PopEvent();
                    events.DelayEvents(10000);
                    return;
                }
                events.Repeat(1000);
                break;
            case EVENT_ANZU_HEALTH2:
                if (me->HealthBelowPct(33))
                {
                    SummonBroods();
                    events.PopEvent();
                    events.DelayEvents(10000);
                    return;
                }
                events.Repeat(1000);
                break;
            }

            DoMeleeAttackIfReady();
        }
    };
};

void AddSC_boss_anzu()
{
    new boss_anzu();
}
