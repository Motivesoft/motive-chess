#pragma once

#define Bitboards Bitboard::getInstance()

class Bitboard
{
private:
    static Bitboard instance;

    static unsigned long long pawnMoves[ 64 ];
    static unsigned long long pawnCaptures[ 64 ]; // Can include ep by checking the EP square
    static unsigned long long pawnDoubleMoves[ 64 ]; // NB valid only if pawnMoves for same square would also be valid
    static unsigned long long knightMoves[ 64 ];

    Bitboard()
    {
        // Singleton, so this should only be called once
        buildBitboards();
    }

    static void buildBitboards();

public:
    static Bitboard getInstance()
    {
        return instance;
    }

    inline unsigned long long getPawnMoves( unsigned short index )
    {
        return pawnMoves[ index ];
    }
};

