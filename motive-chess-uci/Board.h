#pragma once

#include <array>

#include "Fen.h"
#include "Move.h"
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

    Board makeMove( const Move& move )
    {
        Board board( *this );

        LOG_DEBUG << "Make move: " << Move::toString( move );

        // TODO this doesn't yet deal with promotions, castling and en-passant
        board.pieces[ move.getTo() ] = board.pieces[ move.getFrom() ];
        board.pieces[ move.getFrom() ] = Piece::NOTHING;

        // TODO it also needs to update the other state variables, e.g...
        board.activeColor = activeColor == Piece::WHITE ? Piece::BLACK : Piece::WHITE;
        if ( board.activeColor == Piece::WHITE )
        {
            board.fullmoveNumber++;
        }

        // TODO Tuning - does this call the copy constructor too often and should we move to pointers?

        // TODO stick this in a utility class somewhere
        LOG_DEBUG << "Board:";
        LOG_DEBUG << "  ABCDEFGH";
        for ( unsigned short rank = 0, rankIndex = 56; rank < 8; rank++, rankIndex -= 8 )
        {
            std::stringstream stream;
            for ( unsigned short index = rankIndex; index < rankIndex + 8; index++ )
            {
                stream << ( board.pieces[ index ] == Piece::NOTHING ?
                            ( ( index & 1 ) == 0 ? "." : " " ) :
                            Piece::toFENString( board.pieces[ index ] ) );
            }

            LOG_DEBUG << 1 + rankIndex / 8 << " " << stream.str() << " " << 1 + rankIndex / 8;
        }
        LOG_DEBUG << "  ABCDEFGH";


        return board;
    }
};

