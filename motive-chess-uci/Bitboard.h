#pragma once

#include <bitset>
#include <memory>
#include <mutex>

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

    static unsigned long long diagonalMask[ 64 ];
    static unsigned long long antidiagonalMask[ 64 ];

    static void buildBitboards();

    static unsigned long long bitboardFrom0x88( std::bitset<128>& bits );
    static unsigned long long rotate180( unsigned long long x );

    static unsigned long long getDiagonalMaskImpl( unsigned short file, unsigned short rank );
    static unsigned long long getAntiDiagonalMaskImpl( unsigned short file, unsigned short rank );

public:
    static void initialize()
    {
        buildBitboards();
    }

    inline static unsigned long long getPawnMoves( unsigned short index, bool isWhite )
    {
        return isWhite ? whitePawnMoves[ index ] : blackPawnMoves[ index ];
    }

    inline static unsigned long long getPawnCaptures( unsigned short index, bool isWhite )
    {
        return isWhite ? whitePawnCaptures[ index ] : blackPawnCaptures[ index ];
    }

    inline static unsigned long long getKnightMoves( unsigned short index )
    {
        return knightMoves[ index ];
    }

    inline static unsigned long long getBishopMoves( unsigned short index )
    {
        return bishopMoves[ index ];
    }

    inline static unsigned long long getRookMoves( unsigned short index )
    {
        return rookMoves[ index ];
    }

    inline static unsigned long long getQueenMoves( unsigned short index )
    {
        return queenMoves[ index ];
    }

    inline static unsigned long long getKingMoves( unsigned short index )
    {
        return kingMoves[ index ];
    }

    inline static unsigned long long getWhiteKingsideCastlingMask()
    {
        //       hgfedcba
        return 0b01100000ull;
    }

    inline static unsigned long long getBlackKingsideCastlingMask()
    {
        // This is the same as the black queenside castling mask << 56
        //       hgfedcba
        return 0b0110000000000000000000000000000000000000000000000000000000000000ull;
    }

    inline static unsigned long long getWhiteQueensideCastlingMask()
    {
        //       hgfedcba
        return 0b00001110ull;
    }

    inline static unsigned long long getBlackQueensideCastlingMask()
    {
        // This is the same as the white queenside castling mask << 56
        //       hgfedcba
        return 0b0000111000000000000000000000000000000000000000000000000000000000ull;
    }

    inline static unsigned long long getFileMask( unsigned short index )
    {
        return 0x0101010101010101ull << ( index & 0b00000111 );
    }

    inline static unsigned long long getRankMask( unsigned short index )
    {
        return 0x00000000000000FFull << ( index & 0b00111000 );
    }

    inline static unsigned long long getDiagonalMask( unsigned short index )
    {
        return diagonalMask[ index ];
    }

    inline static unsigned long long getAntiDiagonalMask( unsigned short index )
    {
        return antidiagonalMask[ index ];
    }

    /// <summary>
    /// Construct a mask that goes from (index+1) to the top of the board
    /// For performance, assumed index is valid, between 0-63
    /// </summary>
    /// <param name="index">a square</param>
    /// <returns>a bitmask of the squares above 'index'</returns>
    inline static unsigned long long makeUpperMask( unsigned short index )
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
    inline static unsigned long long makeLowerMask( unsigned short index )
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
    inline static unsigned long long makeMask( unsigned short from, unsigned short to )
    {
        return ~makeLowerMask( from ) & ~makeUpperMask( to );
    }

    inline static unsigned long long indexToBit( unsigned short index )
    {
        return indexBitTable[ index ]; 
    }

    // When called in a while loop, extracts each bit of source as an unsigned short index
    inline static bool getEachIndexForward( unsigned short* index, unsigned long long& source )
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

    inline static bool getEachIndexReverse( unsigned short* index, unsigned long long& source )
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
