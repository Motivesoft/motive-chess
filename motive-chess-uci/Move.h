#pragma once

#include <string>

#include "Piece.h"

class Move
{
private:
    // Flags to describe the move
    inline static const unsigned short TO_MASK          = 0b0000111111000000;
    inline static const unsigned short FROM_MASK        = 0b0000000000111111;
    
    inline static const unsigned short FLAG_MASK        = 0b1111000000000000;
    inline static const unsigned short SQUARES_MASK     = 0b0000111111111111;
    inline static const unsigned short PROMOTION_MASK   = 0b0011000000000000;
    inline static const unsigned short CAPTURE_MASK     = 0b0100000000000000;
    inline static const unsigned short EP_CAPTURE_MASK  = 0b0110000000000000;

    inline static const unsigned short CASTLE_KINGSIDE  = 0b0001000000000000;
    inline static const unsigned short CASTLE_QUEENSIDE = 0b0011000000000000;

    inline static const unsigned short PROMOTION_BIT    = 0b1000000000000000;
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

    Move( unsigned short moveBits ) :
        moveBits( moveBits )
    {
        // Nothing to do
    }

public:
    class Builder
    {
    private:
        unsigned short moveBits;

    public:
        Builder( unsigned short from, unsigned short to ) :
            moveBits( 0 )
        {
            moveBits |= from;
            moveBits |= ( to << 6 );
        }
        Builder& setKingsideCastling()
        {
            // Reset anything already set into flags
            moveBits = CASTLE_KINGSIDE | (moveBits & SQUARES_MASK);
            return *this;
        }
        Builder& setQueensideCastling()
        {
            // Reset anything already set into flags
            moveBits = CASTLE_QUEENSIDE | ( moveBits & SQUARES_MASK );
            return *this;
        }
        Builder& setPromotion( unsigned char promotion )
        {
            moveBits |= Piece::isQueen( promotion ) ? PROMOTE_QUEEN : 0;
            moveBits |= Piece::isRook( promotion ) ? PROMOTE_ROOK : 0;
            moveBits |= Piece::isBishop( promotion ) ? PROMOTE_BISHOP : 0;
            moveBits |= Piece::isKnight( promotion ) ? PROMOTE_KNIGHT : 0;
            return *this;
        }
        Builder& setCapture()
        {
            moveBits |= CAPTURE_MASK;
            return *this;
        }
        Builder& setEnPassantCapture()
        {
            moveBits |= EP_CAPTURE_MASK;
            return *this;
        }
        Move build()
        {
            return Move( moveBits );
        }
    };

    static Move::Builder createBuilder( unsigned short from, unsigned short to )
    {
        return Builder( from, to );
    }

    static Move fromString( const std::string& moveString );

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
        return (moveBits & SQUARES_MASK) == 0;
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

    /// <summary>
    /// Return whether the move is a capture. This will include en passant captures
    /// </summary>
    /// <returns></returns>
    inline bool isCapture() const
    {
        return (moveBits & CAPTURE_MASK) == CAPTURE_MASK;
    }

    inline bool isEnPassantCapture() const
    {
        return (moveBits & EP_CAPTURE_MASK) == EP_CAPTURE_MASK;
    }

    std::string toString() const;
};

