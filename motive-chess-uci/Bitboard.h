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

    inline unsigned long long getKingMoves( unsigned short index )
    {
        return kingMoves[ index ];
    }
};
