#pragma once

#include <array>
#include <string>

#include "Logger.h"
#include "Piece.h"
#include "Utilities.h"

class Fen
{
private:
    std::array< unsigned char, 64 > board;
    unsigned char activeColor;
    bool castlingWK;
    bool castlingWQ;
    bool castlingBK;
    bool castlingBQ;
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

    virtual ~Fen()
    {

    }
};

