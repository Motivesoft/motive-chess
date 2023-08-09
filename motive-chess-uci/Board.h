#pragma once

#include <array>

#include "Piece.h"

class Board
{
private:
    std::array<unsigned char, 64> pieces;
    unsigned char activeColor;
    bool castling[ 4 ]; // KQkq
    unsigned short enPassantIndex;
    unsigned short halfmoveClock;
    unsigned short fullmoveNumber;

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
        pieces( std::array< unsigned char, 64>() ),
        activeColor( Piece::WHITE ),
        castling { true, true, true, true },
        enPassantIndex( USHRT_MAX ),
        halfmoveClock( 0 ),
        fullmoveNumber( 0 )
    {
        std::fill( pieces.begin(), pieces.end(), Piece::NOTHING );
    };

    Board( std::array< unsigned char, 64 > pieces,
           unsigned char activeColor,
           bool castlingWK,
           bool castlingWQ,
           bool castlingBK,
           bool castlingBQ,
           unsigned short enPassanIndex,
           unsigned short halfmoveClock,
           unsigned short fullmoveNumber ) :
        pieces( pieces ),
        activeColor( activeColor ),
        castling { castlingWK, castlingWQ, castlingBK, castlingBQ },
        enPassantIndex( enPassantIndex ),
        halfmoveClock( halfmoveClock ),
        fullmoveNumber( fullmoveNumber )
    {
        // TODO implement this
    };

    Board( Board& board ) :
        pieces( board.pieces ),
        activeColor( board.activeColor ),
        castling { board.castling[ 0 ], board.castling[ 1 ], board.castling[ 2 ], board.castling[ 3 ] },
        enPassantIndex( board.enPassantIndex ),
        halfmoveClock( board.halfmoveClock ),
        fullmoveNumber( board.fullmoveNumber )
    {
        // TODO
    };

    virtual ~Board()
    {

    }

    bool operator == ( const Board& board )
    {
        // TODO implement other attrs
        return positionMatch( board );
    }

    bool operator != ( const Board& board )
    {
        // TODO implement other attrs
        return !positionMatch( board );
    }
};

