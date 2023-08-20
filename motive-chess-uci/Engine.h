#pragma once

#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "Broadcaster.h"
#include "CopyProtection.h"
#include "Fen.h"
#include "GameContext.h"
#include "GoContext.h"
#include "Logger.h"
#include "Registration.h"
#include "VersionInfo.h"

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
        UciLogger( Engine& engine, Logger::Level level ) :
            engine( engine ),
            level( level )
        {
            // Nothing to do
        }

        virtual ~UciLogger()
        {
            // Write the same message to the regular log
            Logger( level ).log( location ) << "UCI " << os.str();

            // Log only INFO or above unless in debugging mode
            if ( level >= Logger::Level::INFO || engine.debugging == Engine::DebugSwitch::ON )
            {
                engine.broadcaster.info( os.str() );
            }
        }

        std::ostringstream& log( std::string prefix = "", const std::source_location location = std::source_location::current() )
        {
            this->location = location;

            os << prefix;
            return os;
        }

    private:
        std::ostringstream os;

        std::source_location location;

        Engine& engine;
        Logger::Level level;
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

    unsigned long perftImpl( int depth, Board board, bool divide = false );

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
            std::stringstream details;
            details << versionInfo->getProductName() << " version " << versionInfo->getProductVersion();
            broadcaster.info( details.str() );
        }
        else
        {
            broadcaster.info( "MotiveChess" );
        }
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
    void perftCommand( std::vector<std::string>& arguments );

    // Option methods

    void setBenchmarking( bool benchmarking )
    {
        LOG_INFO << "Set benchmarking " << ( benchmarking ? "on" : "off" );

        this->benchmarking = benchmarking;
    }
};

