#pragma once

#include <sstream>
#include <string>

#include "Piece.h"

class Move
{
private:
    unsigned short from;
    unsigned short to;

    Piece::Type promotion;


public:
    static Move* fromString( std::string& moveString )
    {
        unsigned short fromFile = moveString[ 0 ] - 'a';
        unsigned short fromRank = moveString[ 1 ] - '1';
        unsigned short toFile = moveString[ 2 ] - 'a';
        unsigned short toRank = moveString[ 3 ] - '1';

        if ( moveString.length() > 4 )
        {
            return new Move( ( fromRank << 3 ) + ( fromFile & 7 ), ( toRank << 3 ) + ( toFile & 7 ), Piece::fromString( moveString[ 4 ] ) );
        }

        return new Move( ( fromRank << 3 ) + ( fromFile & 7 ), ( toRank << 3 ) + ( toFile & 7 ) );
    }

    static std::string toString( Move* move )
    {
        std::stringstream stream;
        stream << (char)( ( move->from & 7 ) + 'a' ) 
            << (char)( ( move->from >> 3 ) + '1' )
            << (char)( ( move->to & 7 ) + 'a' ) 
            << (char)( ( move->to >> 3 ) + '1' )
            << Piece::toString( move->promotion );

        return stream.str();
    }

    Move( unsigned short from, unsigned short to, Piece::Type promotion = Piece::Type::NONE ) : 
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

    inline short getFrom()
    {
        return from;
    }

    inline short getTo()
    {
        return to;
    }
};

