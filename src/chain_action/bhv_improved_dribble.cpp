// -*-c++-*-

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "bhv_improved_dribble.h"
#include "bhv_normal_dribble.h"

#include "basic_actions/body_smart_kick.h"
#include "basic_actions/body_kick_one_step.h"
#include "../data_extractor/offensive_data_extractor.h"
#include "../neck/neck_decision.h"
#include "../lib/pimentao_verde_say_message_builder.h"
#include "basic_actions/basic_actions.h"

#include <rcsc/player/player_agent.h>
#include <rcsc/common/server_param.h>
#include <rcsc/common/logger.h>

#include <algorithm>
#include <cstring>

using namespace rcsc;

const char PIMENTAO_IMPROVED_DRIBBLE_DESC[] = "pv_improved_dribble";

Bhv_ImprovedDribble::Bhv_ImprovedDribble( const CooperativeAction & action,
                                          NeckAction::Ptr neck,
                                          ViewAction::Ptr view )
    : M_target_point( action.targetPoint() ),
      M_intermediate_pos( action.intermediatePoint() ),
      M_first_ball_speed( action.firstBallSpeed() ),
      M_first_turn_moment( action.firstTurnMoment() ),
      M_first_dash_power( action.firstDashPower() ),
      M_first_dash_angle( action.firstDashAngle() ),
      M_total_step( action.durationStep() ),
      M_kick_step( action.kickCount() ),
      M_turn_step( action.turnCount() ),
      M_dash_step( action.dashCount() ),
      M_neck_action( neck ),
      M_view_action( view ),
      M_dec( action.description() ),
      M_dash_angle( action.dribble_dash_angle() ),
      M_player_target_point( action.getPlayerTargetPoint() )
{
    (void)M_player_target_point;
    (void)M_dash_angle;
    if ( action.category() != CooperativeAction::Dribble
         || std::strcmp( action.description(), PIMENTAO_IMPROVED_DRIBBLE_DESC ) != 0 )
    {
        M_target_point = Vector2D::INVALIDATED;
        M_total_step = 0;
    }
}

bool
Bhv_ImprovedDribble::execute( PlayerAgent * agent )
{
    const WorldModel & wm = OffensiveDataExtractor::i().option.output_worldMode == FULLSTATE
                                ? agent->fullstateWorld()
                                : agent->world();

    if ( ! wm.self().isKickable() )
    {
        dlog.addText( Logger::DRIBBLE,
                      __FILE__": (ImprovedDribble) not kickable" );
        return false;
    }

    if ( ! M_target_point.isValid()
         || M_total_step <= 0 )
    {
        dlog.addText( Logger::DRIBBLE,
                      __FILE__": (ImprovedDribble) invalid target or step" );
        return false;
    }

    const ServerParam & SP = ServerParam::i();

    if ( M_kick_step > 0 )
    {
        const Vector2D tar = M_target_point;

        Vector2D first_vel = tar - wm.ball().pos();
        first_vel.setLength( M_first_ball_speed );

        const double min_speed = std::max( 0.5, M_first_ball_speed * 0.86 );

        if ( Body_SmartKick( tar, M_first_ball_speed, min_speed, 2 ).execute( agent ) )
        {
            dlog.addText( Logger::DRIBBLE,
                          __FILE__": (ImprovedDribble) first touch: SmartKick" );
        }
        else if ( Body_KickOneStep( tar, M_first_ball_speed, false ).execute( agent ) )
        {
            dlog.addText( Logger::DRIBBLE,
                          __FILE__": (ImprovedDribble) first touch: KickOneStep" );
        }
        else
        {
            return false;
        }

        const Vector2D kick_accel = first_vel - wm.ball().vel();
        const double kick_power = kick_accel.r() / wm.self().kickRate();

        if ( kick_power > SP.maxPower() + 1.0e-5 )
        {
            dlog.addText( Logger::DRIBBLE,
                          __FILE__": (ImprovedDribble) kick power over max (info)" );
        }

        dlog.addCircle( Logger::DRIBBLE,
                        wm.ball().pos() + first_vel,
                        0.1,
                        "#00aa88" );
    }
    else if ( M_turn_step > 0 )
    {
        agent->doTurn( M_first_turn_moment );
    }
    else if ( M_dash_step > 0 )
    {
        agent->doDash( M_first_dash_power, M_first_dash_angle );
    }
    else
    {
        dlog.addText( Logger::DRIBBLE,
                      __FILE__": (ImprovedDribble) no kick/turn/dash steps" );
        return false;
    }

    agent->setIntention( pimentao::create_normal_dribble_tail_intention(
        M_target_point,
        M_turn_step,
        M_total_step - M_turn_step - 1,
        wm.time() ) );

    agent->addSayMessage( new DribbleMessage( M_target_point, M_total_step ) );

    if ( ! M_view_action )
    {
        if ( M_turn_step + M_dash_step >= 3 )
        {
            agent->setViewAction( new View_Normal() );
        }
    }
    else
    {
        agent->setViewAction( M_view_action->clone() );
    }

    if ( ! M_neck_action )
    {
        NeckDecisionWithBall().setNeck( agent, NeckDecisionType::dribbling );
    }
    else
    {
        agent->setNeckAction( M_neck_action->clone() );
    }

    (void)M_dec;
    return true;
}
