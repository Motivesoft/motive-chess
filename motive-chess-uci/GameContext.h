#pragma once

#include <string>
#include <vector>

#include "Fen.h"
#include "Move.h"

// Represents a specific, individual game within the UCI session

class GameContext
{
private:
    const Fen fen;
    const std::vector<Move> moves;

public:
    GameContext( Fen fen, std::vector<Move> moves ) :
        fen( fen ),
        moves( moves )
    {

    }

    const Fen& getFEN()
    {
        return fen;
    }

    const std::vector<Move>& getMoves()
    {
        return moves;
    }
};

