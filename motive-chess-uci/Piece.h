#pragma once

#include <string>

class Piece
{
public:
    enum class Color
    {
        WHITE,
        BLACK,
        NONE
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

private:
    Type type;
    Color color;

public:
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

    Piece() :
        type( Piece::Type::NONE ),
        color( Piece::Color::NONE )
    {

    }

    Piece( Piece& piece ) :
        type( piece.type ),
        color( piece.color )
    {

    }

    virtual ~Piece()
    {

    }

    bool operator == ( const Piece& piece )
    {
        return type == piece.type && color == piece.color;
    }

    bool operator != ( const Piece& piece )
    {
        return type != piece.type || color != piece.color;
    }
};

