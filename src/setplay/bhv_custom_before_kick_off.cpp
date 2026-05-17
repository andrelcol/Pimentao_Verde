// -*-c++-*-

/*
 *Copyright:

 Copyright (C) Hidehisa AKIYAMA

 This code is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 3 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 *EndCopyright:
 */

/////////////////////////////////////////////////////////////////////

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "bhv_custom_before_kick_off.h"

#include "basic_actions/bhv_scan_field.h"
#include "basic_actions/neck_turn_to_relative.h"

#include <rcsc/player/player_agent.h>
#include <rcsc/common/server_param.h>
#include <rcsc/common/logger.h>

using namespace rcsc;

// #define DEBUG_PRINT

namespace {
const double X_LEFT  = -30.0;
const double X_MID   = -24.0;
const double X_RIGHT = -18.0;
const double X_LEG1  = -23.0;
const double X_LEG2  = -18.0;

const double Y_TOP   = -18.0;
const double Y_UP    = -10.0;
const double Y_MID   = -2.0;
const double Y_LOW   = 8.0;
const double Y_BOT   = 18.0;
}

Vector2D
Bhv_CustomBeforeKickOff::getPimentaoVerdeRPosition( int unum )
{
    switch ( unum ) {
        case 1:  return Vector2D( -50.0,  0.0 );   // goalie

        // barra vertical
        case 2:  return Vector2D( X_LEFT, Y_TOP );
        case 3:  return Vector2D( X_LEFT, Y_UP );
        case 4:  return Vector2D( X_LEFT, Y_MID );
        case 5:  return Vector2D( X_LEFT, Y_LOW );
        case 6:  return Vector2D( X_LEFT, Y_BOT );

        // parte superior do R
        case 7:  return Vector2D( X_MID,   Y_TOP );
        case 8:  return Vector2D( X_RIGHT, Y_TOP );
        case 9:  return Vector2D( X_RIGHT, Y_UP );

        // perna diagonal
        case 10: return Vector2D( X_LEG1, -2.0 );
        case 11: return Vector2D( X_LEG2, Y_BOT );

        default: return Vector2D( -25.0, 0.0 );
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
bool
Bhv_CustomBeforeKickOff::execute( PlayerAgent * agent )
{
    const ServerParam & SP = ServerParam::i();
    const WorldModel & wm = agent->world();

    if ( wm.time().cycle() == 0
         && wm.time().stopped() < 5 )
    {
        agent->doTurn( 0.0 );
        agent->setNeckAction( new Neck_TurnToRelative( 0.0 ) );
        return false;
    }

    if ( wm.gameMode().type() == GameMode::AfterGoal_
         && wm.self().vel().r() > 0.05 )
    {
        agent->doTurn( 180.0 );
        agent->setNeckAction( new Neck_TurnToRelative( 0.0 ) );
        return true;
    }

    if ( wm.gameMode().type() == GameMode::BeforeKickOff
         || wm.gameMode().type() == GameMode::AfterGoal_ )
    {
        M_move_point = getPimentaoVerdeRPosition( wm.self().unum() );
    }

    SideID kickoff_side = NEUTRAL;

    if ( wm.gameMode().type() == GameMode::AfterGoal_ )
    {
        if ( wm.gameMode().side() != wm.ourSide() )
        {
            kickoff_side = wm.ourSide();
        }
        else
        {
            kickoff_side = ( wm.ourSide() == LEFT
                             ? RIGHT
                             : LEFT );
        }
    }
    else
    {
        if ( SP.halfTime() > 0 )
        {
            int half_time = SP.halfTime() * 10;
            int extra_half_time = SP.extraHalfTime() * 10;
            int normal_time = half_time * SP.nrNormalHalfs();
            int extra_time = extra_half_time * SP.nrExtraHalfs();

            int time_flag = 0;

            if ( wm.time().cycle() <= normal_time )
            {
                time_flag = ( wm.time().cycle() / half_time ) % 2;
            }
            else if ( wm.time().cycle() <= normal_time + extra_time )
            {
                int overtime = wm.time().cycle() - normal_time;
                time_flag = ( overtime / extra_half_time ) % 2;
            }

            kickoff_side = ( time_flag == 0
                             ? LEFT
                             : RIGHT );
        }
        else
        {
            kickoff_side = LEFT;
        }
    }

    if ( SP.kickoffOffside()
         && M_move_point.x >= 0.0 )
    {
        M_move_point.x = -0.001;
    }

    if ( kickoff_side != wm.ourSide()
         && M_move_point.r() < ServerParam::i().centerCircleR() + 0.1 )
    {
        if ( M_move_point.r() > 0.001 )
        {
            M_move_point *= ( ServerParam::i().centerCircleR() + 0.5 ) / M_move_point.r();
        }
        else
        {
            M_move_point = Vector2D( - ( ServerParam::i().centerCircleR() + 0.5 ), 0.0 );
        }
    }

    const double dist = ( M_move_point - wm.self().pos() ).r();
    if ( dist > 1.0 )
    {
        agent->doMove( M_move_point.x, M_move_point.y );
        agent->setNeckAction( new Neck_TurnToRelative( 0.0 ) );
        return true;
    }

    return Bhv_ScanField().execute( agent );
}