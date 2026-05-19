// -*-c++-*-

/*!
  \file bhv_open_goal_shoot.h
  \brief force shoot when opponent goalie is out and the goal is open
*/

#ifndef BHV_OPEN_GOAL_SHOOT_H
#define BHV_OPEN_GOAL_SHOOT_H

#include <rcsc/player/soccer_action.h>

/*!
  \class Bhv_OpenGoalShoot
  \brief shoot before pass/chain when GK is out and angle or 1-kick course exists
 */
class Bhv_OpenGoalShoot
    : public rcsc::SoccerBehavior {
public:

    bool execute( rcsc::PlayerAgent * agent );
};

#endif
