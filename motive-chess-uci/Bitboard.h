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
        return 0b01100000ull;
    }

    inline unsigned long long getBlackKingsideCastlingMask()
    {
        return getWhiteKingsideCastlingMask() << 56;
    }

    inline unsigned long long getWhiteQueensideCastlingMask()
    {
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

    unsigned long long makeMask( unsigned short from, unsigned short to )
    {
        // This covers going outside at either end of the range as the inputs are unsigned
        if ( from > 63 || to > 63 )
        {
            return 0ull;
        }

        unsigned long long result = 0;
        unsigned long long mask = 1ull << from;

        if ( to < from )
        {
            return makeMask( to, from );
        }

        for ( unsigned long loop = from; loop <= to; loop++, mask <<= 1 )
        {
            result |= mask;
        }

        return result;
    }
};
