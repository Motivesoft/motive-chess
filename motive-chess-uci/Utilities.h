#pragma once

#include <array>
#include <bitset>
#include <source_location>
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

    /// <summary>
    /// Turns 0-based file and rank into an index
    /// </summary>
    /// <param name="file">0-7, representing A-H</param>
    /// <param name="rank">0-7, representing rank</param>
    /// <returns></returns>
    inline static unsigned short squareToIndex( unsigned short file, unsigned short rank )
    {
        return ( rank << 3 ) + ( file & 7 );
    }

    inline static std::string indexToSquare( const unsigned short index, const bool lowercase = true )
    {
        if ( Utilities::isOffboard( index ) )
        {
            // Deliberately off-board
            return "";
        }

        std::stringstream square;

        square << (char)( ( index & 7 ) + ( lowercase ? 'a' : 'A' ) )
               << (char)( ( ( index >> 3 ) & 7 ) + '1' );

        return square.str();
    }

    /// <summary>
    /// Return a 0-based file for an index
    /// </summary>
    /// <param name="index">the square index</param>
    /// <returns>0-7 indicating A through H</returns>
    inline static unsigned short indexToFile( unsigned short index )
    {
        return index & 7;
    }

    /// <summary>
    /// Return a 0-based rank for an index
    /// </summary>
    /// <param name="index">the square index</param>
    /// <returns>0-7 indicating the rank</returns>
    inline static unsigned short indexToRank( unsigned short index )
    {
        return (index >> 3) & 7;
    }

    inline static bool isOffboard( unsigned short index )
    {
        return index == USHRT_MAX;
    }

    inline static unsigned short getOffboardLocation()
    {
        return USHRT_MAX;
    }

    static void dumpBoard( std::array< unsigned char, 64>& pieces, const std::source_location location = std::source_location::current() );
    static void dumpBitboard( unsigned long long pieces, const std::source_location location = std::source_location::current() );
    static void dump0x88( std::bitset<128> bits, const std::source_location location = std::source_location::current() );
    static void dumpBitmask( unsigned long long bits, const std::source_location location = std::source_location::current() );
};

