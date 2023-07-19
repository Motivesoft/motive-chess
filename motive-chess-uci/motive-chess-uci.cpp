// motive-chess-uci.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include "Engine.h"

#define DEBUG

std::vector<std::string> getUciCommands();
bool processUciCommand( Engine& engine, std::vector<std::string> input );

int main( int argc, char** argv )
{
    Engine engine;

    // Initialize list of UCI commands
    std::vector<std::string> uci = getUciCommands();

    std::vector<std::string> input;
    std::string line;
    while( std::getline( std::cin, line ) )
    {
        input.clear();

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
        while ( std::find( uci.begin(), uci.end(), *input.begin() ) == uci.end() )
        {
            input.erase( input.begin() );
            if ( input.size() == 0 )
            {
                break;
            }
        }
        
        // If nothing left, loop around
        if ( input.size() == 0 )
        {
            continue;
        }

#ifdef DEBUG
        // Dump the sanitized input
        bool first = true;
        std::cout << "Input: [";
        for ( std::vector<std::string>::iterator it = input.begin(); it != input.end(); it++ )
        {
            if ( first )
            {
                first = false;
            }
            else
            {
                std::cout << " ";
            }

            std::cout << *it;
        }
        std::cout << "]" << std::endl;
#endif

        // Process command input until told to quit
        if ( !processUciCommand( engine, input ) )
        {
            break;
        }
    }

    return 0;
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

    return uci;
}

bool processUciCommand( Engine& engine, std::vector<std::string> input )
{
    bool keepRunning = true;

    if ( input[ 0 ] == "uci" )
    {
        engine.initialize();
    }
    else if ( input[ 0 ] == "debug" )
    {

    }
    else if ( input[ 0 ] == "isready" )
    {

    }
    else if ( input[ 0 ] == "setoption" )
    {

    }
    else if ( input[ 0 ] == "register" )
    {

    }
    else if ( input[ 0 ] == "ucinewgame" )
    {

    }
    else if ( input[ 0 ] == "position" )
    {

    }
    else if ( input[ 0 ] == "go" )
    {

    }
    else if ( input[ 0 ] == "stop" )
    {

    }
    else if ( input[ 0 ] == "ponderhit" )
    {

    }
    else if ( input[ 0 ] == "quit" )
    {
        keepRunning = false;
    }

    return keepRunning;
}