#pragma once

#include <string>
#include <vector>

class GoContext
{
private:
    std::vector<std::string> searchMoves;
    bool ponder;
    int wtime;
    int btime;
    int winc;
    int binc;
    int movestogo;
    int depth;
    int nodes;
    int mate;
    int movetime;
    bool infinite;

public:
    GoContext( std::vector<std::string> searchMoves, bool ponder, int wtime, int btime, int winc, int binc, int movestogo, int depth, int nodes, int mate, int movetime, bool infinite ) :
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

    GoContext( GoContext& context ) :
        searchMoves( context.searchMoves ),
        ponder( context.ponder ),
        wtime( context.wtime ),
        btime( context.btime ),
        winc( context.winc ),
        binc( context.binc ),
        movestogo( context.movestogo ),
        depth( context.depth ),
        nodes( context.nodes ),
        mate( context.mate ),
        movetime( context.movetime ),
        infinite( context.infinite )
    {

    }

    virtual ~GoContext()
    {

    }
};

