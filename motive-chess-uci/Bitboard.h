#pragma once

#include <memory>
#include <mutex>

#define Bitboards Bitboard::getInstance()

class Bitboard
{
private:
    static std::unique_ptr<Bitboard> instance;

    static unsigned long long pawnMoves[ 64 ];
    static unsigned long long pawnCaptures[ 64 ]; // Can include ep by checking the EP square
    static unsigned long long pawnDoubleMoves[ 64 ]; // NB valid only if pawnMoves for same square would also be valid
    static unsigned long long knightMoves[ 64 ];

    static void buildBitboards();

    inline static std::mutex creationMutex;

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
};