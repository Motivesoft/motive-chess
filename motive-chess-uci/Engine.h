#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "Broadcaster.h"
#include "CopyProtection.h"
#include "Fen.h"
#include "GameContext.h"
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

    Broadcaster& broadcaster;

    CopyProtection copyProtection;
    Registration registration;

    bool initialized;

    bool ucinewgameExpected;
    bool ucinewgameReceived;

    volatile bool benchmarking;
    volatile bool quitting;
    volatile DebugSwitch debugging;

    volatile GameContext* gameContext;

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

    static void thinking( Engine* engine );

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
    void stopImpl();
    void isreadyImpl();
    void debugImpl( DebugSwitch flag );
    void registerImpl();
    void registerImpl( std::string& name, std::string& code );
    void setoptionImpl( std::string& name, std::string& value );
    void positionImpl( const std::string& fen, std::vector<std::string> moves );
    void goImpl( std::vector<std::string> searchMoves, bool ponder, int wtime, int btime, int winc, int binc, int movestogo, int depth, int nodes, int mate, int movetime, bool infinite );
    void perftImpl( int depth, std::string& fen );

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
        debugging( DebugSwitch::OFF ),
        ucinewgameExpected( true ),
        ucinewgameReceived( false ),
        gameContext( nullptr )
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

