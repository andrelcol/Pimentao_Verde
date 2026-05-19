#include "game_analysis_context.h"

#include <rcsc/common/server_param.h>
#include <rcsc/game_mode.h>
#include <rcsc/player/world_model.h>

#include <cmath>

std::string
gameAnalysisPlayModeString( const rcsc::GameMode & mode )
{
    switch ( mode.type() )
    {
    case rcsc::GameMode::BeforeKickOff: return "before_kick_off";
    case rcsc::GameMode::TimeOver: return "time_over";
    case rcsc::GameMode::PlayOn: return "play_on";
    case rcsc::GameMode::KickOff_: return "kick_off";
    case rcsc::GameMode::KickIn_: return "kick_in";
    case rcsc::GameMode::FreeKick_: return "free_kick";
    case rcsc::GameMode::CornerKick_: return "corner_kick";
    case rcsc::GameMode::GoalKick_: return "goal_kick";
    case rcsc::GameMode::AfterGoal_: return "after_goal";
    case rcsc::GameMode::OffSide_: return "offside";
    case rcsc::GameMode::PenaltyKick_: return "penalty_kick";
    case rcsc::GameMode::FirstHalfOver: return "first_half_over";
    case rcsc::GameMode::Pause: return "pause";
    case rcsc::GameMode::Human: return "human";
    case rcsc::GameMode::FoulCharge_: return "foul_charge";
    case rcsc::GameMode::FoulPush_: return "foul_push";
    case rcsc::GameMode::FoulMultipleAttacker_: return "foul_multiple_attacker";
    case rcsc::GameMode::FoulBallOut_: return "foul_ball_out";
    case rcsc::GameMode::BackPass_: return "back_pass";
    case rcsc::GameMode::FreeKickFault_: return "free_kick_fault";
    case rcsc::GameMode::CatchFault_: return "catch_fault";
    case rcsc::GameMode::IndFreeKick_: return "indirect_free_kick";
    case rcsc::GameMode::PenaltySetup_: return "penalty_setup";
    case rcsc::GameMode::PenaltyReady_: return "penalty_ready";
    case rcsc::GameMode::PenaltyTaken_: return "penalty_taken";
    case rcsc::GameMode::PenaltyMiss_: return "penalty_miss";
    case rcsc::GameMode::PenaltyScore_: return "penalty_score";
    case rcsc::GameMode::IllegalDefense_: return "illegal_defense";
    case rcsc::GameMode::PenaltyOnfield_: return "penalty_onfield";
    case rcsc::GameMode::PenaltyFoul_: return "penalty_foul";
    case rcsc::GameMode::GoalieCatch_: return "goalie_catch";
    case rcsc::GameMode::ExtendHalf: return "extend_half";
    default: return "unknown";
    }
}

GameAnalysisFieldZone
gameAnalysisFieldZone( const rcsc::WorldModel & wm )
{
    const double pitch_half = rcsc::ServerParam::i().pitchHalfLength();
    const double third = pitch_half / 3.0;
    const double ball_x = wm.ball().pos().x;

    if ( wm.ourSide() == rcsc::LEFT )
    {
        if ( ball_x < -third )
        {
            return GameAnalysisFieldZone::defensive_third;
        }
        if ( ball_x > third )
        {
            return GameAnalysisFieldZone::attacking_third;
        }
        return GameAnalysisFieldZone::middle_third;
    }

    if ( ball_x > third )
    {
        return GameAnalysisFieldZone::defensive_third;
    }
    if ( ball_x < -third )
    {
        return GameAnalysisFieldZone::attacking_third;
    }
    return GameAnalysisFieldZone::middle_third;
}

std::string
gameAnalysisFieldZoneString( GameAnalysisFieldZone zone )
{
    switch ( zone )
    {
    case GameAnalysisFieldZone::defensive_third: return "defensive_third";
    case GameAnalysisFieldZone::middle_third: return "middle_third";
    case GameAnalysisFieldZone::attacking_third: return "attacking_third";
    default: return "unknown";
    }
}

void
gameAnalysisScores( const rcsc::WorldModel & wm,
                    int & our_score,
                    int & their_score )
{
    const int left = wm.gameMode().scoreLeft();
    const int right = wm.gameMode().scoreRight();

    if ( wm.ourSide() == rcsc::LEFT )
    {
        our_score = left;
        their_score = right;
    }
    else
    {
        our_score = right;
        their_score = left;
    }
}
