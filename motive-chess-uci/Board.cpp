#include "Board.h"

Board Board::makeMove( const Move& move )
{
    Board board( *this );

    LOG_DEBUG << "Make move: " << Move::toString( move );

    // TODO this doesn't yet deal with promotions, castling and en-passant
    board.pieces[ move.getTo() ] = board.pieces[ move.getFrom() ];
    board.pieces[ move.getFrom() ] = Piece::NOTHING;

    // Handle castling - check the piece that has just moved and work it out from there
    if ( board.pieces[ move.getTo() ] == Piece::WKING )
    {
        if ( move.getFrom() == Board::E1 )
        {
            if ( move.getTo() == Board::C1 )
            {
                board.pieces[ Board::D1 ] = board.pieces[ Board::A1 ];
                board.pieces[ Board::A1 ] = Piece::NOTHING;
            }
            else if ( move.getTo() == Board::G1 )
            {
                board.pieces[ Board::F1 ] = board.pieces[ Board::H1 ];
                board.pieces[ Board::H1 ] = Piece::NOTHING;
            }
        }

        // White king has moved - no more castling
        castling[ 0 ] = false;
        castling[ 1 ] = false;
    }
    else if ( board.pieces[ move.getTo() ] == Piece::BKING )
    {
        if ( move.getFrom() == Board::E8 )
        {
            if ( move.getTo() == Board::C8 )
            {
                board.pieces[ Board::D8 ] = board.pieces[ Board::A8 ];
                board.pieces[ Board::A8 ] = Piece::NOTHING;
            }
            else if ( move.getTo() == Board::G8 )
            {
                board.pieces[ Board::F8 ] = board.pieces[ Board::H8 ];
                board.pieces[ Board::H8 ] = Piece::NOTHING;
            }
        }

        // Black king has moved - no more castling
        castling[ 2 ] = false;
        castling[ 3 ] = false;
    }
    else if ( board.pieces[ move.getTo() ] == Piece::WROOK )
    {
        // A white rook has moved - work out which and disable its ability to castle
        if ( move.getFrom() == Board::H1 )
        {
            castling[ 0 ] = false;
        }
        else if ( move.getFrom() == Board::A1 )
        {
            castling[ 1 ] = false;
        }
    }
    else if ( board.pieces[ move.getTo() ] == Piece::BROOK )
    {
        // A black rook has moved - work out which and disable its ability to castle
        if ( move.getFrom() == Board::H8 )
        {
            castling[ 2 ] = false;
        }
        else if ( move.getFrom() == Board::A8 )
        {
            castling[ 3 ] = false;
        }
    }

    // Promotions

    if ( move.getPromotionPiece() != Piece::NOTHING )
    {
        if ( Utilities::indexToRank( move.getTo() ) == 8 )
        {
            board.pieces[ move.getTo() ] = Piece::toColor( move.getPromotionPiece(), Piece::WHITE );
        }
        else if ( Utilities::indexToRank( move.getTo() ) == 1 )
        {
            board.pieces[ move.getTo() ] = Piece::toColor( move.getPromotionPiece(), Piece::BLACK );
        }
    }

    // En-passant

    // TODO it also needs to update the other state variables, e.g...

    // Swap whose move it is
    board.activeColor = activeColor == Piece::WHITE ? Piece::BLACK : Piece::WHITE;

    // Increment move number
    if ( board.activeColor == Piece::WHITE )
    {
        board.fullmoveNumber++;
    }

    /*
    castling { board.castling[ 0 ], board.castling[ 1 ], board.castling[ 2 ], board.castling[ 3 ] },
    enPassantIndex( board.enPassantIndex ),
    halfmoveClock( board.halfmoveClock ),
    fullmoveNumber( board.fullmoveNumber )
     */
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
