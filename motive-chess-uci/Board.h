#pragma once

#include <array>

#include "Fen.h"
#include "Move.h"
#include "Piece.h"

class Board
{
private:
    // Square indices for the first and last ranks
    inline static const unsigned short A1 = 0;
    inline static const unsigned short B1 = 1;
    inline static const unsigned short C1 = 2;
    inline static const unsigned short D1 = 3;
    inline static const unsigned short E1 = 4;
    inline static const unsigned short F1 = 5;
    inline static const unsigned short G1 = 6;
    inline static const unsigned short H1 = 7;

    inline static const unsigned short A8 = 56;
    inline static const unsigned short B8 = 57;
    inline static const unsigned short C8 = 58;
    inline static const unsigned short D8 = 59;
    inline static const unsigned short E8 = 60;
    inline static const unsigned short F8 = 61;
    inline static const unsigned short G8 = 62;
    inline static const unsigned short H8 = 63;

    inline static const unsigned short FILE_A = 0;
    inline static const unsigned short FILE_B = 1;
    inline static const unsigned short FILE_C = 2;
    inline static const unsigned short FILE_D = 3;
    inline static const unsigned short FILE_E = 4;
    inline static const unsigned short FILE_F = 5;
    inline static const unsigned short FILE_G = 6;
    inline static const unsigned short FILE_H = 7;

    inline static const unsigned short RANK_1 = 0;
    inline static const unsigned short RANK_2 = 1;
    inline static const unsigned short RANK_3 = 2;
    inline static const unsigned short RANK_4 = 3;
    inline static const unsigned short RANK_5 = 4;
    inline static const unsigned short RANK_6 = 5;
    inline static const unsigned short RANK_7 = 6;
    inline static const unsigned short RANK_8 = 7;

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

    /// <summary>
    /// Check whether this move refutes the move we're currently examining.
    /// The general idea is to generate pseudo legal moves and for each, generate the possible responses.
    /// Push the responses to this method to determine whether the psuedo legal move is indeed legal
    /// </summary>
    /// <param name="move">a move</param>
    /// <returns>true if this move should not be possible, demonstrating that the move leading to this is invalid</returns>
    bool isRefutation( const Move& move )
    {
        return Piece::isKing( pieces[ move.getTo() ] );
    }

    /// <summary>
    /// Applies move to this board
    /// </summary>
    /// <param name="move">the move</param>
    void applyMove( const Move& move );

public:
    Board() :
        pieces( std::array< unsigned char, 64>() ),
        activeColor( Piece::WHITE ),
        castling { true, true, true, true },
        enPassantIndex( USHRT_MAX ),
        halfmoveClock( 0 ),
        fullmoveNumber( 1 )
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

    Board( const Fen& fen ) :
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

    /// <summary>
    /// Returns a new board, based on the current board but with this move applied
    /// </summary>
    /// <param name="move">the move</param>
    /// <returns>a new board</returns>
    Board makeMove( const Move& move );
};

