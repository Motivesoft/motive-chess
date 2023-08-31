#pragma once

#include "Board.h"

class Evaluation
{
private:
    static short pieceWeights[ 8 ];
    static short pawnAdvancementWhite[ 8 ];
    static short pawnAdvancementBlack[ 8 ];
    static short pawnAdvancementFile[ 8 ];

public:
    static short score( const Board& board );
};

