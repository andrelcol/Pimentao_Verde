// -*-c++-*-

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "smart_shoot_evaluator.h"

#include <rcsc/common/logger.h>
#include <rcsc/player/world_model.h>

#include <algorithm>

using namespace rcsc;

/*-------------------------------------------------------------------*/
/*!

 */
SmartShootSelection
SmartShootEvaluator::selectBest( const WorldModel & wm,
                                 const double opp_dist_thr )
{
    SmartShootSelection selection;

    const ShootGenerator::Container & cont
        = ShootGenerator::instance().courses( wm, opp_dist_thr );

    if ( cont.empty() )
    {
        dlog.addText( Logger::SHOOT,
                      __FILE__": no shoot course" );
        return selection;
    }

    ShootGenerator::Container::const_iterator best_shoot
        = std::min_element( cont.begin(),
                            cont.end(),
                            ShootGenerator::ScoreCmp() );

    if ( best_shoot == cont.end() )
    {
        dlog.addText( Logger::SHOOT,
                      __FILE__": no best shoot" );
        return selection;
    }

    if ( best_shoot->score_ < MIN_SHOOT_SCORE )
    {
        dlog.addText( Logger::SHOOT,
                      __FILE__": best shoot score too low (%.1f)",
                      best_shoot->score_ );
        return selection;
    }

    selection.valid = true;
    selection.course = *best_shoot;

    dlog.addText( Logger::SHOOT,
                  __FILE__": selected course[%d] target=(%.2f, %.2f) score=%.1f",
                  selection.course.index_,
                  selection.course.target_point_.x,
                  selection.course.target_point_.y,
                  selection.course.score_ );

    return selection;
}
