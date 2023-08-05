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
        unsigned short fromFile = moveString[ 0 ] - 'a' + 1;
        unsigned short fromRank = moveString[ 1 ] - '0';
        unsigned short toFile = moveString[ 2 ] - 'a' + 1;
        unsigned short toRank = moveString[ 3 ] - '0';

        if ( moveString.length() > 4 )
        {
            return new Move( ( fromFile << 3 ) + fromRank, ( toFile << 3 ) + toRank, Piece::fromString( moveString[ 4 ] ) );
        }

        return new Move( ( fromFile << 3 ) + fromRank, ( toFile << 3 ) + toRank );
    }

    static std::string toString( Move* move )
    {
        std::stringstream stream;
        stream << (char)( ( move->from >> 3 ) + 'a') << ( move->from & 7 ) << (char)( ( move->to >> 3 ) + 'a') << ( move->to & 7 ) << Piece::toString( move->promotion );

        return stream.str();
    }

    Move( unsigned short from, unsigned short to, Piece::Type promtion = Piece::Type::NONE ) : 
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
};

