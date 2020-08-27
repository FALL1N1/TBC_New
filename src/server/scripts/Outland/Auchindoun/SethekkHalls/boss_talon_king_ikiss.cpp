#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "sethekk_halls.h"

enum TailonkingIkiss
{
    SAY_INTRO                    = 0,
    SAY_AGGRO                    = 1,
    SAY_SLAY                    = 2,
    SAY_DEATH                    = 3,
    EMOTE_ARCANE_EXP            = 4,

    SPELL_BLINK                    = 38194,
    SPELL_BLINK_TELEPORT        = 38203,
    SPELL_MANA_SHIELD            = 38151,
    SPELL_ARCANE_BUBBLE            = 9438,
    SPELL_SLOW                    = 35032,
    SPELL_POLYMORPH_N            = 38245, // Difficulty data
    SPELL_POLYMORPH_H            = 43309,
    SPELL_ARCANE_VOLLEY_N        = 35059, // Difficulty data
    SPELL_ARCANE_VOLLEY_H        = 40424,
    SPELL_ARCANE_EXPLOSION_N    = 38197, // Difficulty data
    SPELL_ARCANE_EXPLOSION_H    = 40425,

    EVENT_SPELL_BLINK            = 1,
    EVENT_SPELL_POLYMORPH        = 2,
    EVENT_SPELL_SLOW            = 3,
    EVENT_SPELL_ARCANE_VOLLEY    = 4,
    EVENT_SPELL_ARCANE_EXPLO    = 5,
    EVENT_HEALTH_CHECK            = 6,
    EVENT_SPELL_BLINK_2            = 7
};

class boss_talon_king_ikiss : public CreatureScript
{
public:
    boss_talon_king_ikiss() : CreatureScript("boss_talon_king_ikiss") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_talon_king_ikissAI (creature);
    }

    struct boss_talon_king_ikissAI : public ScriptedAI
    {
        boss_talon_king_ikissAI(Creature* creature) : ScriptedAI(creature)
        {
        }

        EventMap events;
        bool _spoken;

        void Reset()
        {
            _spoken = false;
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (!_spoken && who->GetTypeId() == TYPEID_PLAYER)
            {
                Talk(SAY_INTRO);
                _spoken = true;
            }

            ScriptedAI::MoveInLineOfSight(who);
        }

        void EnterCombat(Unit* /*who*/)
        {
            Talk(SAY_AGGRO);

            events.ScheduleEvent(EVENT_SPELL_BLINK, 35000);
            events.ScheduleEvent(EVENT_SPELL_ARCANE_VOLLEY, 5000);
            events.ScheduleEvent(EVENT_SPELL_POLYMORPH, 8000);
            events.ScheduleEvent(EVENT_HEALTH_CHECK, 2000);
            if (IsHeroic())
                events.ScheduleEvent(EVENT_SPELL_SLOW, urand(15000, 25000));
        }

        void JustDied(Unit* /*killer*/)
        {
            Talk(SAY_DEATH);

            if (InstanceScript* instance = me->GetInstanceScript())
                instance->SetData(DATA_IKISSDOOREVENT, DONE);
        }

        void KilledUnit(Unit* /*victim*/)
        {
            if (urand(0,1))
                Talk(SAY_SLAY);
        }

        void UpdateAI(uint32 diff)
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);
            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            switch (events.GetEvent())
            {
                case EVENT_SPELL_ARCANE_VOLLEY:
                    me->CastSpell(me, SPELL_ARCANE_VOLLEY_N, false);
                    events.Repeat(urand(7000, 12000));
                    break;
                case EVENT_SPELL_POLYMORPH:
                    if (Unit* target = (IsHeroic() ? SelectTarget(SELECT_TARGET_RANDOM, 0) : SelectTarget(SELECT_TARGET_MAXTHREAT, 1)))
                        me->CastSpell(target, SPELL_POLYMORPH_N, false);
                    events.Repeat(urand(15000, 17500));
                    break;
                case EVENT_SPELL_SLOW:
                    me->CastSpell(me, SPELL_SLOW, false);
                    events.Repeat(urand(15000, 30000));
                    break;
                case EVENT_HEALTH_CHECK:
                    if (me->HealthBelowPct(20))
                    {
                        me->CastSpell(me, SPELL_MANA_SHIELD, false);
                        events.PopEvent();
                        return;
                    }
                    events.Repeat(1000);
                    break;
                case EVENT_SPELL_BLINK:
                    Talk(EMOTE_ARCANE_EXP);
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                    {
                        me->CastSpell(target, SPELL_BLINK, false);
                        me->NearTeleportTo(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), target->GetOrientation());

                        DoCast(target, SPELL_BLINK_TELEPORT);
                    }
                    events.Repeat(urand(35000, 40000));
                    events.DelayEvents(500);
                    events.ScheduleEvent(EVENT_SPELL_BLINK_2, 0);
                    return;
                case EVENT_SPELL_BLINK_2:
                    me->CastSpell(me, SPELL_ARCANE_EXPLOSION_N, false);
                    me->CastSpell(me, SPELL_ARCANE_BUBBLE, true);
                    events.PopEvent();
                    break;
            }

            DoMeleeAttackIfReady();
        }
    };
};
 
void AddSC_boss_talon_king_ikiss()
{
    new boss_talon_king_ikiss(); 
}
