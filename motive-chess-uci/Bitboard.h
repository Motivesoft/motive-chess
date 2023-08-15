#pragma once

#include <bitset>
#include <memory>
#include <mutex>

#define Bitboards Bitboard::getInstance()

class Bitboard
{
private:
    static std::unique_ptr<Bitboard> instance;

    static unsigned long long pawnMoves[ 64 ];
    static unsigned long long pawnCaptures[ 64 ]; // Can include ep by checking the EP square
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

    inline unsigned long long getPawnMoves( unsigned short index )
    {
        return pawnMoves[ index ];
    }

    inline unsigned long long getKnightMoves( unsigned short index )
    {
        return knightMoves[ index ];
    }

    inline unsigned long long getQueenMoves( unsigned short index )
    {
        return queenMoves[ index ];
    }

    inline unsigned long long getKingMoves( unsigned short index )
    {
        return kingMoves[ index ];
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

        for ( int rloop = forigin; rloop < 7; rloop++ )
        {
            for ( int floop = rorigin; floop < 7; floop++ )
            {
                value |= 1ull << ((rloop<<3)+floop);
            }
        }



        return value;
    }

    unsigned long long getAntiDiagonalMask( unsigned short file, unsigned short rank )
    {
        unsigned long long value = 0x0101010101010101;

        value <<= file;

        return value;
    }
};
