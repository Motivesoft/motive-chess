#include "Board.h"

Board Board::makeMove( const Move& move )
{
    Board board( *this );

    board.applyMove( move );

    return board;
}

void Board::applyMove( const Move& move )
{
    LOG_TRACE << "Apply move: " << Move::toString( move );

    if ( move.isNullMove() )
    {
        // To be honest, not sure what to do here - return an unchanged board, or an updated one with no move made
        // but other attributes updated as though a move had been made and it was now the other side's go
        LOG_TRACE << "Ignoring null move";
        return;
    }

    // Store this for later tests
    unsigned char movingPiece = pieces[ move.getFrom() ]; // Will not be Piece::NOTHING
    unsigned char capturedPiece = pieces[ move.getTo() ]; // May be Piece::NOTHING

    // Make the main part of the move and then check for other actions

    pieces[ move.getTo() ] = movingPiece;
    pieces[ move.getFrom() ] = Piece::NOTHING;

    // Handle castling - check the piece that has just moved and work it out from there
    if ( movingPiece == Piece::WKING )
    {
        if ( move.getFrom() == Board::E1 )
        {
            if ( move.getTo() == Board::C1 )
            {
                pieces[ Board::D1 ] = pieces[ Board::A1 ];
                pieces[ Board::A1 ] = Piece::NOTHING;

                LOG_TRACE << "White castling queen side";
            }
            else if ( move.getTo() == Board::G1 )
            {
                pieces[ Board::F1 ] = pieces[ Board::H1 ];
                pieces[ Board::H1 ] = Piece::NOTHING;

                LOG_TRACE << "White castling king side";
            }
        }

        // White king has moved - no more castling
        castling[ 0 ] = false;
        castling[ 1 ] = false;
    }
    else if ( movingPiece == Piece::BKING )
    {
        if ( move.getFrom() == Board::E8 )
        {
            if ( move.getTo() == Board::C8 )
            {
                pieces[ Board::D8 ] = pieces[ Board::A8 ];
                pieces[ Board::A8 ] = Piece::NOTHING;

                LOG_TRACE << "Black castling queen side";
            }
            else if ( move.getTo() == Board::G8 )
            {
                pieces[ Board::F8 ] = pieces[ Board::H8 ];
                pieces[ Board::H8 ] = Piece::NOTHING;

                LOG_TRACE << "Black castling king side";
            }
        }

        // Black king has moved - no more castling
        castling[ 2 ] = false;
        castling[ 3 ] = false;
    }
    else if ( movingPiece == Piece::WROOK )
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
    else if ( movingPiece == Piece::BROOK )
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

    LOG_TRACE << "Castling set to " 
        << ( castling[ 0 ] ? "K" : "" )
        << ( castling[ 1 ] ? "Q" : "" )
        << ( castling[ 2 ] ? "k" : "" )
        << ( castling[ 3 ] ? "q" : "" );

    // Promotions

    if ( move.getPromotionPiece() != Piece::NOTHING )
    {
        pieces[ move.getTo() ] = move.getPromotionPiece();

        LOG_TRACE << "Handling promotion to " << Piece::toFENString( move.getPromotionPiece() );
    }

    // En-passant

    if ( move.getTo() == enPassantIndex )
    {
        if ( Piece::isPawn( movingPiece ) )
        {
            LOG_TRACE << "Handling en-passant capture at " << Utilities::indexToSquare( enPassantIndex );

            unsigned short file = Utilities::indexToFile( enPassantIndex );

            // An en-passant capture is happening. Remove the enemy pawn
            if ( Utilities::indexToRank( enPassantIndex ) == RANK_3 )
            {
                pieces[ Utilities::squareToIndex( file, RANK_4 ) ] = Piece::NOTHING;
            }
            else if( Utilities::indexToRank( enPassantIndex ) == RANK_6 )
            {
                pieces[ Utilities::squareToIndex( file, RANK_5 ) ] = Piece::NOTHING;
            }
        }
    }

    // Swap whose move it is
    activeColor = Piece::swapColor( activeColor );
    LOG_TRACE << "Active color now " << Piece::toColorString( activeColor );

    // Clear this but then determine whether this new move sets it again
    enPassantIndex = Utilities::getOffboardLocation();

    if ( Piece::isPawn( movingPiece ) )
    {
        unsigned short file = Utilities::indexToFile( move.getFrom() );

        if ( Utilities::indexToRank( move.getFrom() ) == RANK_2 && Utilities::indexToRank( move.getTo() ) == RANK_4 )
        {
            enPassantIndex = Utilities::squareToIndex( file, RANK_3 );
        }
        else if ( Utilities::indexToRank( move.getFrom() ) == RANK_7 && Utilities::indexToRank( move.getTo() ) == RANK_5 )
        {
            enPassantIndex = Utilities::squareToIndex( file, RANK_6 );
        }

        LOG_TRACE << "En-passant square: " << ( Utilities::isOffboard( enPassantIndex ) ? "none" : Utilities::indexToSquare( enPassantIndex ) );
    }
    else
    {
        LOG_TRACE << "En-passant square cleared";
    }

    // Halfmove increment? Only if not a capture or pawn move
    if ( Piece::isEmpty( capturedPiece ) && !Piece::isPawn( movingPiece ) )
    {
        halfmoveClock++;
        
        LOG_TRACE << "Adding one to halfmove clock. Now " << halfmoveClock;
    }
    else
    {
        halfmoveClock = 0;

        LOG_TRACE << "Reset halfmove clock to zero";
    }

    // Increment move number
    if ( Piece::isWhite( activeColor ) )
    {
        fullmoveNumber++;

        LOG_TRACE << "Full move incrementing to " << fullmoveNumber;
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
            stream << ( pieces[ index ] == Piece::NOTHING ?
                        ( ( index & 1 ) == 0 ? "." : " " ) :
                        Piece::toFENString( pieces[ index ] ) );
        }

        LOG_DEBUG << 1 + rankIndex / 8 << " " << stream.str() << " " << 1 + rankIndex / 8;
    }
    LOG_DEBUG << "  ABCDEFGH";
}
