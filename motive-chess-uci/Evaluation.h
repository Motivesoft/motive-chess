#pragma once

#include "Board.h"

class Evaluation
{
private:
public:
    static float scoreForWhite( const Board& board );
    static float scoreForBlack( const Board& board );

    static float score( const Board& board );
};

