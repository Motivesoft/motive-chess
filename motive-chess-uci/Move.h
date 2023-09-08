#pragma once

#include <assert.h>
#include <string>

#include "Log.h"
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
    //Move( unsigned short from,
    //      unsigned short to,
    //      unsigned char promotion = Piece::emptyPiece(),
    //      bool castlingKingside = false,
    //      bool castlingQueenside = false );

    static Move nullMove;

    Move( unsigned short from, unsigned short to, unsigned short additionalBits = 0 ) :
        moveBits( from | (to << 6) | additionalBits )
    {
    }

public:
    inline static Move* createMove( unsigned short from, unsigned short to, bool capture = false, bool epCapture = false )
    {
        return new Move( from, to, (unsigned short) ( capture ? CAPTURE_MASK : 0 ) | ( epCapture ? EP_CAPTURE_MASK : 0 ) );
    }
    inline static Move* createKingsideCastlingMove( unsigned short kingIndex )
    {
        return new Move( kingIndex, kingIndex + 2, CASTLE_KINGSIDE );
    }
    inline static Move* createQueensideCastlingMove( unsigned short kingIndex )
    {
        return new Move( kingIndex, kingIndex - 2, CASTLE_QUEENSIDE );
    }
    inline static Move* createKnightPromotionMove( unsigned short from, unsigned short to, bool capture = false )
    {
        return new Move( from, to, PROMOTE_KNIGHT | (unsigned short) (capture ? CAPTURE_MASK : 0) );
    }
    inline static Move* createBishopPromotionMove( unsigned short from, unsigned short to, bool capture = false )
    {
        return new Move( from, to, PROMOTE_BISHOP | (unsigned short) ( capture ? CAPTURE_MASK : 0 ) );
    }
    inline static Move* createRookPromotionMove( unsigned short from, unsigned short to, bool capture = false )
    {
        return new Move( from, to, PROMOTE_ROOK | (unsigned short) ( capture ? CAPTURE_MASK : 0 ) );
    }
    inline static Move* createQueenPromotionMove( unsigned short from, unsigned short to, bool capture = false )
    {
        return new Move( from, to, PROMOTE_QUEEN | (unsigned short) ( capture ? CAPTURE_MASK : 0 ) );
    }

    inline static Move* getNullMove()
    {
        return &nullMove;
    }

    static Move* fromString( const std::string& moveString );

    std::string toString() const;

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
};

/// <summary>
/// Lightweight container for Moves, based on a perceived maximum number of possible moves in a single position and
/// using a pre-allocated array
/// Future enhancements may use  Move pointer or unique_ptr instead of the object itself
/// </summary>
/// <typeparam name="T"></typeparam>
//template<typename T> 
class MoveArray
{
private:
    inline static const size_t CAPACITY = 256;

    Move* moves[ CAPACITY ];
    size_t moveCount;

public:
    MoveArray() :
        moveCount( 0 ),
        moves {}
    {

    }

    ~MoveArray()
    {
        for ( size_t loop = 0; loop < moveCount; loop++ )
        {
            delete moves[ loop ];
        }
    }

    size_t count()
    {
        return moveCount;
    }

    void add( Move* move )
    {
        assert( moveCount < CAPACITY );
        moves[ moveCount++ ] = move;
    }

    void remove( size_t index )
    {
        assert( index < moveCount );
        delete moves[ index ];

        // Don't move all of the elements down the list, just fill in the gap with the one from the end
        // That means that any removals will likely change the list ordering, which is fine if we don't 
        // care or we know we won't be removing things once we are happy with the list and so we can then
        // correctly order it with impugnity - basically, this is a bespoke collection, not a general one
        if ( index < moveCount - 1 )
        {
            moves[ index ] = moves[ moveCount - 1 ];
        }

        moveCount--;
    }

    inline Move* operator[]( size_t index )
    {
        assert( index < moveCount );
        return moves[ index ];
    }

    inline bool empty()
    {
        return moveCount == 0;
    }

    inline Move** begin()
    {
        return std::begin( moves );
    }

    inline Move** end()
    {
        return &moves[ moveCount ];
    }
};