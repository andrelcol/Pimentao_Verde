// -*-c++-*-

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "actgen_improved_dribble.h"
#include "bhv_improved_dribble.h"
#include "dribble.h"
#include "field_analyzer.h"
#include "action_state_pair.h"
#include "predict_state.h"

#include <rcsc/player/world_model.h>
#include <rcsc/common/logger.h>
#include <rcsc/common/server_param.h>

using namespace rcsc;

void
ActGen_ImprovedDribble::generate( std::vector< ActionStatePair > * result,
                                  const PredictState & state,
                                  const WorldModel & current_wm,
                                  const std::vector< ActionStatePair > & path ) const
{
    static GameTime s_last_call_time( 0, 0 );
    static int s_action_count = 0;

    if ( current_wm.time() != s_last_call_time )
    {
        s_action_count = 0;
        s_last_call_time = current_wm.time();
    }

    if ( path.empty() )
    {
        return;
    }

    const AbstractPlayerObject * holder = state.ballHolder();
    if ( ! holder )
    {
        return;
    }

    /* Fewer samples than simple dribble: focus on forward-ish lanes with more
       conservative opponent margin (bonus_step). */
    const int ANGLE_DIVS = 8;
    const double ANGLE_STEP = 360.0 / ANGLE_DIVS;
    const int DIST_DIVS = 2;
    const double DIST_STEP = 1.5;
    const int bonus_step = 0;

    const ServerParam & SP = ServerParam::i();
    const double max_x = SP.pitchHalfLength() - 1.0;
    const double max_y = SP.pitchHalfWidth() - 1.0;

    const PlayerType * ptype = holder->playerTypePtr();

    for ( int a = 0; a < ANGLE_DIVS; ++a )
    {
        const AngleDeg target_angle = ANGLE_STEP * a;

        if ( holder->pos().x < 16.0
             && target_angle.abs() > 100.0 )
        {
            continue;
        }

        if ( holder->pos().x < -36.0
             && holder->pos().absY() < 20.0
             && target_angle.abs() > 45.0 )
        {
            continue;
        }

        const Vector2D unit_vec = Vector2D::from_polar( 1.0, target_angle );
        for ( int d = 1; d <= DIST_DIVS; ++d )
        {
            const double holder_move_dist = DIST_STEP * d;
            const Vector2D target_point
                = holder->pos()
                + unit_vec.setLengthVector( holder_move_dist );

            if ( target_point.absX() > max_x
                 || target_point.absY() > max_y )
            {
                continue;
            }

            const int holder_reach_step
                = 1 + 1
                + ptype->cyclesToReachDistance( holder_move_dist - ptype->kickableArea() * 0.5 );

            bool exist_opponent = false;
            for ( AbstractPlayerObject::Cont::const_iterator o = state.theirPlayers().begin();
                  o != state.theirPlayers().end();
                  ++o )
            {
                const double opp_move_dist = ( *o )->pos().dist( target_point );
                const int o_step
                    = 1
                    + ( *o )->playerTypePtr()->cyclesToReachDistance(
                          opp_move_dist - ptype->kickableArea() );

                if ( o_step - bonus_step <= holder_reach_step )
                {
                    exist_opponent = true;
                    break;
                }
            }

            if ( exist_opponent )
            {
                continue;
            }

            const double ball_speed = SP.firstBallSpeed( state.ball().pos().dist( target_point ),
                                                         holder_reach_step );

            PredictState::ConstPtr result_state( new PredictState( state,
                                                                   holder_reach_step,
                                                                   holder->unum(),
                                                                   target_point ) );
            CooperativeAction::Ptr action( new Dribble( holder->unum(),
                                                          target_point,
                                                          ball_speed,
                                                          1,
                                                          1,
                                                          holder_reach_step + 2,
                                                          PIMENTAO_IMPROVED_DRIBBLE_DESC ) );
            ++s_action_count;
            action->setIndex( s_action_count );
            result->push_back( ActionStatePair( action, result_state ) );
        }
    }
}
