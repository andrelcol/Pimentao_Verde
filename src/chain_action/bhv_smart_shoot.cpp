// -*-c++-*-

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "bhv_smart_shoot.h"

#include "../data_extractor/offensive_data_extractor.h"
#include "../setting.h"
#include "bhv_strict_check_shoot.h"
#include "smart_shoot_evaluator.h"

#include <rcsc/player/player_agent.h>
#include <rcsc/common/logger.h>

using namespace rcsc;

/*-------------------------------------------------------------------*/
/*!

 */
bool
Bhv_SmartShoot::execute( PlayerAgent * agent )
{
    if ( ! Setting::i()->mChainAction
         || ! Setting::i()->mChainAction->mUseSmartShoot )
    {
        return Bhv_StrictCheckShoot( opp_dist_thr ).execute( agent );
    }

    const WorldModel & wm
        = OffensiveDataExtractor::i().option.output_worldMode == FULLSTATE
        ? agent->fullstateWorld()
        : agent->world();

    if ( ! wm.self().isKickable() )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << " not ball kickable!"
                  << std::endl;
        dlog.addText( Logger::ACTION,
                      __FILE__":  not kickable" );
        return false;
    }

    const SmartShootSelection selection
        = SmartShootEvaluator::selectBest( wm, opp_dist_thr );

    if ( ! selection.valid )
    {
        dlog.addText( Logger::SHOOT,
                      __FILE__": invalid selection, fallback strict" );
        return Bhv_StrictCheckShoot( opp_dist_thr ).execute( agent );
    }

    if ( Bhv_StrictCheckShoot::executeKickForCourse( agent,
                                                     wm,
                                                     selection.course ) )
    {
        return true;
    }

    dlog.addText( Logger::SHOOT,
                  __FILE__": kick failed, fallback strict" );
    return Bhv_StrictCheckShoot( opp_dist_thr ).execute( agent );
}
