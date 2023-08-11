#include <sstream>

#include "Move.h"
#include "Utilities.h"

Move Move::fromString( std::string& moveString )
{
    // Contains a promotion?
    if ( moveString.length() > 4 )
    {
        return Move( Utilities::squareToIndex( moveString.substr( 0, 2 ) ),
                     Utilities::squareToIndex( moveString.substr( 2, 2 ) ),
                     Piece::fromMoveString( moveString[ 4 ] ) );
    }

    return Move( Utilities::squareToIndex( moveString.substr( 0, 2 ) ),
                 Utilities::squareToIndex( moveString.substr( 2, 2 ) ) );
}

std::string Move::toString( const Move& move, bool lowercase )
{
    std::stringstream stream;

    if ( move.isNullMove() )
    {
        // Special case for UCI
        return "0000";
    }

    stream << Utilities::indexToSquare( move.from, lowercase )
           << Utilities::indexToSquare( move.to, lowercase )
           << Piece::toMoveString( move.promotion );

    return stream.str();
}
