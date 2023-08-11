#pragma once

#include <array>
#include <string>

#include "CastlingRights.h"
#include "Logger.h"
#include "Piece.h"
#include "Utilities.h"

class Fen
{
private:
    std::array< unsigned char, 64 > pieces;
    unsigned char activeColor;
    CastlingRights castlingRights;
    unsigned short enPassantIndex;
    unsigned short halfmoveClock;
    unsigned short fullmoveNumber;

    Fen( std::string position );

    void skipSpace( std::string::iterator& it, std::string::iterator& end );

    std::string nextWord( std::string::iterator& it, std::string::iterator& end );

public:
    inline static const std::string startingPosition = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    static Fen fromStartingPosition()
    {
        return fromPosition( startingPosition );
    }

    static Fen fromPosition( std::string position )
    {
        return Fen( position );
    }

    Fen( Fen& fen ) :
        pieces( fen.pieces ),
        activeColor( fen.activeColor ),
        castlingRights( fen.castlingRights ),
        enPassantIndex( fen.enPassantIndex ),
        halfmoveClock( fen.halfmoveClock ),
        fullmoveNumber( fen.fullmoveNumber )
    {

    }

    virtual ~Fen()
    {

    }

    friend class Board;
};

