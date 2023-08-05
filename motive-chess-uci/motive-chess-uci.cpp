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
bool configureLogging( int argc, char** argv );
bool processCommandLineArguments( Engine& engine, int argc, char** argv );
bool processUciCommand( Engine& engine, std::vector<std::string> input );


bool processCommandLineArguments( int argc,
                                  char** argv,
                                  bool* benchmarking, Streams& streams )
{
    *benchmarking = true;

    if ( argc > 1 )
    {
        streams.setInputFile( "C:/Projects/GitHub/motive-chess/x64/Debug/in.txt" );
    }
    if ( argc > 2 )
    {
        streams.setOutputFile( "C:/Projects/GitHub/motive-chess/x64/Debug/out.txt" );
    }
    if ( argc > 3 )
    {
        streams.setLogFile( "C:/Projects/GitHub/motive-chess/x64/Debug/log.txt" );
    }

    return true;
}

int main( int argc, char** argv )
{
    bool b;

    Streams streams;
    processCommandLineArguments( argc, argv, &b, streams );
    std::string x;

    std::getline( *streams.getInputStream(), x );
    *streams.getOuputStream() << "[UCI] Got: [" << x << "] with benchmarking: " << b << std::endl;
    *streams.getLogStream() << "[LOG] Got: [" << x << "] with benchmarking: " << b << std::endl;
    if ( 0 == 0 )return 0;


    // Allow problems during logging configuration to tear us down
    if ( !configureLogging( argc, argv ) )
    {
        return -1;
    }

    // Debugging purposes
    //   std::ifstream infile;
    //   infile.open( "C:/Projects/GitHub/motive-chess/x64/Debug/x.txt" );
    //   std::istream* inputStream = &infile;// std::cin;
    std::istream* inputStream = &std::cin;

    // Configure output location for where to post our UCI responses
    Broadcaster broadcaster( std::cout );
    Engine engine( broadcaster );

    // Process command line
    if ( processCommandLineArguments( engine, argc, argv ) )
    {
        LOG_DEBUG << "Starting";

        engine.open();

        // Initialize list of UCI commands
        std::vector<std::string> uci = getUciCommands();

        std::vector<std::string> input;
        std::string line;
        while ( std::getline( *inputStream, line ) )
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

    // Special testing command - perft
    uci.push_back( "perft" );

    return uci;
}

bool configureLogging( int argc, char** argv )
{

    // TODO remove this temporary setup code
    std::ofstream logfile;
    logfile.open( "./motive-chess.log" );
    Logger::configure( &logfile );

    // Default logging setup
    // TODO set this to INFO
    LOG_LEVEL( Logger::Level::TRACE );

    return true;
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
    // Special perft command
    else if ( command == "perft" )
    {
        engine.perftCommand( input );
    }

    LOG_DEBUG << "Quit state is " << quit;

    return !quit;
}