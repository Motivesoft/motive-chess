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
    Broadcaster broadcaster( std::cout );
    Engine engine( broadcaster );

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
    bool quit = false;

    // Pop the command (first item) and leave the rest
    std::string command = input[ 0 ];
    input.erase( input.begin() );

    if ( command == "uci" )
    {
        engine.uci();
    }
    else if ( command == "debug" )
    {
        engine.debug( input );
    }
    else if ( command == "isready" )
    {
        engine.isready();
    }
    else if ( command == "setoption" )
    {

    }
    else if ( command == "register" )
    {

    }
    else if ( command == "ucinewgame" )
    {

    }
    else if ( command == "position" )
    {

    }
    else if ( command == "go" )
    {

    }
    else if ( command == "stop" )
    {

    }
    else if ( command == "ponderhit" )
    {

    }
    else if ( command == "quit" )
    {
        quit = engine.quit();
    }

    return !quit;
}