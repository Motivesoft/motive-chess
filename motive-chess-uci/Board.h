#pragma once

#include <array>

#include "Fen.h"
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

    // This is a precise equality check, not a "is this the same position" check
    bool positionMatch( const Board& board )
    {
        for ( int loop = 0; loop < 64; loop++ )
        {
            if ( pieces[ loop ] != board.pieces[ loop ] )
            {
                return false;
            }
        }

        if ( activeColor != board.activeColor )
        {
            return false;
        }

        for ( int loop = 0; loop < 4; loop++ )
        {
            if ( castling[ loop ] != board.castling[ loop ] )
            {
                return false;
            }
        }

        if ( enPassantIndex != board.enPassantIndex )
        {
            return false;
        }

        if ( halfmoveClock != board.halfmoveClock )
        {
            return false;
        }

        if ( fullmoveNumber != board.fullmoveNumber )
        {
            return false;
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
           unsigned short enPassantIndex,
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

    Board( Fen& fen ) :
        pieces( fen.board ),
        activeColor( fen.activeColor ),
        castling { fen.castlingWK, fen.castlingWQ, fen.castlingBK, fen.castlingBQ },
        enPassantIndex( fen.enPassantIndex ),
        halfmoveClock( fen.halfmoveClock ),
        fullmoveNumber( fen.fullmoveNumber )
    {

    }

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

    // 

    /// <summary>
    /// Performs an "is this the same position" check, not a precise equality check
    /// </summary>
    /// <param name="board">the board to compare with</param>
    /// <returns>true if the on-board position is the same</returns>
    bool isSamePosition( const Board& board )
    {
        for ( int loop = 0; loop < 64; loop++ )
        {
            if ( pieces[ loop ] != board.pieces[ loop ] )
            {
                return false;
            }
        }

        if ( activeColor != board.activeColor )
        {
            return false;
        }

        return true;
    }
};

