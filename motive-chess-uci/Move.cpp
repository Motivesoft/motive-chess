#include <sstream>

#include "Move.h"
#include "Utilities.h"

const Move Move::nullMove( 0, 0 );

Move Move::fromString( const std::string& moveString )
{
    // Contains a promotion?
    if ( moveString.length() > 4 )
    {
        return createPromotionMove( Utilities::squareToIndex( moveString.substr( 0, 2 ) ),
                                    Utilities::squareToIndex( moveString.substr( 2, 2 ) ),
                                    Piece::promotionPieceFromMoveString( moveString ) );
    }

    return createMove( Utilities::squareToIndex( moveString.substr( 0, 2 ) ),
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

    stream << Utilities::indexToSquare( from )
           << Utilities::indexToSquare( to )
           << Piece::toMoveString( promotion );

    return stream.str();
}
