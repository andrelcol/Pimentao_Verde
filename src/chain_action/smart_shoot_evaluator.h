// -*-c++-*-

/*!
  \file smart_shoot_evaluator.h
  \brief v1 shoot course selection wrapping ShootGenerator
*/

#ifndef SMART_SHOOT_EVALUATOR_H
#define SMART_SHOOT_EVALUATOR_H

#include "shoot_generator.h"

namespace rcsc {
class WorldModel;
}

struct SmartShootSelection {
    bool valid;
    ShootGenerator::Course course;

    SmartShootSelection()
        : valid( false ),
          course( 0,
                  rcsc::Vector2D::INVALIDATED,
                  0.0,
                  rcsc::AngleDeg( 0.0 ),
                  0.0,
                  0 )
      { }
};

class SmartShootEvaluator {
public:
    static constexpr double MIN_SHOOT_SCORE = -100000.0;

    static SmartShootSelection
    selectBest( const rcsc::WorldModel & wm,
                const double opp_dist_thr );
};

#endif
