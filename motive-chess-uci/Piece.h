#pragma once

#include <string>

/// <summary>
/// Bit mask for piece to byte:
/// 76543210
/// bit 0-2 represent piece types
///   000 = Nothing
///   001 = Pawn
///   010 = Knight
///   011 = Bishop
///   100 = Rook
///   101 = Queen
///   110 = King
/// bit 3 represents color (0=white)
/// bit 4-7 available for other properties (e.g. has moved, can be en-passanted, ...)
/// </summary>
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
    const Type type;
    const Color color;

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

    /// <summary>
    /// Converts a piece into a byte representation
    /// </summary>
    /// <param name="piece">a piece</param>
    /// <returns>a byte value representing a piece</returns>
    static inline unsigned char pieceToByte( const Piece& piece )
    {
        unsigned char value = piece.color == Piece::Color::WHITE ? 0 : 1 << 3;

        switch ( piece.type )
        {
            case Piece::Type::PAWN:
                value |= 0b001;
                break;

            case Piece::Type::KNIGHT:
                value |= 0b010;
                break;

            case Piece::Type::BISHOP:
                value |= 0b011;
                break;

            case Piece::Type::ROOK:
                value |= 0b100;
                break;

            case Piece::Type::QUEEN:
                value |= 0b101;
                break;

            case Piece::Type::KING:
                value |= 0b110;
                break;

            default:
            case Piece::Type::NONE:
                // Do nothing
                value |= 0b000;
                break;
        }

        return value;
    }

    static inline Piece byteToPiece( const unsigned char piece )
    {
        Piece::Type type;
        Piece::Color color = ( piece & 8 ) == 8 ? Piece::Color::BLACK : Piece::Color::WHITE;

        switch ( piece & 7 )
        {
            case 0b001:
                type = Piece::Type::PAWN;
                break;

            case 0b010:
                type = Piece::Type::KNIGHT;
                break;

            case 0b011:
                type = Piece::Type::BISHOP;
                break;

            case 0b100:
                type = Piece::Type::ROOK;
                break;

            case 0b101:
                type = Piece::Type::QUEEN;
                break;

            case 0b110:
                type = Piece::Type::KING;
                break;

            default:
            case 0b000:
                type = Piece::Type::NONE;
                break;
        }

        return Piece( type, color );
    }

    static inline unsigned char colorBit( const unsigned char& piece )
    {
        return piece & 8;
    }

    static inline unsigned char typeBits( const unsigned char& piece )
    {
        return piece & 7;
    }

    static inline bool isNothing( const Piece& piece )
    {
        return piece.type == Piece::Type::NONE;
    }

    static inline bool isNothing( const unsigned char& piece )
    {
        return typeBits( piece ) == 0b000;
    }

    static inline bool isWhite( const Piece& piece )
    {
        // Piece has white color and is a piece type
        return piece.color == Piece::Color::WHITE && !isNothing( piece );
    }

    static inline bool isWhite( const unsigned char& piece )
    {
        // Piece has white bit set and is a piece type
        return !colorBit( piece ) && !isNothing( piece );
    }

    static inline bool isBlack( const Piece& piece )
    {
        // Piece has black color and is a piece type
        return piece.color == Piece::Color::BLACK && !isNothing( piece );
    }

    static inline bool isBlack( const unsigned char& piece )
    {
        // Piece has black bit set and is a piece type
        return colorBit( piece ) && !isNothing( piece );
    }

    Piece() :
        type( Piece::Type::NONE ),
        color( Piece::Color::NONE )
    {

    }

    Piece( Piece::Type type, Piece::Color color ) :
        type( type ),
        color( color )
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

