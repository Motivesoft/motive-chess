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
    const Fen fen;
    const std::vector<std::shared_ptr<Move>> moves;

public:
    GameContext( Fen fen, std::vector<std::shared_ptr<Move>>& moves ) :
        fen( fen ),
        moves( moves )
    {

    }

    const Fen& getFEN() const
    {
        return fen;
    }

    const std::vector<std::shared_ptr<Move>>& getMoves() const
    {
        return moves;
    }
};

