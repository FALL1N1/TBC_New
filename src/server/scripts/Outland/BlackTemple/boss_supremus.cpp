#include "ScriptMgr.h"
#include "black_temple.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h" 
#include "ScriptedCreature.h"

enum Supremus
{
    EMOTE_NEW_TARGET                    = 0,
    EMOTE_PUNCH_GROUND                  = 1,
    EMOTE_GROUND_CRACK                  = 2,

    SPELL_SNARE_SELF                    = 41922,
    SPELL_MOLTEN_PUNCH                  = 40126,
    SPELL_HATEFUL_STRIKE                = 41926,
    SPELL_MOLTEN_FLAME                  = 40980,
    SPELL_VOLCANIC_ERUPTION             = 40276,
    SPELL_VOLCANIC_ERUPTION_TRIGGER     = 40117,
    SPELL_BERSERK                       = 45078,
    SPELL_CHARGE                        = 41581,

    NPC_SUPREMUS_PUNCH_STALKER          = 23095,

    EVENT_SPELL_BERSERK                 = 1,
    EVENT_SPELL_HATEFUL_STRIKE          = 2,
    EVENT_SPELL_MOLTEN_FLAMES           = 3,
    EVENT_SWITCH_PHASE                  = 4,
    EVENT_SPELL_VOLCANIC_ERUPTION       = 5,
    EVENT_SWITCH_TARGET                 = 6,
    EVENT_CHECK_DIST                    = 7,

    EVENT_GROUP_ABILITIES               = 1,
};

class boss_supremus : public CreatureScript
{
    public:
        boss_supremus() : CreatureScript("boss_supremus") { }

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetBlackTempleAI<boss_supremusAI>(creature);
        }

        struct boss_supremusAI : public BossAI
        {
            boss_supremusAI(Creature* creature) : BossAI(creature, DATA_SUPREMUS) { }

            void Reset()
            {
                BossAI::Reset();
            }

            void JustEngagedWith(Unit* who) override
            {
                BossAI::JustEngagedWith(who);

                SchedulePhase(false);
                events.ScheduleEvent(EVENT_SPELL_BERSERK, 900000);
                events.ScheduleEvent(EVENT_SPELL_MOLTEN_FLAMES, 4000);
            }

            void SchedulePhase(bool run)
            {
                events.CancelEventGroup(EVENT_GROUP_ABILITIES);
                events.ScheduleEvent(EVENT_SWITCH_PHASE, 60000);
                me->GetThreatManager().ResetAllThreat();

                if (!run)
                {
                    events.ScheduleEvent(EVENT_SPELL_HATEFUL_STRIKE, 5000, EVENT_GROUP_ABILITIES);
                    me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, false);
                    me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, false);
                    me->RemoveAurasDueToSpell(SPELL_SNARE_SELF);
                }
                else
                {
                    events.ScheduleEvent(EVENT_SPELL_VOLCANIC_ERUPTION, 5000, EVENT_GROUP_ABILITIES);
                    events.ScheduleEvent(EVENT_SWITCH_TARGET, 0, EVENT_GROUP_ABILITIES);
                    events.ScheduleEvent(EVENT_CHECK_DIST, 0, EVENT_GROUP_ABILITIES);
                    me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                    me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
                    me->CastSpell(me, SPELL_SNARE_SELF, true);
                }                
            }

            void JustDied(Unit* killer)
            {
                BossAI::JustDied(killer);
            }

            void JustSummoned(Creature* summon)
            {
                summons.Summon(summon);
                if (summon->GetEntry() == NPC_SUPREMUS_PUNCH_STALKER)
                {
                    summon->ToTempSummon()->InitStats(20000);
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true)) 
                        summon->GetMotionMaster()->MoveFollow(target, 0.0f, 0.0f, MOTION_SLOT_ACTIVE);
                }
                else
                    summon->CastSpell(summon, SPELL_VOLCANIC_ERUPTION_TRIGGER, true);
            }

            void SummonedCreatureDespawn(Creature* summon)
            {
                summons.Despawn(summon);
            }

            Unit* FindHatefulStrikeTarget()
            {
                uint32 health = 0;
                Unit* target = nullptr;

                for (auto* ref : me->GetThreatManager().GetUnsortedThreatList())
                {
                    Unit* unit = ref->GetVictim();
                    if (me->IsWithinMeleeRange(unit))
                    {
                        if (unit->GetHealth() > health)
                        {
                            health = unit->GetHealth();
                            target = unit;
                        }
                    }
                }

                return target;
            }

            void UpdateAI(uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_SPELL_BERSERK:
                        me->CastSpell(me, SPELL_BERSERK, true);
                        break;
                    case EVENT_SPELL_HATEFUL_STRIKE:
                        if (Unit* target = FindHatefulStrikeTarget()) 
                            me->CastSpell(target, SPELL_HATEFUL_STRIKE, false);
                        events.ScheduleEvent(EVENT_SPELL_HATEFUL_STRIKE, urand(1500, 3000), EVENT_GROUP_ABILITIES);
                        break;
                    case EVENT_SPELL_MOLTEN_FLAMES:
                        me->CastSpell(me, SPELL_MOLTEN_PUNCH, false);
                        events.ScheduleEvent(EVENT_SPELL_MOLTEN_FLAMES, 20000, EVENT_GROUP_ABILITIES);
                        break;
                    case EVENT_SWITCH_PHASE:
                        SchedulePhase(!me->HasAura(SPELL_SNARE_SELF));
                        break;
                    case EVENT_SWITCH_TARGET:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                        {
                            me->GetThreatManager().ResetAllThreat();
                            me->GetThreatManager().AddThreat(target, 5000000.0f);
                            Talk(EMOTE_NEW_TARGET);
                        }
                        events.ScheduleEvent(EVENT_SWITCH_TARGET, 10000, EVENT_GROUP_ABILITIES);
                        break;
                    case EVENT_CHECK_DIST:
                        if (me->GetDistance(me->GetVictim()) > 40.0f)
                        {
                            Talk(EMOTE_PUNCH_GROUND);
                            me->CastSpell(me->GetVictim(), SPELL_CHARGE, true);
                            events.ScheduleEvent(EVENT_CHECK_DIST, 5000, EVENT_GROUP_ABILITIES);
                            break;
                        }
                        events.ScheduleEvent(EVENT_CHECK_DIST, 1, EVENT_GROUP_ABILITIES);
                        break;
                    case EVENT_SPELL_VOLCANIC_ERUPTION:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                        {
                            me->CastSpell(target, SPELL_VOLCANIC_ERUPTION, true);
                            Talk(EMOTE_GROUND_CRACK);
                        }
                        events.ScheduleEvent(EVENT_SPELL_VOLCANIC_ERUPTION, urand(10000, 18000), EVENT_GROUP_ABILITIES);
                        break;
                }

                DoMeleeAttackIfReady();
                EnterEvadeIfOutOfCombatArea();
            }
                        
            bool CheckEvadeIfOutOfCombatArea() const
            {
                return me->GetPositionX() < 565 || me->GetPositionX() > 865 || me->GetPositionY() < 545 || me->GetPositionY() > 1000;
            }
        };
};

class molten_flame : public CreatureScript
{
public:
    molten_flame() : CreatureScript("molten_flame") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new molten_flameAI(creature);
    }

    struct molten_flameAI : public ScriptedAI
    {
        molten_flameAI(Creature* creature) : ScriptedAI(creature) {}

        void InitializeAI()
        {
            float x, y, z;
            me->GetNearPoint(me, x, y, z, 1, 100);
            me->GetMotionMaster()->MovePoint(0, x, y, z);
            me->SetVisible(false);
            me->CastSpell(me, SPELL_MOLTEN_FLAME, true);
        }
    };
};

class npc_volcano : public CreatureScript
{
public:
    npc_volcano() : CreatureScript("npc_volcano") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_volcanoAI(creature);
    }

    struct npc_volcanoAI : public ScriptedAI
    {
        npc_volcanoAI(Creature* creature) : ScriptedAI(creature) {}

        void Reset()
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE); 
            DoCast(me, SPELL_VOLCANIC_ERUPTION);
            me->SetReactState(REACT_PASSIVE);
            wait = 3000;
        }
        uint32 wait;

        void EnterCombat(Unit* /*who*/) {}

        void MoveInLineOfSight(Unit* /*who*/) {}

        void DoAction(const int32 /*info*/)
        {
            me->RemoveAura(SPELL_VOLCANIC_ERUPTION);
        }

        void UpdateAI(const uint32 diff)
        {
            if (wait <= diff)//wait 3secs before casting
            {
                DoCast(me, SPELL_VOLCANIC_ERUPTION);
                wait = 60000;
            }
            else wait -= diff;
        }
    };
};

void AddSC_boss_supremus()
{
    new boss_supremus();
    new molten_flame();
    new npc_volcano();
}
