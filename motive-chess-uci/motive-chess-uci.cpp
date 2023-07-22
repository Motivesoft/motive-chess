// motive-chess-uci.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "Engine.h"
#include "Logger.h"

std::vector<std::string> getUciCommands();
void logSanitizedInput( std::vector<std::string> input );
bool processCommandLineArguments( Engine& engine, int argc, char** argv );
bool processUciCommand( Engine& engine, std::vector<std::string> input );

int main( int argc, char** argv )
{
    //std::ofstream logfile;
    //logfile.open("./log.txt");
    //Logger::configure( &logfile );

    // Default logging setup
    LOG_LEVEL( Logger::Level::INFO );

    Broadcaster broadcaster( std::cout );
    Engine engine( broadcaster );

    // Process command line
    if ( processCommandLineArguments( engine, argc, argv ) )
    {
        LOG_DEBUG << "Starting";

        // Initialize list of UCI commands
        std::vector<std::string> uci = getUciCommands();

        std::vector<std::string> input;
        std::string line;
        while ( std::getline( std::cin, line ) )
        {
            input.clear();

            LOG_DEBUG << "Raw input: [" << line << "]";

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
        LOG_DEBUG << "Exiting";
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

    // Special testing commands - perft
    uci.push_back( "eval" );
    uci.push_back( "perft" );
    uci.push_back( "eval" );

    return uci;
}

bool processCommandLineArguments( Engine& engine, int argc, char** argv )
{
    std::vector<std::string> arguments;
    for ( int loop = 1; loop < argc; loop++ )
    {
        arguments.push_back( argv[ loop ] );
    }

    std::vector<std::string>::iterator it;

    // Check for logging configuration options before actually logging anything

    it = std::find( arguments.begin(), arguments.end(), "verbose" );
    if ( it != arguments.end() )
    {
        Logger::configure( Logger::Level::TRACE );
    }

    it = std::find( arguments.begin(), arguments.end(), "debug" );
    if ( it != arguments.end() )
    {
        Logger::configure( Logger::Level::DEBUG );
    }

    it = std::find( arguments.begin(), arguments.end(), "quiet" );
    if ( it != arguments.end() )
    {
        Logger::configure( Logger::Level::ERROR );
    }

    it = std::find( arguments.begin(), arguments.end(), "silent" );
    if ( it != arguments.end() )
    {
        Logger::configure( Logger::Level::NONE );
    }

    // Don't count argv[0] as an argument, or list it here
    LOG_INFO << arguments.size() << " command line argument(s)";
    for ( it = arguments.begin(); it != arguments.end(); it++ )
    {
        LOG_INFO << "  " << *it;
    }

    // Now check for other instructions

    it = std::find( arguments.begin(), arguments.end(), "bench" );
    if ( it != arguments.end() )
    {
        engine.setBenchmarking( true );
    }

    it = std::find( arguments.begin(), arguments.end(), "help" );
    if ( it != arguments.end() )
    {
        std::cout << std::endl << "Supported command line arguments..." << std::endl;
        std::cout << std::endl << "Logging:" << std::endl;
        std::cout << "  silent  - no logging" << std::endl;
        std::cout << "  quiet   - minimal logging" << std::endl;
        std::cout << "  debug   - detailed logging" << std::endl;
        std::cout << "  verbose - trace logging" << std::endl;
        std::cout << std::endl << "Information (application exits immediately):" << std::endl;
        std::cout << "  help    - this help" << std::endl;
        std::cout << "  version - version information" << std::endl;
        std::cout << std::endl << "Configuration:" << std::endl;
        std::cout << "  bench   - run in benchmark mode" << std::endl;

        return false;
    }

    it = std::find( arguments.begin(), arguments.end(), "version" );
    if ( it != arguments.end() )
    {
        // TODO extract this from resources?
        std::cout << "Motive Chess version 0.0" << std::endl;

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

    LOG_DEBUG << "Processing " << command;

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

    LOG_DEBUG << "Quit state is " << quit;

    return !quit;
}