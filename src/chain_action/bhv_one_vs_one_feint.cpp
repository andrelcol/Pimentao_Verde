// -*-c++-*-

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "bhv_one_vs_one_feint.h"
#include "bhv_normal_dribble.h"
#include "action_chain_holder.h"
#include "action_chain_graph.h"
#include "cooperative_action.h"

#include "basic_actions/body_smart_kick.h"
#include "basic_actions/body_kick_one_step.h"
#include "basic_actions/basic_actions.h"
#include "../neck/neck_decision.h"
#include "../setting.h"
#include "../lib/pimentao_verde_say_message_builder.h"

#include <rcsc/player/player_agent.h>
#include <rcsc/common/server_param.h>
#include <rcsc/common/logger.h>
#include <rcsc/geom/line_2d.h>

using namespace rcsc;

namespace {

bool
is_enabled()
{
    if ( ! Setting::i()->mChainAction )
    {
        return true;
    }
    return Setting::i()->mChainAction->mUseOneVsOneFeint;
}

const AbstractPlayerObject *
nearest_opponent( const WorldModel & wm )
{
    const PlayerObject::Cont & opps = wm.opponentsFromSelf();
    if ( opps.empty() )
    {
        return nullptr;
    }
    return opps.front();
}

}

bool
Bhv_OneVsOneFeint::shouldTry( const PlayerAgent * agent )
{
    if ( ! is_enabled() )
    {
        return false;
    }

    const WorldModel & wm = agent->world();

    if ( wm.gameMode().type() != GameMode::PlayOn )
    {
        return false;
    }

    if ( ! wm.self().isKickable() )
    {
        return false;
    }

    if ( wm.kickableTeammate() )
    {
        return false;
    }

    const AbstractPlayerObject * opp = nearest_opponent( wm );
    if ( ! opp || opp->unum() < 1 )
    {
        return false;
    }

    const double opp_dist = opp->distFromSelf();
    const double opp_ball = opp->distFromBall();

    if ( opp_dist < 1.8 || opp_dist > 8.0 )
    {
        return false;
    }

    if ( opp_ball > 5.0 )
    {
        return false;
    }

    const Vector2D ball = wm.ball().pos();
    if ( ball.x < -20.0 )
    {
        return false;
    }

    const Vector2D goal = ServerParam::i().theirTeamGoalPos();
    const AngleDeg to_goal = ( goal - wm.self().pos() ).th();
    const AngleDeg to_opp = ( opp->pos() - wm.self().pos() ).th();
    if ( ( to_goal - to_opp ).abs() > 75.0 )
    {
        return false;
    }

    ActionChainHolder::instance().update( agent );
    const ActionChainGraph & graph = ActionChainHolder::i().graph();
    if ( graph.getAllChain().empty() )
    {
        return true;
    }

    const CooperativeAction & first = graph.getFirstAction();

    if ( first.category() == CooperativeAction::Shoot )
    {
        return false;
    }

    if ( first.category() == CooperativeAction::Dribble )
    {
        if ( first.targetPoint().x >= ball.x - 0.5 )
        {
            return false;
        }
    }

    if ( first.category() == CooperativeAction::Pass )
    {
        if ( first.M_opp_min_dif_cycle >= 3
             && first.targetPoint().x >= wm.self().pos().x - 2.0 )
        {
            return false;
        }
        return true;
    }

    if ( first.category() == CooperativeAction::Hold
         || first.category() == CooperativeAction::Move )
    {
        return true;
    }

    if ( first.category() == CooperativeAction::Dribble )
    {
        return true;
    }

    return false;
}

bool
Bhv_OneVsOneFeint::execute( PlayerAgent * agent )
{
    if ( ! shouldTry( agent ) )
    {
        return false;
    }

    const WorldModel & wm = agent->world();
    const ServerParam & SP = ServerParam::i();

    const AbstractPlayerObject * opp = nearest_opponent( wm );
    if ( ! opp )
    {
        return false;
    }

    const Vector2D ball = wm.ball().pos();
    const Vector2D goal = SP.theirTeamGoalPos();

    Vector2D to_goal = goal - ball;
    if ( to_goal.r() < 0.1 )
    {
        to_goal = Vector2D( 1.0, 0.0 );
    }
    to_goal.setLength( 1.0 );

    Vector2D lateral( -to_goal.y, to_goal.x );
    if ( lateral.innerProduct( opp->pos() - ball ) > 0.0 )
    {
        lateral *= -1.0;
    }

    const double side_dist = 1.15;
    Vector2D feint_ball = ball + lateral * side_dist;

    const double max_x = SP.pitchHalfLength() - 1.0;
    const double max_y = SP.pitchHalfWidth() - 1.0;
    feint_ball.x = std::min( feint_ball.x, max_x );
    feint_ball.x = std::max( feint_ball.x, -max_x );
    feint_ball.y = std::min( feint_ball.y, max_y );
    feint_ball.y = std::max( feint_ball.y, -max_y );

    const double kick_dist = ball.dist( feint_ball );
    double kick_speed = SP.firstBallSpeed( kick_dist, 2 );
    kick_speed = std::min( kick_speed, SP.ballSpeedMax() * 0.85 );
    kick_speed = std::max( kick_speed, 0.65 );

    agent->debugClient().addMessage( "1v1Feint" );
    dlog.addText( Logger::TEAM,
                  __FILE__": 1v1 feint kick to (%.1f %.1f) spd=%.2f",
                  feint_ball.x, feint_ball.y, kick_speed );

    const double min_speed = std::max( 0.45, kick_speed * 0.82 );

    if ( ! Body_SmartKick( feint_ball, kick_speed, min_speed, 2 ).execute( agent )
         && ! Body_KickOneStep( feint_ball, kick_speed, false ).execute( agent ) )
    {
        return false;
    }

    const int n_dash = 4;
    const int n_turn = 1;
    const AngleDeg dash_angle = ( feint_ball - wm.self().pos() ).th();
    const Vector2D run_target = wm.self().pos()
        + Vector2D::polar2vector( 4.0, dash_angle );

    agent->setIntention( pimentao::create_advance_dribble_tail_intention(
        feint_ball,
        run_target,
        dash_angle,
        n_turn,
        n_dash,
        wm.time() ) );

    agent->addSayMessage( new DribbleMessage( feint_ball, n_turn + n_dash + 1 ) );

    if ( n_turn + n_dash >= 3 )
    {
        agent->setViewAction( new View_Normal() );
    }

    NeckDecisionWithBall().setNeck( agent, NeckDecisionType::dribbling );
    return true;
}
