#pragma once

#include "Board.h"
#include "Move.h"

class Evaluation
{
private:
    static short pieceWeights[ 8 ];
    static short pawnAdvancementWhite[ 8 ];
    static short pawnAdvancementBlack[ 8 ];
    static short pawnAdvancementFile[ 8 ];

public:
    static short scorePosition( Board board );

    static short minimax( Board board, unsigned short depth, short alpha, short beta, bool maximising );
};

