#ifndef GAME_ANALYSIS_CONTEXT_H
#define GAME_ANALYSIS_CONTEXT_H

#include <rcsc/geom/vector_2d.h>
#include <rcsc/types.h>

#include <string>

namespace rcsc {
class GameMode;
class WorldModel;
}

enum class GameAnalysisFieldZone {
    defensive_third,
    middle_third,
    attacking_third
};

std::string
gameAnalysisPlayModeString( const rcsc::GameMode & mode );

GameAnalysisFieldZone
gameAnalysisFieldZone( const rcsc::WorldModel & wm );

void
gameAnalysisScores( const rcsc::WorldModel & wm,
                    int & our_score,
                    int & their_score );

std::string
gameAnalysisFieldZoneString( GameAnalysisFieldZone zone );

#endif
