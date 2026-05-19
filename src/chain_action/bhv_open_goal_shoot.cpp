// -*-c++-*-

/*!
  \file bhv_open_goal_shoot.cpp
  \brief force shoot when opponent goalie is out and the goal is open
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "bhv_open_goal_shoot.h"

#include "bhv_strict_check_shoot.h"
#include "body_force_shoot.h"
#include "field_analyzer.h"
#include "shoot_generator.h"

#include "../data_extractor/offensive_data_extractor.h"
#include "basic_actions/body_smart_kick.h"
#include "basic_actions/neck_turn_to_goalie_or_scan.h"

#include <rcsc/player/player_agent.h>
#include <rcsc/player/player_predicate.h>
#include <rcsc/player/debug_client.h>
#include <rcsc/common/logger.h>
#include <rcsc/common/server_param.h>

#include <algorithm>

using namespace rcsc;

namespace {

static const int VALID_OPPONENT_THRESHOLD = 10;
static const double MIN_SHOOT_SCORE = -100000.0;

bool
shouldAttemptOpenGoalShoot( const WorldModel & wm )
{
    if ( wm.gameMode().isPenaltyKickMode() )
    {
        return false;
    }

    if ( wm.gameMode().type() == GameMode::IndFreeKick_ )
    {
        return false;
    }

    if ( wm.time().stopped() > 0 )
    {
        return false;
    }

    if ( ! wm.self().isKickable() )
    {
        return false;
    }

    return true;
}

double
openShootAngle( const WorldModel & wm )
{
    const AbstractPlayerObject::Cont opponents
        = wm.getPlayers( new OpponentOrUnknownPlayerPredicate( wm ) );

    return FieldAnalyzer::can_shoot_from( true,
                                          wm.ball().pos(),
                                          opponents,
                                          VALID_OPPONENT_THRESHOLD );
}

bool
hasOneKickShootCourse( const WorldModel & wm,
                       const double opp_dist_thr )
{
    const ShootGenerator::Container & cont
        = ShootGenerator::instance().courses( wm, opp_dist_thr );

    for ( ShootGenerator::Container::const_iterator it = cont.begin();
          it != cont.end();
          ++it )
    {
        if ( it->kick_step_ == 1
             && it->score_ > MIN_SHOOT_SCORE )
        {
            return true;
        }
    }

    return false;
}

bool
hasOpenGoalShootTrigger( const WorldModel & wm )
{
    if ( openShootAngle( wm ) > 0.0 )
    {
        return true;
    }

    return hasOneKickShootCourse( wm, 0.0 );
}

bool
executeBestOneKickCourse( PlayerAgent * agent,
                          const WorldModel & wm,
                          const double opp_dist_thr )
{
    const ShootGenerator::Container & cont
        = ShootGenerator::instance().courses( wm, opp_dist_thr );

    ShootGenerator::Container::const_iterator best = cont.end();
    for ( ShootGenerator::Container::const_iterator it = cont.begin();
          it != cont.end();
          ++it )
    {
        if ( it->kick_step_ != 1
             || it->score_ <= MIN_SHOOT_SCORE )
        {
            continue;
        }

        if ( best == cont.end()
             || ShootGenerator::ScoreCmp()( *it, *best ) )
        {
            best = it;
        }
    }

    if ( best == cont.end() )
    {
        return false;
    }

    agent->debugClient().addMessage( "OpenGoal1Kick" );
    agent->debugClient().setTarget( best->target_point_ );

    Vector2D one_step_vel
        = KickTable::calc_max_velocity( ( best->target_point_ - wm.ball().pos() ).th(),
                                        wm.self().kickRate(),
                                        wm.ball().vel() );
    const double one_step_speed = one_step_vel.r();

    if ( one_step_speed > best->first_ball_speed_ * 0.99 )
    {
        if ( Body_SmartKick( best->target_point_,
                             one_step_speed,
                             one_step_speed * 0.99 - 0.0001,
                             1 ).execute( agent ) )
        {
            agent->setNeckAction( new Neck_TurnToGoalieOrScan( -1 ) );
            Bhv_StrictCheckShoot::time = wm.time().cycle();
            Bhv_StrictCheckShoot::target = best->target_point_;
            Bhv_StrictCheckShoot::speed = best->first_ball_speed_;
            return true;
        }
    }

    if ( Body_SmartKick( best->target_point_,
                         best->first_ball_speed_,
                         best->first_ball_speed_ * 0.99,
                         1 ).execute( agent ) )
    {
        agent->setNeckAction( new Neck_TurnToGoalieOrScan( -1 ) );
        Bhv_StrictCheckShoot::time = wm.time().cycle();
        Bhv_StrictCheckShoot::target = best->target_point_;
        Bhv_StrictCheckShoot::speed = best->first_ball_speed_;
        return true;
    }

    return false;
}

} // namespace

/*-------------------------------------------------------------------*/
/*!

 */
bool
Bhv_OpenGoalShoot::execute( PlayerAgent * agent )
{
    const WorldModel & wm
        = OffensiveDataExtractor::i().option.output_worldMode == FULLSTATE
        ? agent->fullstateWorld()
        : agent->world();

    if ( ! shouldAttemptOpenGoalShoot( wm ) )
    {
        return false;
    }

    if ( ! FieldAnalyzer::isTheirGoalieOutOfGoal( wm ) )
    {
        return false;
    }

    if ( ! hasOpenGoalShootTrigger( wm ) )
    {
        return false;
    }

    dlog.addText( Logger::SHOOT,
                  __FILE__": open goal shoot trigger" );
    agent->debugClient().addMessage( "OpenGoal" );

    const double relaxed_opp_dist_thr
        = FieldAnalyzer::isTitasDaRobotica( wm ) ? -0.8 : 0.0;

    if ( Bhv_StrictCheckShoot( relaxed_opp_dist_thr ).execute( agent ) )
    {
        return true;
    }

    if ( executeBestOneKickCourse( agent, wm, relaxed_opp_dist_thr ) )
    {
        return true;
    }

    if ( openShootAngle( wm ) > 0.0 )
    {
        return Body_ForceShoot().execute( agent );
    }

    return false;
}
