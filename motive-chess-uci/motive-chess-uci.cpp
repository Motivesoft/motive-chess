// motive-chess-uci.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

int main( int argc, char** argv )
{
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

        // 
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

        if ( line == "quit" || 
             ( !input.empty() && input[ 0 ] == "quit" ) )
        {
            break;
        }
    }

    return 0;
}
