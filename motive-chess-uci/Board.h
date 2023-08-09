#pragma once

#include <array>

#include "Fen.h"

class Board
{
private:
    std::array<unsigned char, 64> pieces;

    bool positionMatch( const Board& board )
    {
        // TODO keep this up to date with any additional state
        for ( int loop = 0; loop < 64; loop++ )
        {
            if ( pieces[ loop ] != board.pieces[ loop ] )
            {
                return false;
            }
        }
        return true;
    }

public:
    Board() :
        pieces( std::array< unsigned char, 64>() )
    {
        // TODO include other attributes
        for ( int loop = 0; loop < 64; loop++ )
        {
            pieces[ loop ] = Piece::NOTHING;
        }
    };

    Board( Fen fenString ) :
        pieces( std::array< unsigned char, 64>() )
    {
        // TODO implement this
    };

    Board( Board& board ) :
        pieces( std::array< unsigned char, 64>() )
    {
        // TODO copy other attributes
        for ( int loop = 0; loop < 64; loop++ )
        {
            pieces[ loop ] = board.pieces[ loop ];
        }
    };

    virtual ~Board()
    {

    }

    bool operator == ( const Board& board )
    {
        return positionMatch( board );
    }

    bool operator != ( const Board& board )
    {
        return !positionMatch( board );
    }
};

