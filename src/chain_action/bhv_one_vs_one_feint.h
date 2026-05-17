// -*-c++-*-
// Pimentao_Verde: lateral feint + run onto ball (1v1 test behaviour).

#ifndef BHV_ONE_VS_ONE_FEINT_H
#define BHV_ONE_VS_ONE_FEINT_H

#include <rcsc/player/soccer_action.h>

class Bhv_OneVsOneFeint
    : public rcsc::SoccerBehavior {
public:
    bool execute( rcsc::PlayerAgent * agent );

    /*! True when 1v1 feint is preferable to a weak chain pass / hold. */
    static bool shouldTry( const rcsc::PlayerAgent * agent );
};

#endif
