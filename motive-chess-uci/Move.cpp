#include <sstream>

#include "Move.h"
#include "Utilities.h"

const Move Move::nullMove( 0, 0 );

Move::Move( unsigned short from,
            unsigned short to,
            unsigned char promotion,
            bool castlingKingside,
            bool castlingQueenside )
{
    moveBits = from;
    moveBits |= to << 6;

    // TODO See if we can improve this by matching Piece constants to Promote constants
    // - should work as we can assume the provided value is already legal
    moveBits |= Piece::isQueen( promotion ) ? PROMOTE_QUEEN : 0;
    moveBits |= Piece::isRook( promotion ) ? PROMOTE_ROOK : 0;
    moveBits |= Piece::isBishop( promotion ) ? PROMOTE_BISHOP : 0;
    moveBits |= Piece::isKnight( promotion ) ? PROMOTE_KNIGHT : 0;

    moveBits |= castlingKingside ? CASTLE_KINGSIDE : 0;
    moveBits |= castlingQueenside ? CASTLE_QUEENSIDE : 0;
}

Move* Move::fromString( const std::string& moveString )
{
    // Contains a promotion?
    if ( moveString.length() > 4 )
    {
        return Move::createBuilder( Utilities::squareToIndex( moveString.substr( 0, 2 ) ),
                                    Utilities::squareToIndex( moveString.substr( 2, 2 ) ) )
            .setPromotion( Piece::promotionPieceFromMoveString( moveString ) ).build();
    }

    return Move::createBuilder( Utilities::squareToIndex( moveString.substr( 0, 2 ) ),
                                Utilities::squareToIndex( moveString.substr( 2, 2 ) ) )
        .build();
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
