#include <sstream>

#include "Move.h"
#include "Utilities.h"

Move* Move::fromString( std::string& moveString )
{
    // Contains a promotion?
    if ( moveString.length() > 4 )
    {
        return new Move( Utilities::squareToIndex( moveString.substr( 0, 2 ) ),
                         Utilities::squareToIndex( moveString.substr( 2, 2 ) ),
                         Piece::fromMoveString( moveString[ 4 ] ) );
    }

    return new Move( Utilities::squareToIndex( moveString.substr( 0, 2 ) ),
                     Utilities::squareToIndex( moveString.substr( 2, 2 ) ) );
}

std::string Move::toString( Move* move, bool lowercase )
{
    std::stringstream stream;

    stream << Utilities::indexToSquare( move->from, lowercase )
           << Utilities::indexToSquare( move->to, lowercase )
           << Piece::toMoveString( move->promotion );

    return stream.str();
}
