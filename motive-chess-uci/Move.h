#pragma once

#include <string>

#include "Piece.h"

class Move
{
private:
    // Flags to describe the move
    inline static const unsigned short FROM_MASK        = 0b0000000000111111;
    inline static const unsigned short TO_MASK          = 0b0000111111000000;
    inline static const unsigned short FLAG_MASK        = 0b1111000000000000;

    inline static const unsigned short CAPTURE_MASK     = 0b0001000000000000;

    inline static const unsigned short CASTLE_KINGSIDE  = 0b0001000000000000;
    inline static const unsigned short CASTLE_QUEENSIDE = 0b0011000000000000;

    inline static const unsigned short PROMOTION_BIT    = 0b1000000000000000;
    inline static const unsigned short PROMOTION_MASK   = 0b0011000000000000;
    inline static const unsigned short PROMOTE_KNIGHT   = 0b1000000000000000;
    inline static const unsigned short PROMOTE_BISHOP   = 0b1001000000000000;
    inline static const unsigned short PROMOTE_ROOK     = 0b1010000000000000;
    inline static const unsigned short PROMOTE_QUEEN    = 0b1011000000000000;

    unsigned short moveBits;
    
    // Store everything in an unsigned int? 
    //  0xFFFFFFFF or 
    //  0b0000000000000000000000000000000000000000000000000000000000000000
    //  
    //  1        1
    //  2631     2631
    //  84268421 84268421
    //  -------- --------
    //  FEDCBA98 76543210
    // 
    // 0-63 from - bits 0-5
    // 0-63 to   - bits 6-B
    // flags     - bits C-F


    // TODO improve the constructor and factory methods here, maybe?
    Move( unsigned short from,
          unsigned short to,
          unsigned char promotion = Piece::emptyPiece(),
          bool castlingKingside = false,
          bool castlingQueenside = false );

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
        moveBits( move.moveBits )
    {
    }

    Move( const Move& move ) :
        moveBits( move.moveBits )
    {
    }

    virtual ~Move()
    {

    }

    inline bool operator == ( const Move& move ) const
    {
        return move.moveBits == moveBits;
    }

    inline bool operator != ( const Move& move ) const
    {
        return move.moveBits != moveBits;
    }

    inline unsigned short getFrom() const
    {
        return moveBits & FROM_MASK;
    }

    inline unsigned short getTo() const
    {
        return (moveBits & TO_MASK) >> 6;
    }

    inline unsigned char getPromotionPiece( unsigned char color ) const
    {
        if ( moveBits & PROMOTION_BIT )
        {
            // Mask moveBits and then shift it down to bits 0-1 to index into the promotion array
            return Piece::getPromotionPieces( color )[ (moveBits & PROMOTION_MASK) >> 12 ];
        }

        return Piece::emptyPiece();
    }

    inline bool isPromotion() const
    {
        return moveBits & PROMOTION_BIT;
    }

    inline bool isNullMove() const
    {
        return (moveBits & (FROM_MASK | TO_MASK)) == 0;
    }

    inline bool isKingsideCastle() const
    {
        return (moveBits & FLAG_MASK) == CASTLE_KINGSIDE;
    }

    inline bool isQueensideCastle() const
    {
        return (moveBits & FLAG_MASK) == CASTLE_QUEENSIDE;
    }

    inline bool isCastling() const
    {
        return isKingsideCastle() || isQueensideCastle();
    }

    std::string toString() const;
};

