#include "game_analysis_logger.h"

#include "game_analysis_context.h"
#include "../setting.h"

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <rcsc/common/server_param.h>
#include <rcsc/game_mode.h>
#include <rcsc/player/world_model.h>
#include <rcsc/player/player_object.h>

#include <cstdio>
#include <ctime>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>

namespace {

std::string
sanitizeName( const std::string & name )
{
    if ( name.empty() )
    {
        return "unknown";
    }

    std::string out;
    out.reserve( name.size() );
    for ( std::size_t i = 0; i < name.size(); ++i )
    {
        const char c = name[i];
        if ( ( c >= 'a' && c <= 'z' )
             || ( c >= 'A' && c <= 'Z' )
             || ( c >= '0' && c <= '9' )
             || c == '-' || c == '_' )
        {
            out += c;
        }
        else
        {
            out += '_';
        }
    }
    return out;
}

bool
mkdirOne( const std::string & path )
{
    if ( path.empty() )
    {
        return false;
    }

    struct stat st;
    if ( stat( path.c_str(), &st ) == 0 )
    {
        return S_ISDIR( st.st_mode );
    }

    return ::mkdir( path.c_str(), 0755 ) == 0;
}

bool
mkdirRecursive( const std::string & path )
{
    if ( path.empty() )
    {
        return false;
    }

    if ( mkdirOne( path ) )
    {
        return true;
    }

    const std::size_t slash = path.find_last_of( '/' );
    if ( slash == std::string::npos || slash == 0 )
    {
        return false;
    }

    const std::string parent = path.substr( 0, slash );
    if ( ! mkdirRecursive( parent ) )
    {
        return false;
    }

    return mkdirOne( path );
}

void
writeVec2( rapidjson::Writer< rapidjson::StringBuffer > & writer,
           const char * key,
           const rcsc::Vector2D & v )
{
    writer.Key( key );
    writer.StartObject();
    writer.Key( "x" );
    writer.Double( v.x );
    writer.Key( "y" );
    writer.Double( v.y );
    writer.EndObject();
}

}

/*-------------------------------------------------------------------*/

GameAnalysisLogger &
GameAnalysisLogger::i()
{
    static GameAnalysisLogger s_instance;
    return s_instance;
}

/*-------------------------------------------------------------------*/

GameAnalysisLogger::GameAnalysisLogger()
    : M_enabled( false ),
      M_match_ready( false ),
      M_meta_written( false ),
      M_last_play_mode_type( -1 ),
      M_last_snapshot_cycle( -1000000 ),
      M_snapshot_interval( 50 )
{
}

/*-------------------------------------------------------------------*/

bool
GameAnalysisLogger::enabled() const
{
    return M_enabled;
}

/*-------------------------------------------------------------------*/

bool
GameAnalysisLogger::appendJsonLine( const char * filename,
                                    const char * json_line )
{
    if ( ! M_match_ready || M_match_dir.empty() )
    {
        return false;
    }

    const std::string path = M_match_dir + "/" + filename;
    FILE * fp = std::fopen( path.c_str(), "a" );
    if ( ! fp )
    {
        return false;
    }

    std::fputs( json_line, fp );
    std::fputc( '\n', fp );
    std::fclose( fp );
    return true;
}

/*-------------------------------------------------------------------*/

bool
GameAnalysisLogger::ensureMatchDir( const rcsc::WorldModel & wm )
{
    if ( M_match_ready )
    {
        return true;
    }

    const GameAnalysisSetting * ga = Setting::i()->mGameAnalysis;
    if ( ! ga )
    {
        return false;
    }

    const std::time_t now = std::time( nullptr );
    std::tm tm_buf;
#if defined( _WIN32 )
    localtime_s( &tm_buf, &now );
#else
    localtime_r( &now, &tm_buf );
#endif

    char time_buf[32];
    std::strftime( time_buf, sizeof( time_buf ), "%Y%m%d_%H%M%S", &tm_buf );

    const std::string our_name = sanitizeName( wm.ourTeamName() );
    const std::string their_name = sanitizeName( wm.theirTeamName() );
    const char * side = ( wm.ourSide() == rcsc::LEFT ? "left" : "right" );

    M_match_dir = ga->mGameAnalysisLogDir
        + "/match_"
        + time_buf
        + "_"
        + our_name
        + "_vs_"
        + their_name
        + "_"
        + side;

    if ( ! mkdirRecursive( ga->mGameAnalysisLogDir )
         || ! mkdirRecursive( M_match_dir ) )
    {
        M_match_dir.clear();
        return false;
    }

    M_match_ready = true;
    return true;
}

/*-------------------------------------------------------------------*/

void
GameAnalysisLogger::writeMatchMeta( const rcsc::WorldModel & wm )
{
    if ( M_meta_written )
    {
        return;
    }

    const rcsc::ServerParam & SP = rcsc::ServerParam::i();
    const Setting * setting = Setting::i();
    const GameAnalysisSetting * ga = setting->mGameAnalysis;

    rapidjson::StringBuffer buffer;
    rapidjson::Writer< rapidjson::StringBuffer > writer( buffer );

    writer.StartObject();
    writer.Key( "type" );
    writer.String( "match_meta" );
    writer.Key( "cycle" );
    writer.Int( wm.time().cycle() );
    writer.Key( "team" );
    writer.String( wm.ourTeamName().c_str() );
    writer.Key( "opponent" );
    writer.String( wm.theirTeamName().c_str() );
    writer.Key( "side" );
    writer.String( wm.ourSide() == rcsc::LEFT ? "left" : "right" );
    writer.Key( "settings_file" );
    writer.String( setting->mJsonPath.c_str() );
    writer.Key( "formation" );
    writer.String( setting->mStrategySetting->mFormation.c_str() );
    writer.Key( "team_tactic" );
    writer.String( setting->mStrategySetting->mTeamTactic.c_str() );
    writer.Key( "use_smart_shoot" );
    writer.Bool( setting->mChainAction->mUseSmartShoot );
    writer.Key( "game_analysis_full_mode" );
    writer.Bool( ga ? ga->mGameAnalysisLogFullMode : false );
    writer.Key( "server_param" );
    writer.StartObject();
    writer.Key( "ball_decay" );
    writer.Double( SP.ballDecay() );
    writer.Key( "ball_speed_max" );
    writer.Double( SP.ballSpeedMax() );
    writer.Key( "kick_power_rate" );
    writer.Double( SP.kickPowerRate() );
    writer.Key( "player_speed_max" );
    writer.Double( SP.defaultPlayerSpeedMax() );
    writer.Key( "dash_power_rate" );
    writer.Double( SP.defaultDashPowerRate() );
    writer.EndObject();
    writer.EndObject();

    if ( appendJsonLine( "match_meta.jsonl", buffer.GetString() ) )
    {
        M_meta_written = true;
    }
}

/*-------------------------------------------------------------------*/

void
GameAnalysisLogger::writeCycleSnapshot( const rcsc::WorldModel & wm )
{
    const int cycle = wm.time().cycle();
    if ( cycle - M_last_snapshot_cycle < M_snapshot_interval )
    {
        return;
    }

    M_last_snapshot_cycle = cycle;

    int our_score = 0;
    int their_score = 0;
    gameAnalysisScores( wm, our_score, their_score );

    const rcsc::Vector2D ball_pos = wm.ball().pos();
    const rcsc::Vector2D ball_vel = wm.ball().vel();
    const GameAnalysisFieldZone zone = gameAnalysisFieldZone( wm );

    rapidjson::StringBuffer buffer;
    rapidjson::Writer< rapidjson::StringBuffer > writer( buffer );

    writer.StartObject();
    writer.Key( "type" );
    writer.String( "cycle_snapshot" );
    writer.Key( "cycle" );
    writer.Int( cycle );
    writer.Key( "play_mode" );
    writer.String( gameAnalysisPlayModeString( wm.gameMode() ).c_str() );
    writer.Key( "side" );
    writer.String( wm.ourSide() == rcsc::LEFT ? "left" : "right" );
    writer.Key( "score_us" );
    writer.Int( our_score );
    writer.Key( "score_them" );
    writer.Int( their_score );
    writeVec2( writer, "ball_pos", ball_pos );
    writeVec2( writer, "ball_vel", ball_vel );
    writeVec2( writer, "self_pos", wm.self().pos() );
    writer.Key( "field_zone" );
    writer.String( gameAnalysisFieldZoneString( zone ).c_str() );

    if ( ! wm.teammatesFromBall().empty()
         && wm.teammatesFromBall().front() )
    {
        const rcsc::PlayerObject * tm = wm.teammatesFromBall().front();
        writer.Key( "nearest_teammate_to_ball" );
        writer.StartObject();
        writer.Key( "unum" );
        writer.Int( tm->unum() );
        writer.Key( "dist" );
        writer.Double( tm->distFromBall() );
        writeVec2( writer, "pos", tm->pos() );
        writer.EndObject();
    }

    if ( ! wm.opponentsFromBall().empty()
         && wm.opponentsFromBall().front() )
    {
        const rcsc::PlayerObject * opp = wm.opponentsFromBall().front();
        writer.Key( "nearest_opponent_to_ball" );
        writer.StartObject();
        writer.Key( "unum" );
        writer.Int( opp->unum() );
        writer.Key( "dist" );
        writer.Double( opp->distFromBall() );
        writeVec2( writer, "pos", opp->pos() );
        writer.EndObject();
    }

    writer.EndObject();

    appendJsonLine( "cycle_snapshot.jsonl", buffer.GetString() );
}

/*-------------------------------------------------------------------*/

void
GameAnalysisLogger::writeSystemEvent( const rcsc::WorldModel & wm,
                                      const char * event_type )
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer< rapidjson::StringBuffer > writer( buffer );

    writer.StartObject();
    writer.Key( "type" );
    writer.String( "system_event" );
    writer.Key( "event" );
    writer.String( event_type );
    writer.Key( "cycle" );
    writer.Int( wm.time().cycle() );
    writer.Key( "play_mode" );
    writer.String( gameAnalysisPlayModeString( wm.gameMode() ).c_str() );
    writer.EndObject();

    appendJsonLine( "system_events.jsonl", buffer.GetString() );
}

/*-------------------------------------------------------------------*/

void
GameAnalysisLogger::onCycle( const rcsc::WorldModel & wm )
{
    const GameAnalysisSetting * ga = Setting::i()->mGameAnalysis;
    if ( ! ga || ! ga->mEnableGameAnalysisLog )
    {
        M_enabled = false;
        return;
    }

    M_enabled = true;
    M_snapshot_interval = ga->mGameAnalysisSnapshotInterval;
    if ( M_snapshot_interval < 1 )
    {
        M_snapshot_interval = 50;
    }

    if ( ! ensureMatchDir( wm ) )
    {
        return;
    }

    writeMatchMeta( wm );

    const int play_mode_type = static_cast< int >( wm.gameMode().type() );
    if ( M_last_play_mode_type >= 0
         && play_mode_type != M_last_play_mode_type )
    {
        writeSystemEvent( wm, "play_mode_change" );
    }
    M_last_play_mode_type = play_mode_type;

    writeCycleSnapshot( wm );
}

/*-------------------------------------------------------------------*/

void
GameAnalysisLogger::flush()
{
    // Commit 1 uses append-and-close per line; nothing buffered.
}
