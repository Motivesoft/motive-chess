#pragma once

#include <string>

class Piece
{
public:
    enum class Color
    {
        WHITE,
        BLACK
    };

    enum class Type
    {
        KING,
        QUEEN,
        ROOK,
        BISHOP,
        KNIGHT,
        PAWN,
        NONE
    };

    static std::string toString( Piece::Type piece, bool lowercase = true )
    {
        switch ( piece )
        {
            case Type::KING:
                return lowercase ? "k" : "K";

            case Type::QUEEN:
                return lowercase ? "q" : "Q";

            case Type::ROOK:
                return lowercase ? "r" : "R";

            case Type::BISHOP:
                return lowercase ? "b" : "B";

            case Type::KNIGHT:
                return lowercase ? "n" : "N";

            case Type::PAWN:
                return lowercase ? "p" : "P";

            default:
                return "";
        }
    }

    static Piece::Type fromString( std::string piece )
    {
        return fromString( piece[ 0 ] );
    }

    static Piece::Type fromString( char piece )
    {
        switch ( piece )
        {
            case 'k':
            case 'K':
                return Type::KING;
            
            case 'q':
            case 'Q':
                return Type::QUEEN;
            
            case 'r':
            case 'R':
                return Type::ROOK;
            
            case 'b':
            case 'B':
                return Type::BISHOP;
            
            case 'n':
            case 'N':
                return Type::KNIGHT;
            
            case 'p':
            case 'P':
                return Type::PAWN;
            
            default:
                return Type::NONE;
        }
    }
};

