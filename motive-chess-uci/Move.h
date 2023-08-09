#pragma once

#include <string>

#include "Piece.h"

class Move
{
private:
    const unsigned short from;
    const unsigned short to;

    const unsigned char promotion;


public:
    static Move* fromString( std::string& moveString );
    static std::string toString( Move* move, bool lowercase = true );

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

    virtual ~Move()
    {

    }

    bool operator == ( const Move& move )
    {
        return ( move.from == from && move.to == to && move.promotion == promotion );
    }

    bool operator != ( const Move& move )
    {
        return !( move.from == from && move.to == to && move.promotion == promotion );
    }

    inline unsigned short getFrom()
    {
        return from;
    }

    inline unsigned short getTo()
    {
        return to;
    }

    inline bool isNullMove()
    {
        return from == 0 && to == 0;
    }
};

