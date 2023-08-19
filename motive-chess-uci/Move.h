#pragma once

#include <string>

#include "Piece.h"

class Move
{
private:
    // Flags to describe the move
    inline static const unsigned char FLAG_MASK        = 0b11110000;

    inline static const unsigned char CASTLE_KINGSIDE  = 0b00100000;
    inline static const unsigned char CASTLE_QUEENSIDE = 0b01100000;

    inline static const unsigned char PROMOTION_BIT    = 0b10000000;
    inline static const unsigned char PROMOTE_KNIGHT   = 0b10000000;
    inline static const unsigned char PROMOTE_BISHOP   = 0b10010000;
    inline static const unsigned char PROMOTE_ROOK     = 0b10100000;
    inline static const unsigned char PROMOTE_QUEEN    = 0b10110000;

    unsigned short from;
    unsigned short to;
    unsigned char flag;
    
    // TODO need a way to drop this and just use the flags above - but relies on calling code 
    // handling colorizing the piece
    unsigned char promotion;

    Move( unsigned short from, 
          unsigned short to, 
          unsigned char promotion = Piece::emptyPiece(),
          bool castlingKingside = false,
          bool castlingQueenside = false ) : 
        from( from ),
        to( to ),
        promotion( promotion )
    {
        flag = 0;

        // TODO See if we can improve this by matching Piece constants to Promote constants
        // - should work as we can assume the provided value is already legal
        flag |= Piece::isQueen( promotion ) ? PROMOTE_QUEEN : 0;
        flag |= Piece::isRook( promotion ) ? PROMOTE_ROOK : 0;
        flag |= Piece::isBishop( promotion ) ? PROMOTE_BISHOP : 0;
        flag |= Piece::isKnight( promotion ) ? PROMOTE_KNIGHT : 0;

        flag |= castlingKingside ? CASTLE_KINGSIDE : 0;
        flag |= castlingQueenside ? CASTLE_QUEENSIDE : 0;
    }

public:
    static Move fromString( const std::string& moveString );

    static Move createKingsideCastlingMove( unsigned short from, unsigned short to )
    {
        return Move( from, to, Piece::emptyPiece(), true, false );
    }

    static Move createQueensideCastlingMove( unsigned short from, unsigned short to )
    {
        return Move( from, to, Piece::emptyPiece(), false, true );
    }

    static Move createPromotionMove( unsigned short from, unsigned short to, unsigned char promotion )
    {
        return Move( from, to, promotion );
    }

    static Move createMove( unsigned short from, unsigned short to )
    {
        return Move( from, to );
    }

    static const Move nullMove;

    Move( Move& move ) :
        from( move.from ),
        to( move.to ),
        flag( move.flag ),
        promotion( move.promotion )
    {
    }

    Move( const Move& move ) :
        from( move.from ),
        to( move.to ),
        flag( move.flag ),
        promotion( move.promotion )
    {
    }

    virtual ~Move()
    {

    }

    bool operator == ( const Move& move ) const
    {
        return ( move.from == from && move.to == to && move.flag == flag && move.promotion == promotion );
    }

    bool operator != ( const Move& move ) const
    {
        return !( move.from == from && move.to == to && move.flag == flag && move.promotion == promotion );
    }

    inline unsigned short getFrom() const
    {
        return from;
    }

    inline unsigned short getTo() const
    {
        return to;
    }

    inline unsigned char getPromotionPiece() const
    {
        // TODO redo this to get rid of promotion and use the flags - but needs caller code to colorize it
        return promotion;
    }

    inline bool isPromotion() const
    {
        return flag & PROMOTION_BIT;
    }

    inline bool isNullMove() const
    {
        return from == 0 && to == 0;
    }

    inline bool isKingsideCastle() const
    {
        return (flag & FLAG_MASK) == CASTLE_KINGSIDE;
    }

    inline bool isQueensideCastle() const
    {
        return (flag & FLAG_MASK) == CASTLE_QUEENSIDE;
    }

    inline bool isCastle() const
    {
        return isKingsideCastle() || isQueensideCastle();
    }

    std::string toString() const;
};

