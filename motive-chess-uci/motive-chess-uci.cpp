// motive-chess-uci.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#define DEBUG

int main( int argc, char** argv )
{
    std::cout << "MotiveChess" << std::endl;

    // Initialize list of UCI commands
    std::vector<std::string> uci;
    uci.push_back( "uci" );
    uci.push_back( "quit" );

    std::vector<std::string> input;
    std::string line;
    while( std::getline( std::cin, line ) )
    {
        // Tokenize the input into a list of strings
        input.clear();

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

        // Dump the sanitized input
#ifdef DEBUG
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

        if ( line == "quit" || 
             ( !input.empty() && input[ 0 ] == "quit" ) )
        {
            break;
        }
    }

    return 0;
}
