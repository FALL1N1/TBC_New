#include "ScriptPCH.h"

enum Yells
{
    SAY_INTRO       = 0,
    SAY_AGGRO       = 1,
    SAY_SURPREME    = 2,
    SAY_KILL        = 3,
    SAY_DEATH       = 4,
    EMOTE_FRENZY    = 5,
    SAY_RAND        = 6
};

enum Spells
{
    SPELL_SHADOWVOLLEY          = 32963,
    SPELL_CLEAVE                = 31779,
    SPELL_THUNDERCLAP           = 36706,
    SPELL_VOIDBOLT              = 39329,
    SPELL_MARKOFKAZZAK          = 32960,
    SPELL_MARKOFKAZZAK_DAMAGE   = 32961,
    SPELL_ENRAGE                = 32964,
    SPELL_CAPTURESOUL           = 32966,
    SPELL_TWISTEDREFLECTION     = 21063
};

enum Events
{
    EVENT_SHADOW_VOLLEY         = 1,
    EVENT_CLEAVE                = 2,
    EVENT_THUNDERCLAP           = 3,
    EVENT_VOIDBOLT              = 4,
    EVENT_MARK_OF_KAZZAK        = 5,
    EVENT_ENRAGE                = 6,
    EVENT_TWISTED_REFLECTION    = 7
};

class boss_doomlord_kazzak : public CreatureScript
{
    public:
        boss_doomlord_kazzak() : CreatureScript("boss_doomlord_kazzak") { }

        struct boss_doomlordkazzakAI : public ScriptedAI
        {
            boss_doomlordkazzakAI(Creature* pCreature): ScriptedAI(pCreature) { }

            void Reset()
            {
                _events.Reset();
                _events.ScheduleEvent(EVENT_SHADOW_VOLLEY, urand(6000, 10000));
                _events.ScheduleEvent(EVENT_CLEAVE, 7000);
                _events.ScheduleEvent(EVENT_THUNDERCLAP, urand(14000, 18000));
                _events.ScheduleEvent(EVENT_VOIDBOLT, 30000);
                _events.ScheduleEvent(EVENT_MARK_OF_KAZZAK, 25000);
                _events.ScheduleEvent(EVENT_ENRAGE, 60000);
                _events.ScheduleEvent(EVENT_TWISTED_REFLECTION, 33000);
            }

            void JustRespawned()
            {
                Talk(SAY_INTRO);
            }

            void EnterCombat(Unit* /*pWho*/)
            {
                Talk(SAY_AGGRO);
            }

            void KilledUnit(Unit* victim)
            {
                // When Kazzak kills a player (not pets/totems), he regens some health
                if(victim->GetTypeId() != TYPEID_PLAYER)
                    return;

                DoCast(me, SPELL_CAPTURESOUL);

                Talk(SAY_KILL);
            }

            void JustDied(Unit* /*pVictim*/)
            {
                Talk(SAY_DEATH);
            }

            void UpdateAI(uint32 const diff)
            {
                // Return since we have no target
                if(!UpdateVictim())
                    return;

                _events.Update(diff);

                if(me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while(uint32 eventId = _events.ExecuteEvent())
                {
                    switch(eventId)
                    {
                        case EVENT_SHADOW_VOLLEY:
                            DoCastVictim(SPELL_SHADOWVOLLEY);
                            _events.ScheduleEvent(EVENT_SHADOW_VOLLEY, urand(4000, 6000));
                            break;
                        case EVENT_CLEAVE:
                            DoCastVictim(SPELL_CLEAVE);
                            _events.ScheduleEvent(EVENT_CLEAVE, urand(8000, 12000));
                            break;
                        case EVENT_THUNDERCLAP:
                            DoCastVictim(SPELL_THUNDERCLAP);
                            _events.ScheduleEvent(EVENT_THUNDERCLAP, urand(10000, 14000));
                            break;
                        case EVENT_VOIDBOLT:
                            DoCastVictim(SPELL_VOIDBOLT);
                            _events.ScheduleEvent(EVENT_VOIDBOLT, urand(15000, 18000));
                            break;
                        case EVENT_MARK_OF_KAZZAK:
                            if(Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                                DoCast(target, SPELL_MARKOFKAZZAK);
                            _events.ScheduleEvent(EVENT_MARK_OF_KAZZAK, 20000);
                            break;
                        case EVENT_ENRAGE:
                            Talk(EMOTE_FRENZY);
                            DoCast(me, SPELL_ENRAGE);
                            _events.ScheduleEvent(EVENT_ENRAGE, 30000);
                            break;
                        case EVENT_TWISTED_REFLECTION:
                            if(Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                                DoCast(target, SPELL_TWISTEDREFLECTION);
                            _events.ScheduleEvent(EVENT_TWISTED_REFLECTION, 15000);
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new boss_doomlordkazzakAI(pCreature);
        }
};

void AddSC_boss_doomlordkazzak()
{
    new boss_doomlord_kazzak;
}
