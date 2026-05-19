#ifndef GAME_ANALYSIS_LOGGER_H
#define GAME_ANALYSIS_LOGGER_H

#include <string>

namespace rcsc {
class WorldModel;
}

class GameAnalysisLogger {
public:
    static GameAnalysisLogger & i();

    bool enabled() const;

    void onCycle( const rcsc::WorldModel & wm );

    void flush();

private:
    GameAnalysisLogger();
    GameAnalysisLogger( const GameAnalysisLogger & );
    GameAnalysisLogger & operator=( const GameAnalysisLogger & );

    bool ensureMatchDir( const rcsc::WorldModel & wm );
    void writeMatchMeta( const rcsc::WorldModel & wm );
    void writeCycleSnapshot( const rcsc::WorldModel & wm );
    void writeSystemEvent( const rcsc::WorldModel & wm,
                           const char * event_type );

    bool appendJsonLine( const char * filename, const char * json_line );

    bool M_enabled;
    bool M_match_ready;
    bool M_meta_written;
    int M_last_play_mode_type;
    int M_last_snapshot_cycle;
    int M_snapshot_interval;
    std::string M_match_dir;
};

#endif
