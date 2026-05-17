// -*-c++-*-
// Pimentao_Verde: optional dribble executor (parallel to Bhv_NormalDribble).

#ifndef BHV_IMPROVED_DRIBBLE_H
#define BHV_IMPROVED_DRIBBLE_H

#include "cooperative_action.h"

#include <rcsc/player/soccer_action.h>
#include <rcsc/geom/vector_2d.h>

/*! String tag on CooperativeAction::Dribble to route here from Bhv_ChainAction. */
extern const char PIMENTAO_IMPROVED_DRIBBLE_DESC[];

/*!
  Same tail as normal dribble (IntentionNormalDribble) but first kick prefers
  Body_SmartKick with a safer minimum speed before falling back to KickOneStep.
 */
class Bhv_ImprovedDribble
    : public rcsc::SoccerBehavior {
private:
    rcsc::Vector2D M_target_point;
    rcsc::Vector2D M_intermediate_pos;
    rcsc::Vector2D M_player_target_point;

    double M_first_ball_speed;
    double M_dash_angle;
    double M_first_turn_moment;
    double M_first_dash_power;
    rcsc::AngleDeg M_first_dash_angle;

    int M_total_step;
    int M_kick_step;
    int M_turn_step;
    int M_dash_step;

    rcsc::NeckAction::Ptr M_neck_action;
    rcsc::ViewAction::Ptr M_view_action;

    const char * M_dec;

public:
    Bhv_ImprovedDribble( const CooperativeAction & action,
                         rcsc::NeckAction::Ptr neck = rcsc::NeckAction::Ptr(),
                         rcsc::ViewAction::Ptr view = rcsc::ViewAction::Ptr() );

    bool execute( rcsc::PlayerAgent * agent );
};

#endif
