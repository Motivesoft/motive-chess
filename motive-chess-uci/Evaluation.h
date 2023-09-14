#pragma once

#include "Board.h"

class Evaluation
{
private:
    static short pieceWeights[ 8 ];

public:
    static short scorePosition( const Board& board, unsigned char color );

    static short minimax( const Board& board, unsigned short depth, short alpha, short beta, bool maximizing, unsigned char color );
};

