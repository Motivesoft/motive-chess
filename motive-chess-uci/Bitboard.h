#pragma once

#include <bitset>
#include <memory>
#include <mutex>

#include "Logger.h"

#define Bitboards Bitboard::getInstance()

class Bitboard
{
private:
    static std::unique_ptr<Bitboard> instance;

    static unsigned long long whitePawnMoves[ 64 ];
    static unsigned long long whitePawnCaptures[ 64 ];
    static unsigned long long blackPawnMoves[ 64 ];
    static unsigned long long blackPawnCaptures[ 64 ];

    static unsigned long long knightMoves[ 64 ];
    static unsigned long long bishopMoves[ 64 ];
    static unsigned long long rookMoves[ 64 ];
    static unsigned long long queenMoves[ 64 ];
    static unsigned long long kingMoves[ 64 ];

    static unsigned long long indexBitTable[ 64 ];

    static void buildBitboards();

    inline static std::mutex creationMutex;

    static unsigned long long bitboardFrom0x88( std::bitset<128>& bits );
    static unsigned long long rotate180( unsigned long long x );

public:
    static Bitboard* getInstance()
    {
        creationMutex.lock();
        if ( !instance )
        {
            instance = std::make_unique<Bitboard>();
            buildBitboards();
        }
        creationMutex.unlock();

        return instance.get();
    }

    inline unsigned long long getPawnMoves( unsigned short index, bool isWhite )
    {
        return isWhite ? whitePawnMoves[ index ] : blackPawnMoves[ index ];
    }

    inline unsigned long long getPawnCaptures( unsigned short index, bool isWhite )
    {
        return isWhite ? whitePawnCaptures[ index ] : blackPawnCaptures[ index ];
    }

    inline unsigned long long getKnightMoves( unsigned short index )
    {
        return knightMoves[ index ];
    }

    inline unsigned long long getBishopMoves( unsigned short index )
    {
        return bishopMoves[ index ];
    }

    inline unsigned long long getRookMoves( unsigned short index )
    {
        return rookMoves[ index ];
    }

    inline unsigned long long getQueenMoves( unsigned short index )
    {
        return queenMoves[ index ];
    }

    inline unsigned long long getKingMoves( unsigned short index )
    {
        return kingMoves[ index ];
    }

    inline unsigned long long getWhiteKingsideCastlingMask()
    {
        //       hgfedcba
        return 0b01100000ull;
    }

    inline unsigned long long getBlackKingsideCastlingMask()
    {
        return getWhiteKingsideCastlingMask() << 56;
    }

    inline unsigned long long getWhiteQueensideCastlingMask()
    {
        //       hgfedcba
        return 0b00001110ull;
    }

    inline unsigned long long getBlackQueensideCastlingMask()
    {
        return getWhiteQueensideCastlingMask() << 56;
    }

    unsigned long long getFileMask( unsigned short file )
    {
        unsigned long long value = 0x0101010101010101;

        value <<= file;

        return value;
    }

    unsigned long long getRankMask( unsigned short rank )
    {
        unsigned long long value = 0xff;

        value <<= (rank * 8);

        return value;
    }

    unsigned long long getDiagonalMask( unsigned short file, unsigned short rank )
    {
        unsigned long long value = 0;

        unsigned short forigin = ( file == rank ) ? 0 : ( file < rank ) ? 0 : file-rank;
        unsigned short rorigin = ( file == rank ) ? 0 : ( file < rank ) ? rank-file : 0;

        for ( ; forigin < 8 && rorigin < 8; forigin++, rorigin++ )
        {
            value |= 1ull << ((rorigin<<3)+forigin);
        }

        return value;
    }

    unsigned long long getAntiDiagonalMask( unsigned short file, unsigned short rank )
    {
        unsigned long long value = 0;

        // Use short here, not unsigned short, as we need to check for wandering over the left-hand edge

        short rorigin = rank == ( 7 - file ) ? 0 : rank > ( 7 - file ) ? rank - ( 7 - file ) : 0;
        short forigin = rank == ( 7 - file ) ? 7 : rank > ( 7 - file ) ? 7 : file + rank;
        for ( ; rorigin < 8 && forigin >= 0; forigin--, rorigin++ )
        {
            value |= 1ull << ( ( rorigin << 3 ) + forigin );
        }

        return value;
    }

    /// <summary>
    /// Construct a mask that goes from (index+1) to the top of the board
    /// For performance, assumed index is valid, between 0-63
    /// </summary>
    /// <param name="index">a square</param>
    /// <returns>a bitmask of the squares above 'index'</returns>
    unsigned long long makeUpperMask( unsigned short index )
    {
        if ( index == 63 )
        {
            return 0ull;
        }

        return ~( ( 1ull << (index + 1) ) - 1 );
    }

    /// <summary>
    /// Construct a mask that goes from (index-1) to the bottom of the board
    /// For performance, assumed index is valid, between 0-63
    /// </summary>
    /// <param name="index">a square</param>
    /// <returns>bitmask of the squares below 'index'</returns>
    unsigned long long makeLowerMask( unsigned short index )
    {
        return ( 1ull << index ) - 1;
    }

    /// <summary>
    /// Make an inclusive mask of the squares between from and to
    /// For performance, assumed from and to are valid, between 0-63
    /// </summary>
    /// <param name="from">the lower square index</param>
    /// <param name="to">the upper square index</param>
    /// <returns>an inclusive mask between the two extents</returns>
    unsigned long long makeMask( unsigned short from, unsigned short to )
    {
        return ~makeLowerMask( from ) & ~makeUpperMask( to );
    }

    inline unsigned long long indexToBit( unsigned short index )
    {
        return indexBitTable[ index ]; 
    }

    // When called in a while loop, extracts each bit of source as an unsigned short index
    inline bool getEachIndexForward( unsigned short* index, unsigned long long& source )
    {
        unsigned long sourceIndex;
        if ( _BitScanForward64( &sourceIndex, source ) )
        {
            // Now we've read a bit, remove it from source using this lookup table
            source ^= indexBitTable[ sourceIndex ];

            // Cast the result to an unsigned short
            *index = static_cast<unsigned short>( sourceIndex );
            return true;
        }

        return false;
    }

    inline bool getEachIndexReverse( unsigned short* index, unsigned long long& source )
    {
        unsigned long sourceIndex;
        if ( _BitScanReverse64( &sourceIndex, source ) )
        {
            // Now we've read a bit, remove it from source using this lookup table
            source ^= indexBitTable[ sourceIndex ];

            // Cast the result to an unsigned short
            *index = static_cast<unsigned short>( sourceIndex );
            return true;
        }

        return false;
    }
};
