#pragma once

#include <string>

#include "Piece.h"

class Move
{
private:
    const unsigned short from;
    const unsigned short to;

    // This may be colorless
    const unsigned char promotion;

public:
    static Move fromString( std::string& moveString );
    static std::string toString( const Move& move, bool lowercase = true );

    Move( unsigned short from, unsigned short to, unsigned char promotion = Piece::NOTHING ) : 
        from( from ),
        to( to ),
        promotion( promotion )
    {

    }

    Move( Move& move ) :
        from( move.from ),
        to( move.to ),
        promotion( move.promotion )
    {
    }

    Move( const Move& move ) :
        from( move.from ),
        to( move.to ),
        promotion( move.promotion )
    {
    }

    virtual ~Move()
    {

    }

    bool operator == ( const Move& move ) const
    {
        return ( move.from == from && move.to == to && move.promotion == promotion );
    }

    bool operator != ( const Move& move ) const
    {
        return !( move.from == from && move.to == to && move.promotion == promotion );
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
        return promotion;
    }

    inline bool isNullMove() const
    {
        return from == 0 && to == 0;
    }
};

