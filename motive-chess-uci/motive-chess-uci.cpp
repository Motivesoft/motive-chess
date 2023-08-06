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
#include "Streams.h"

std::vector<std::string> getUciCommands();
void logSanitizedInput( std::vector<std::string> input );
bool processCommandLine( int argc, char** argv, bool* benchmarking, Streams& streams );
bool processUciCommand( Engine& engine, std::vector<std::string> input );

int main( int argc, char** argv )
{
    Streams streams;
    bool benchmarking;

    // Allow command line processing to cause an exit without further activity
    if ( processCommandLine( argc, argv, &benchmarking, streams ) )
    {
        LOG_DEBUG << "Starting";

        // Configure output location for where to post our UCI responses
        Broadcaster broadcaster( streams.getOuputStream() );
        Engine engine( broadcaster );

        engine.open();
        engine.setBenchmarking( benchmarking );

        // Initialize list of UCI commands
        std::vector<std::string> uci = getUciCommands();

        std::vector<std::string> input;
        std::string line;
        while ( streams.getLine( line ) )
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
    
    Logger::Level logLevel = Logger::Level::TRACE; // TODO set this to INFO
    
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
    }

    it = std::find( arguments.begin(), arguments.end(), "-debug" );
    if ( it != arguments.end() )
    {
        logLevel = Logger::Level::DEBUG;
    }

    it = std::find( arguments.begin(), arguments.end(), "-quiet" );
    if ( it != arguments.end() )
    {
        logLevel = Logger::Level::ERROR;
    }

    it = std::find( arguments.begin(), arguments.end(), "-silent" );
    if ( it != arguments.end() )
    {
        logLevel = Logger::Level::NONE;
    }

    // Allow control of input and output

    it = std::find( arguments.begin(), arguments.end(), "-logfile" );
    if ( it != arguments.end() )
    {
        // Take the next argument as a filename
        ++it;
        if ( it != arguments.end() )
        {
            streams.setLogFile( *it );
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
    LOG_DEBUG << arguments.size() << " command line argument(s)";
    for ( it = arguments.begin(); it != arguments.end(); it++ )
    {
        LOG_DEBUG << "  " << *it;
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
    // Special perft command
    else if ( command == "perft" )
    {
        engine.perftCommand( input );
    }

    LOG_DEBUG << "Quit state is " << quit;

    return !quit;
}