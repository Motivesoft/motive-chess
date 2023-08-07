#pragma once

#include <sstream>
#include <string>

/// <summary>
/// Static utility methods for mapping and suchlike
/// 
///     a  b  c  d  e  f  g  h
///    -------------------------
/// 8 | 56 57 58 59 60 61 62 63 |
/// 7 | 48                      |
/// 6 | 40                      |
/// 5 | 32                      |
/// 4 | 24                      |
/// 3 | 16                      |
/// 2 | 08                      |
/// 1 | 00 01 02 03 04 05 06 07 |
///    -------------------------
///
/// For performance, assume input values are valid
/// </summary>
class Utilities
{
public:
    inline static unsigned short squareToIndex( const std::string& square )
    {
        // Support upper and lower case files
        unsigned short a = ( square[ 0 ] >= 'a' && square[ 0 ] <= 'h' ) ? 'a' : 'A';

        unsigned short file = square[ 0 ] - a;
        unsigned short rank = square[ 1 ] - '1';

        return ( rank << 3 ) + ( file & 7 );
    }

    inline static unsigned short squareToIndex( unsigned short file, unsigned short rank )
    {
        return ( rank << 3 ) + ( file & 7 );
    }

    inline static std::string indexToSquare( const unsigned short index, const bool lowercase = true )
    {
        std::stringstream square;

        square << (char)( ( index & 7 ) + ( lowercase ? 'a' : 'A' ) )
               << (char)( ( ( index >> 3 ) & 7 ) + '1' );

        return square.str();
    }

    inline static unsigned short indexToFile( unsigned short index )
    {
        return index & 7;
    }

    inline static unsigned short indexToRank( unsigned short index )
    {
        return (index >> 3) & 7;
    }

};

