// motive-chess-uci.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "Engine.h"
#include "Log.h"
#include "Logger.h"
#include "LogManager.h"
#include "Streams.h"
#include "VersionInfo.h"

std::vector<std::string> getUciCommands();
void logSanitizedInput( std::vector<std::string> input );
bool processCommandLine( int argc, char** argv, bool* benchmarking, Streams& streams );
bool processUciCommand( Engine& engine, std::vector<std::string> input );

int main( int argc, char** argv )
{
    Streams streams;
    bool benchmarking;

    Log::setDestination( new ConsoleLogDestination() );

    Log::Info << "The answer is " << 42 << " or, in hex, " << std::hex << 42 << std::endl;
    Log::Info( "Hello cruel world" );
    Log::Info << "The answer is " << 42 << " or, in hex, " << std::hex << 42 << std::endl;
    Log::Info << "The answer is " << 42 << " or, in hex, " << std::hex << 42 << std::endl;
    Log::Info( "Hello cruel world" );
    Log::Info( "Hello cruel world" );
    Log::Info = Log::Debug;
    // Default setup, may be overridden later
    LogManager::setLogger( new ConsoleLogger() );

    // Allow command line processing to cause an exit without further activity
    if ( processCommandLine( argc, argv, &benchmarking, streams ) )
    {
        LOG_DEBUG << "Starting";

        //LogManager::getLogger()->log( LogManager::Level::DEBUG, "Starting" );
        //LogManager::getLogger()->log( LogManager::Level::DEBUG, [ & ]( LogManager::LevelLogger& logger ) -> void 
        //{
        //    logger.write( "Simples" );
        //    logger.write( " and " );
        //    logger.write( "Dimples" );
        //} );
        //PLOG_DEBUG( "Is this OK?" );
        //VLOG_DEBUG( "Pi is %f", 3.1415926535897973 );
        //SLOG_DEBUG( l,
        //    {
        //        l.stream() << "How about this?";
        //    } );
        //LogManager::getLogger()->log( LogManager::Level::DEBUG, [ & ] ( LogManager::LevelLogger& logger ) -> void
        //{
        //    logger.stream() << "Hello, cruel world. " << "Here we go";
        //    logger.stream() << "Again";
        //} );

        LogManager::setLevel( LogManager::Level::ERROR );
        std::string hello( "Hello world" );
        PLOG_DEBUG( hello );
        SLOG_DEBUG( l, 
                    { 
                        l.stream() << hello; 
                    } );
        LogManager::getLogger()->log( LogManager::Level::DEBUG, [&] ( LogManager::LevelLogger& logger ) -> void
        {
            logger << "Hello" << ", dolly. Are you " << 44 << "yet?" << std::endl;
            logger << "Hello" << ", dolly. Are you " << 44 << "yet?" << std::endl;
        } );
//        SLOG_DEBUG( 1, { l << "Hello" << ", dolly. Are you " << 42 << "yet?" << std::endl; } );
        VLOG_DEBUG( hello );

        // Configure output location for where to post our UCI responses
        Broadcaster broadcaster( streams.getOuputStream() );
        Engine engine( broadcaster );

        engine.setBenchmarking( benchmarking );

        // Initialize list of UCI commands
        std::vector<std::string> uci = getUciCommands();

        std::vector<std::string> input;
        std::string line;
        while ( streams.getLine( line ) )
        {
            input.clear();

            LOG_TRACE << "Raw input: [" << line << "]";

            // Tokenize the input into a list of strings
            std::replace( line.begin(), line.end(), '\n', ' ' );
            std::replace( line.begin(), line.end(), '\t', ' ' );
            std::stringstream stream( line );
            std::string token;
            while ( std::getline( stream, token, ' ' ) )
            {
                if ( token.length() > 0 )
                {
                    input.push_back( token );
                }
            }

            // Prune unrecognized commands from start of input
            if ( input.size() )
            {
                while ( std::find( uci.begin(), uci.end(), *input.begin() ) == uci.end() )
                {
                    input.erase( input.begin() );
                    if ( input.size() == 0 )
                    {
                        break;
                    }
                }
            }

            // If nothing left, loop around
            if ( input.size() == 0 )
            {
                continue;
            }

            // Dump the sanitized input
            logSanitizedInput( input );

            // Process command input until told to quit
            if ( !processUciCommand( engine, input ) )
            {
                break;
            }
        }

        LOG_DEBUG << "Closing";
    }
    else
    {
        LOG_DEBUG << "Closing after command line processing";
    }

    Logger::shutdown();

    return 0;
}

void logSanitizedInput( std::vector<std::string> input )
{
    std::ostringstream sanitized;

    bool first = true;
    for ( std::vector<std::string>::iterator it = input.begin(); it != input.end(); it++ )
    {
        if ( first )
        {
            first = false;
        }
        else
        {
            sanitized << " ";
        }

        sanitized << *it;
    }

    LOG_TRACE << "Cleaned input: [" << sanitized.str() << "]";
}

std::vector<std::string> getUciCommands()
{
    std::vector<std::string> uci;

    // From 2006 version of UCI spec from ShredderChess site
    uci.push_back( "uci" );
    uci.push_back( "debug" );
    uci.push_back( "isready" );
    uci.push_back( "setoption" );
    uci.push_back( "register" );
    uci.push_back( "ucinewgame" );
    uci.push_back( "position" );
    uci.push_back( "go" );
    uci.push_back( "stop" );
    uci.push_back( "ponderhit" );
    uci.push_back( "quit" );

    // Special testing command - perft
    uci.push_back( "perft" );

    return uci;
}

bool processCommandLine( int argc, char** argv, bool* benchmarking, Streams& streams )
{
    // Set initial defaults
    
    Logger::Level logLevel = Logger::Level::INFO;
    
    *benchmarking = false;

    // Process switches

    std::vector<std::string> arguments;
    for ( int loop = 1; loop < argc; loop++ )
    {
        arguments.push_back( argv[ loop ] );
    }

    // Search for options, rather than looping through the input

    std::vector<std::string>::iterator it;

    // Check for logging configuration options before actually logging anything

    it = std::find( arguments.begin(), arguments.end(), "-verbose" );
    if ( it != arguments.end() )
    {
        logLevel = Logger::Level::TRACE;

        LogManager::getLogger()->setLevel( LogManager::Level::TRACE );
    }

    it = std::find( arguments.begin(), arguments.end(), "-debug" );
    if ( it != arguments.end() )
    {
        logLevel = Logger::Level::DEBUG;

        LogManager::getLogger()->setLevel( LogManager::Level::DEBUG );
    }

    it = std::find( arguments.begin(), arguments.end(), "-quiet" );
    if ( it != arguments.end() )
    {
        logLevel = Logger::Level::ERROR;

        LogManager::getLogger()->setLevel( LogManager::Level::ERROR );
    }

    it = std::find( arguments.begin(), arguments.end(), "-silent" );
    if ( it != arguments.end() )
    {
        logLevel = Logger::Level::NONE;

        LogManager::setLogger( new NullLogger() );
    }

    // Allow control of input and output

    it = std::find( arguments.begin(), arguments.end(), "-logfile" );
    if ( it != arguments.end() )
    {
        // Take the next argument as a filename
        ++it;
        if ( it != arguments.end() )
        {
// TODO reinstate this if we need to            streams.setLogFile( *it );

            LogManager::setLogger( new FileLogger( *it, LogManager::getLogger()->getLevel() ) );
        }
    }

    // Complete logging setup now we have determined its configuration

    Logger::configure( &streams.getLogStream(), logLevel );

    it = std::find( arguments.begin(), arguments.end(), "-input" );
    if ( it != arguments.end() )
    {
        // Take the next argument as a filename
        ++it;
        if ( it != arguments.end() )
        {
            streams.setInputFile( *it );
        }
    }

    it = std::find( arguments.begin(), arguments.end(), "-output" );
    if ( it != arguments.end() )
    {
        // Take the next argument as a filename
        ++it;
        if ( it != arguments.end() )
        {
            streams.setOutputFile( *it );
        }
    }

    // Now check for other instructions

    it = std::find( arguments.begin(), arguments.end(), "-bench" );
    if ( it != arguments.end() )
    {
        *benchmarking = true;
    }

    // Informational stuff

    // Dump the command line args for posterity.
    LOG_TRACE << arguments.size() << " command line argument(s)";
    for ( it = arguments.begin(); it != arguments.end(); it++ )
    {
        LOG_TRACE << "  " << *it;
    }

    it = std::find( arguments.begin(), arguments.end(), "-help" );
    if ( it != arguments.end() )
    {
        std::cout << std::endl << "Supported command line arguments..." << std::endl;
        std::cout << std::endl << "Logging:" << std::endl;
        std::cout << "  -silent  - no logging" << std::endl;
        std::cout << "  -quiet   - minimal logging" << std::endl;
        std::cout << "  -debug   - detailed logging" << std::endl;
        std::cout << "  -verbose - trace logging" << std::endl;
        std::cout << std::endl << "Information (application exits immediately):" << std::endl;
        std::cout << "  -help    - this help" << std::endl;
        std::cout << "  -version - version information" << std::endl;
        std::cout << std::endl << "Configuration:" << std::endl;
        std::cout << "  -bench   - run in benchmark mode" << std::endl;
        std::cout << std::endl << "Debug options:" << std::endl;
        std::cout << "  -input <file>   - read input from file rather than stdin" << std::endl;
        std::cout << "  -output <file>  - write output to file rather than stdout" << std::endl;
        std::cout << "  -logfile <file> - write logging to file rather than stderr" << std::endl;
        std::cout << std::endl;

        return false;
    }

    it = std::find( arguments.begin(), arguments.end(), "-version" );
    if ( it != arguments.end() )
    {
        VersionInfo* versionInfo = VersionInfo::getVersionInfo();

        if ( versionInfo->isAvailable() )
        {
            std::cout << versionInfo->getProductNameAndVersion() << std::endl;
        }
        else
        {
            std::cout << "MotiveChess, version unknown" << std::endl;
        }

        return false;
    }

    return true;
}

bool processUciCommand( Engine& engine, std::vector<std::string> input )
{
    bool quit = false;

    // Pop the command (first item) and leave the rest
    std::string command = input[ 0 ];
    input.erase( input.begin() );

    LOG_TRACE << "Processing " << command;

    if ( command == "uci" )
    {
        engine.uciCommand();
    }
    else if ( command == "debug" )
    {
        engine.debugCommand( input );
    }
    else if ( command == "isready" )
    {
        engine.isreadyCommand();
    }
    else if ( command == "setoption" )
    {
        engine.setoptionCommand( input );
    }
    else if ( command == "register" )
    {
        engine.registerCommand( input );
    }
    else if ( command == "ucinewgame" )
    {
        engine.ucinewgameCommand();
    }
    else if ( command == "position" )
    {
        engine.positionCommand( input );
    }
    else if ( command == "go" )
    {
        engine.goCommand( input );
    }
    else if ( command == "stop" )
    {
        engine.stopCommand();
    }
    else if ( command == "ponderhit" )
    {
        engine.ponderhitCommand();
    }
    else if ( command == "quit" )
    {
        quit = engine.quitCommand();
    }
    // Special perft command
    else if ( command == "perft" )
    {
        engine.perftCommand( input );
    }

    LOG_TRACE << "Quit state is " << quit;

    return !quit;
}