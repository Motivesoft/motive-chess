#include <sstream>

#include "Log.h"
#include "Move.h"
#include "Utilities.h"

Move Move::nullMove( 0, 0 );

Move* Move::fromString( const std::string& moveString )
{
    // There is no easy way to check these moves for being captures or castling, but we don't need to in the
    // conditions that this gets used.

    if ( moveString.length() > 4 )
    {
        // Contains a promotion
        switch ( moveString[ 4 ] )
        {
            case 'k':
                return Move::createKnightPromotionMove( Utilities::squareToIndex( moveString.substr( 0, 2 ) ),
                                                        Utilities::squareToIndex( moveString.substr( 2, 2 ) ) );

            case 'b':
                return Move::createBishopPromotionMove( Utilities::squareToIndex( moveString.substr( 0, 2 ) ),
                                                        Utilities::squareToIndex( moveString.substr( 2, 2 ) ) );

            case 'r':
                return Move::createRookPromotionMove( Utilities::squareToIndex( moveString.substr( 0, 2 ) ),
                                                      Utilities::squareToIndex( moveString.substr( 2, 2 ) ) );

            case 'q':
                return Move::createQueenPromotionMove( Utilities::squareToIndex( moveString.substr( 0, 2 ) ),
                                                       Utilities::squareToIndex( moveString.substr( 2, 2 ) ) );

            default:
                Log::Error << "Unrecognised promotion piece in move: " << moveString << std::endl;
                return getNullMove();
        }
    }

    return Move::createMove( Utilities::squareToIndex( moveString.substr( 0, 2 ) ),
                             Utilities::squareToIndex( moveString.substr( 2, 2 ) ) );
}

std::string Move::toString() const
{
    std::stringstream stream;

    if ( isNullMove() )
    {
        // Special case for UCI
        return "0000";
    }

    stream << Utilities::indexToSquare( getFrom() )
           << Utilities::indexToSquare( getTo() )
           << Piece::toMoveString( getPromotionPiece( Piece::getStartingColor() ) ); // Color doesn't matter here

    return stream.str();
}
