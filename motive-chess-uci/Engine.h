#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "Board.h"
#include "Broadcaster.h"
#include "CopyProtection.h"
#include "Fen.h"
#include "GameContext.h"
#include "GoContext.h"
#include "Log.h"
#include "Registration.h"
#include "VersionInfo.h"
#include "Utilities.h"

class Engine
{
public:
    enum class DebugSwitch
    {
        OFF,
        ON
    };

private:
    inline static const std::string OPTION_BENCH = "Benchmark";

    // What to do with findings when thinking concludes
    enum class ThinkingOutcome
    {
        BROADCAST,
        DISCARD
    };

    Broadcaster& broadcaster;

    CopyProtection copyProtection;
    Registration registration;

    bool initialized;

    bool ucinewgameExpected;
    bool ucinewgameReceived;

    volatile bool benchmarking;
    volatile DebugSwitch debugging;

    volatile bool quitting;
    volatile bool continueThinking;
    volatile bool broadcastThinkingOutcome;

    std::thread* thinkingThread;
    Board* thinkingBoard;

    GameContext* gameContext;

    class UciLogger
    {
    public:
        UciLogger( Engine& engine, Log::Level level ) :
            engine( engine ),
            level( level )
        {
            // Nothing to do
        }

        virtual ~UciLogger()
        {
            // Write the same message to the regular log
            Log::logger( level ) << "UCI " << os.str() << std::endl;

            // Log only INFO or above unless in debugging mode
            if ( level >= Log::Level::INFO || engine.debugging == Engine::DebugSwitch::ON )
            {
                engine.broadcaster.info( os.str() );
            }
        }

        std::ostringstream& log( std::string prefix = "" )
        {
            os << prefix;
            return os;
        }

    private:
        std::ostringstream os;

        Engine& engine;
        Log::Level level;
    };

    static void thinking( Engine* engine, Board* board, GoContext* context );

    // Helper methods
    void listVisibleOptions();
    void releaseGameContext()
    {
        if ( gameContext != nullptr )
        {
            delete gameContext;
            gameContext = nullptr;
        }
    }

    // Implementation methods that do not broadcast notifications 
    void stopImpl( ThinkingOutcome thinkingOutcome = ThinkingOutcome::DISCARD );
    void isreadyImpl();
    void debugImpl( DebugSwitch flag );
    void registerImpl();
    void registerImpl( std::string& name, std::string& code );
    void setoptionImpl( std::string& name, std::string& value );
    void positionImpl( const std::string& fen, std::vector<std::string> moves );
    void goImpl( GoContext* goContext );

    unsigned long perftImpl( int depth, Board* board, bool divide = false );

    void perftDepth( Board* board, int depth );
    void perftRange( Board* board, std::vector<std::pair<unsigned int, unsigned int>> expectedResults );
    void perftFile( std::string& filename );

    void initializeImpl();

    std::vector<std::string> getGoDirectives()
    {
        std::vector<std::string> directives;
        
        directives.push_back( "searchmoves" );
        directives.push_back( "ponder" );
        directives.push_back( "wtime" );
        directives.push_back( "btime" );
        directives.push_back( "winc" );
        directives.push_back( "binc" );
        directives.push_back( "movestogo" );
        directives.push_back( "depth" );
        directives.push_back( "nodes" );
        directives.push_back( "mate" );
        directives.push_back( "movetime" );
        directives.push_back( "infinite" );

        return directives;
    }

public:
    Engine( Broadcaster& broadcaster ) : 
        broadcaster( broadcaster ), 
        initialized( false ),
        benchmarking( false ),
        quitting( false ),
        continueThinking( false ),
        broadcastThinkingOutcome( false ),
        debugging( DebugSwitch::OFF ),
        ucinewgameExpected( true ),
        ucinewgameReceived( false ),
        gameContext( nullptr ),
        thinkingThread( nullptr ),
        thinkingBoard( nullptr )
    {
        VersionInfo* versionInfo = VersionInfo::getVersionInfo();

        if ( versionInfo->isAvailable() )
        {
            broadcaster.info( versionInfo->getProductNameAndVersion() );
        }
        else
        {
            broadcaster.info( "MotiveChess" );
        }

        initializeImpl();
    }

    virtual ~Engine()
    {
        stopImpl();
        releaseGameContext();

        initialized = false;
    }

    void uciCommand();
    void debugCommand( std::vector<std::string>& arguments );
    void isreadyCommand();
    void setoptionCommand( std::vector<std::string>& arguments );
    void registerCommand( std::vector<std::string>& arguments );
    void ucinewgameCommand();
    void positionCommand( std::vector<std::string>& arguments );
    void goCommand( std::vector<std::string>& arguments );
    void stopCommand();
    void ponderhitCommand();
    bool quitCommand();

    // Special perft command
    void perftCommand( std::vector<std::string>& arguments, bool expectsDepth = true );

    // Option methods

    void setBenchmarking( bool benchmarking )
    {
        Log::Info << "Set benchmarking " << ( benchmarking ? "on" : "off" ) << std::endl;

        this->benchmarking = benchmarking;
    }
};

