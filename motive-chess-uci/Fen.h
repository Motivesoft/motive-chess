#pragma once

#include <array>
#include <string>

#include "CastlingRights.h"
#include "Log.h"
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

    Fen( const std::string position );

    void skipSpace( std::string::iterator& it, std::string::iterator& end );

    std::string nextWord( std::string::iterator& it, std::string::iterator& end );

private:
    Fen( const std::array<unsigned char,64>& pieces,
         const unsigned char activeColor,
         const CastlingRights& castlingRights,
         const unsigned short enPassantIndex,
         const unsigned short halfmoveClock,
         const unsigned short fullmoveNumber ) :
        pieces( pieces ),
        activeColor( activeColor ),
        castlingRights( castlingRights ),
        enPassantIndex( enPassantIndex ),
        halfmoveClock( halfmoveClock ),
        fullmoveNumber( fullmoveNumber )
    {

    }

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

    std::string toString();

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

