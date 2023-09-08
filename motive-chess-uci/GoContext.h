#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Move.h"

class GoContext
{
private:
    std::vector<Move> searchMoves;
    bool ponder;
    unsigned int wtime;
    unsigned int btime;
    unsigned int winc;
    unsigned int binc;
    unsigned int movestogo;
    unsigned int depth;
    unsigned int nodes;
    unsigned int mate;
    unsigned int movetime;
    bool infinite;

public:
    GoContext( std::vector<Move>& searchMoves, bool ponder, int wtime, int btime, int winc, int binc, int movestogo, int depth, int nodes, int mate, int movetime, bool infinite ) :
        searchMoves( searchMoves ),
        ponder( ponder ),
        wtime( wtime ),
        btime( btime ),
        winc( winc ),
        binc( binc ),
        movestogo( movestogo ),
        depth( depth ),
        nodes( nodes ),
        mate( mate ),
        movetime( movetime ),
        infinite( infinite )
    {

    }

    virtual ~GoContext()
    {

    }

    inline unsigned int getDepth() const
    {
        return depth;
    }

    inline std::vector<Move>& getSearchMoves()
    {
        return searchMoves;
    }
};

