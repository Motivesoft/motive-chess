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

    static std::string toString( Piece::Type piece )
    {
        switch ( piece )
        {
            case Type::KING:
                return "k";

            case Type::QUEEN:
                return "q";

            case Type::ROOK:
                return "r";

            case Type::BISHOP:
                return "b";

            case Type::KNIGHT:
                return "n";

            case Type::PAWN:
                return "p";

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
                return Type::KING;
            case 'q':
                return Type::QUEEN;
            case 'r':
                return Type::ROOK;
            case 'b':
                return Type::BISHOP;
            case 'n':
                return Type::KNIGHT;
            case 'p':
                return Type::PAWN;
            default:
                return Type::NONE;
        }
    }
};

