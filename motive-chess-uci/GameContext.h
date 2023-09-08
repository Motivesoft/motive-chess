#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Fen.h"
#include "Move.h"

// Represents a specific, individual game within the UCI session

class GameContext
{
private:
    Fen fen;
    std::vector<Move> moves;

public:
    GameContext( Fen fen, std::vector<Move>& moves ) :
        fen( fen ),
        moves( moves )
    {

    }

    Fen& getFEN()
    {
        return fen;
    }

    std::vector<Move>& getMoves()
    {
        return moves;
    }
};

