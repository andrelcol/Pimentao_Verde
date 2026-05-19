// -*-c++-*-

/*!
  \file bhv_smart_shoot.h
  \brief smart shoot behavior (v1 wraps ShootGenerator, falls back to strict check)
*/

#ifndef BHV_SMART_SHOOT_H
#define BHV_SMART_SHOOT_H

#include <rcsc/player/soccer_action.h>

class Bhv_SmartShoot
    : public rcsc::SoccerBehavior {
public:
    double opp_dist_thr;

    explicit Bhv_SmartShoot( double thr )
        : opp_dist_thr( thr )
      { }

    bool execute( rcsc::PlayerAgent * agent );
};

#endif
