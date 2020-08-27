#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "sethekk_halls.h"

enum Says
{
    SAY_SUMMON                  = 0,
    SAY_AGGRO                   = 1,
    SAY_SLAY                    = 2,
    SAY_DEATH                   = 3
};

enum Spells
{
    SPELL_FROST_SHOCK           = 21401, //37865
    SPELL_FLAME_SHOCK           = 34354,
    SPELL_SHADOW_SHOCK          = 30138,
    SPELL_ARCANE_SHOCK          = 37132,

    SPELL_CHAIN_LIGHTNING       = 15659, //15305

    SPELL_SUMMON_SYTH_FIRE      = 33537,                   // Spawns 19203
    SPELL_SUMMON_SYTH_ARCANE    = 33538,                   // Spawns 19205
    SPELL_SUMMON_SYTH_FROST     = 33539,                   // Spawns 19204
    SPELL_SUMMON_SYTH_SHADOW    = 33540,                   // Spawns 19206

    SPELL_FLAME_BUFFET          = 33526,
    SPELL_ARCANE_BUFFET         = 33527,
    SPELL_FROST_BUFFET          = 33528,
    SPELL_SHADOW_BUFFET         = 33529
};

enum Events
{
    EVENT_FLAME_SHOCK           = 1,
    EVENT_ARCANE_SHOCK          = 2,
    EVENT_FROST_SHOCK           = 3,
    EVENT_SHADOW_SHOCK          = 4,
    EVENT_CHAIN_LIGHTNING       = 5
};

enum Lakka
{
    NPC_LAKKA         = 18956,
    SAY_LAKKA_FREE    = 1
};

class boss_darkweaver_syth : public CreatureScript
{
public:
    boss_darkweaver_syth() : CreatureScript("boss_darkweaver_syth") { }

    struct boss_darkweaver_sythAI : public BossAI
    {
        boss_darkweaver_sythAI(Creature* creature) : BossAI(creature, DATA_DARKWEAVER_SYTH)
        {
            Initialize();
        }

        void Initialize()
        {
            _summon90 = false;
            _summon50 = false;
            _summon10 = false;
        }

        void Reset() override
        {
            Initialize();
            _Reset();
        }

        void JustEngagedWith(Unit* who) override
        {
            BossAI::JustEngagedWith(who);
            events.ScheduleEvent(EVENT_FLAME_SHOCK, 2s);
            events.ScheduleEvent(EVENT_ARCANE_SHOCK, 4s);
            events.ScheduleEvent(EVENT_FROST_SHOCK, 6s);
            events.ScheduleEvent(EVENT_SHADOW_SHOCK, 8s);
            events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, 15s);

            Talk(SAY_AGGRO);
        }

        void JustDied(Unit* /*killer*/) override
        {
            _JustDied();
            Talk(SAY_DEATH);

            if (Creature* lakka = me->FindNearestCreature(NPC_LAKKA, 500.0f, true))
                lakka->AI()->Talk(SAY_LAKKA_FREE);
        }

        void KilledUnit(Unit* who) override
        {
            if (who->GetTypeId() == TYPEID_PLAYER)
                Talk(SAY_SLAY);
        }

        void JustSummoned(Creature* summoned) override
        {
            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                summoned->AI()->AttackStart(target);

            summons.Summon(summoned);
        }

        void DamageTaken(Unit* /*attacker*/, uint32& damage) override
        {
            if (me->HealthBelowPctDamaged(90, damage) && !_summon90)
            {
                SythSummoning();
                _summon90 = true;
            }

            if (me->HealthBelowPctDamaged(50, damage) && !_summon50)
            {
                SythSummoning();
                _summon50 = true;
            }

            if (me->HealthBelowPctDamaged(10, damage) && !_summon10)
            {
                SythSummoning();
                _summon10 = true;
            }
        }

        void SythSummoning()
        {
            Talk(SAY_SUMMON);

            if (me->IsNonMeleeSpellCast(false))
                me->InterruptNonMeleeSpells(false);

            DoCast(me, SPELL_SUMMON_SYTH_ARCANE, true);   //front
            DoCast(me, SPELL_SUMMON_SYTH_FIRE, true);     //back
            DoCast(me, SPELL_SUMMON_SYTH_FROST, true);    //left
            DoCast(me, SPELL_SUMMON_SYTH_SHADOW, true);   //right
        }

        void ExecuteEvent(uint32 eventId) override
        {
            switch (eventId)
            {
                case EVENT_FLAME_SHOCK:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                        DoCast(target, SPELL_FLAME_SHOCK);
                    events.ScheduleEvent(EVENT_FLAME_SHOCK, 10s, 15s);
                    break;
                case EVENT_ARCANE_SHOCK:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                        DoCast(target, SPELL_ARCANE_SHOCK);
                    events.ScheduleEvent(EVENT_ARCANE_SHOCK, 10s, 15s);
                    break;
                case EVENT_FROST_SHOCK:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                        DoCast(target, SPELL_FROST_SHOCK);
                    events.ScheduleEvent(EVENT_FROST_SHOCK, 10s, 15s);
                    break;
                case EVENT_SHADOW_SHOCK:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                        DoCast(target, SPELL_SHADOW_SHOCK);
                    events.ScheduleEvent(EVENT_SHADOW_SHOCK, 10s, 15s);
                    break;
                case EVENT_CHAIN_LIGHTNING:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                        DoCast(target, SPELL_CHAIN_LIGHTNING);
                    events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, 25s);
                    break;
                default:
                    break;
            }
        }

        private:
            bool _summon90;
            bool _summon50;
            bool _summon10;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return GetSethekkHallsAI<boss_darkweaver_sythAI>(creature);
    }
};

void AddSC_boss_darkweaver_syth()
{
    new boss_darkweaver_syth();
}
