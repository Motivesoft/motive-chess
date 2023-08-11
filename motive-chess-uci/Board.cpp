#include "Board.h"

Board Board::makeMove( const Move& move )
{
    Board board( *this );

    LOG_DEBUG << "Make move: " << Move::toString( move );

    if ( move.isNullMove() )
    {
        // To be honest, not sure what to do here - return an unchanged board, or an updated one with no move made
        // but other attributes updated as though a move had been made and it was now the other side's go
        LOG_TRACE << "Ignoring null move";
        return board;
    }

    // Store this for later tests
    unsigned char movingPiece = board.pieces[ move.getFrom() ]; // Will not be Piece::NOTHING
    unsigned char capturedPiece = board.pieces[ move.getTo() ]; // May be Piece::NOTHING

    // Make the main part of the move and then check for other actions

    board.pieces[ move.getTo() ] = movingPiece;
    board.pieces[ move.getFrom() ] = Piece::NOTHING;

    // Handle castling - check the piece that has just moved and work it out from there
    if ( movingPiece == Piece::WKING )
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
        board.castling[ 0 ] = false;
        board.castling[ 1 ] = false;
    }
    else if ( movingPiece == Piece::BKING )
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
        board.castling[ 2 ] = false;
        board.castling[ 3 ] = false;
    }
    else if ( movingPiece == Piece::WROOK )
    {
        // A white rook has moved - work out which and disable its ability to castle
        if ( move.getFrom() == Board::H1 )
        {
            board.castling[ 0 ] = false;
        }
        else if ( move.getFrom() == Board::A1 )
        {
            board.castling[ 1 ] = false;
        }
    }
    else if ( movingPiece == Piece::BROOK )
    {
        // A black rook has moved - work out which and disable its ability to castle
        if ( move.getFrom() == Board::H8 )
        {
            board.castling[ 2 ] = false;
        }
        else if ( move.getFrom() == Board::A8 )
        {
            board.castling[ 3 ] = false;
        }
    }

    LOG_TRACE << "Castling set to " 
        << ( board.castling[ 0 ] ? "K" : "" )
        << ( board.castling[ 1 ] ? "Q" : "" )
        << ( board.castling[ 2 ] ? "k" : "" )
        << ( board.castling[ 3 ] ? "q" : "" );

    // Promotions

    if ( move.getPromotionPiece() != Piece::NOTHING )
    {
        if ( Utilities::indexToRank( move.getTo() ) == 8 )
        {
            board.pieces[ move.getTo() ] = Piece::toColor( move.getPromotionPiece(), Piece::WHITE );

            LOG_TRACE << "Handling white promotion to " << Piece::toFENString( move.getPromotionPiece() );
        }
        else if ( Utilities::indexToRank( move.getTo() ) == 1 )
        {
            board.pieces[ move.getTo() ] = Piece::toColor( move.getPromotionPiece(), Piece::BLACK );

            LOG_TRACE << "Handling black promotion to " << Piece::toFENString( move.getPromotionPiece() );
        }
    }

    // En-passant

    if ( move.getTo() == enPassantIndex )
    {
        if ( Piece::isPawn( movingPiece ) )
        {
            LOG_TRACE << "Handling en-passant capture at " << Utilities::indexToSquare( board.enPassantIndex );

            unsigned short enPassantFile = Utilities::indexToFile( enPassantIndex );

            // An en-passant capture is happening. Remove the enemy pawn
            if ( Utilities::indexToRank( enPassantIndex ) == 3 )
            {
                board.pieces[ Utilities::squareToIndex( enPassantFile, 4 ) ] = Piece::NOTHING;
            }
            else if( Utilities::indexToRank( enPassantIndex ) == 6 )
            {
                board.pieces[ Utilities::squareToIndex( enPassantFile, 5 ) ] = Piece::NOTHING;
            }
        }
    }

    // Swap whose move it is
    board.activeColor = activeColor == Piece::WHITE ? Piece::BLACK : Piece::WHITE;
    LOG_TRACE << "Active color now " << ( board.activeColor == Piece::WHITE ? "White" : "Black" );

    // TODO Clear this but then determine whether this new move sets it again
    board.enPassantIndex = USHRT_MAX;

    if ( Piece::isPawn( movingPiece ) )
    {
        unsigned short file = Utilities::indexToFile( move.getFrom() );

        if ( Utilities::indexToRank( move.getFrom() ) == 2 && Utilities::indexToRank( move.getTo() ) == 4 )
        {
            board.enPassantIndex = Utilities::squareToIndex( file, 3 );
        }
        else if ( Utilities::indexToRank( move.getFrom() ) == 7 && Utilities::indexToRank( move.getTo() ) == 5 )
        {
            board.enPassantIndex = Utilities::squareToIndex( file, 6 );
        }

        LOG_TRACE << "En-passant square set to " << Utilities::indexToSquare( board.enPassantIndex );
    }

    // Halfmove increment? Only if not a capture or pawn move
    if ( capturedPiece == Piece::NOTHING && !Piece::isPawn( movingPiece ) )
    {
        board.halfmoveClock++;
        
        LOG_TRACE << "Adding one to halfmove clock. Now " << board.halfmoveClock;
    }

    // Increment move number
    if ( board.activeColor == Piece::WHITE )
    {
        board.fullmoveNumber++;

        LOG_TRACE << "Full move incrementing to " << board.fullmoveNumber;
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
