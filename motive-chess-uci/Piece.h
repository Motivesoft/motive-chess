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
    static const unsigned char WHITE    = 0b00010000;
    static const unsigned char BLACK    = 0b00100000;
    static const unsigned char NOCOLOR  = 0b00000000;

    static const unsigned char NOPIECE = 0b00000000;
    static const unsigned char KING    = 0b00000110;
    static const unsigned char QUEEN   = 0b00000101;
    static const unsigned char ROOK    = 0b00000100;
    static const unsigned char BISHOP  = 0b00000011;
    static const unsigned char KNIGHT  = 0b00000010;
    static const unsigned char PAWN    = 0b00000001;

    static const unsigned char NOTHING = 0b00000000; // NOCOLOR | NOPIECE
    static const unsigned char WKING   = 0b00001110; // WHITE   | KING  
    static const unsigned char WQUEEN  = 0b00001101; // WHITE   | QUEEN 
    static const unsigned char WROOK   = 0b00001100; // WHITE   | ROOK  
    static const unsigned char WBISHOP = 0b00001011; // WHITE   | BISHOP
    static const unsigned char WKNIGHT = 0b00001010; // WHITE   | KNIGHT
    static const unsigned char WPAWN   = 0b00001001; // WHITE   | PAWN  
    static const unsigned char BKING   = 0b00010110; // BLACK   | KING  
    static const unsigned char BQUEEN  = 0b00010101; // BLACK   | QUEEN 
    static const unsigned char BROOK   = 0b00010100; // BLACK   | ROOK  
    static const unsigned char BBISHOP = 0b00010011; // BLACK   | BISHOP
    static const unsigned char BKNIGHT = 0b00010010; // BLACK   | KNIGHT
    static const unsigned char BPAWN   = 0b00010001; // BLACK   | PAWN  
    
    static std::string toFENStrin0b001g( const unsigned char value )
    {
        switch ( value )
        {
            case WKING:
                return "K";

            case WQUEEN:
                return "Q";

            case WROOK:
                return "R";

            case WBISHOP:
                return "B";

            case WKNIGHT:
                return "N";

            case WPAWN:
                return "P";

            case BKING:
                return "k";

            case BQUEEN:
                return "q";

            case BROOK:
                return "r";

            case BBISHOP:
                return "b";

            case BKNIGHT:
                return "n";

            case BPAWN:
                return "p";

            case NOTHING:
            default:
                return "";
        }
    }

    static std::string toMovesString( unsigned char value )
    {
        switch ( value )
        {
            case WKING:
            case BKING:
                return "k";

            case WQUEEN:
            case BQUEEN:
                return "q";

            case WROOK:
            case BROOK:
                return "r";

            case WBISHOP:
            case BBISHOP:
                return "b";

            case WKNIGHT:
            case BKNIGHT:
                return "n";

            case WPAWN:
            case BPAWN:
                return "p";

            default:
                return "";
        }
    }

    static unsigned char fromFENString( std::string value )
    {
        return fromFENString( value[ 0 ] );
    }

    static unsigned char fromFENString( char value )
    {
        switch ( value )
        {
            case 'K':
                return WKING;

            case 'Q':
                return WQUEEN;

            case 'R':
                return WROOK;

            case 'B':
                return WBISHOP;

            case 'N':
                return WKNIGHT;

            case 'P':
                return WPAWN;

            case 'k':
                return BKING;

            case 'q':
                return BQUEEN;

            case 'r':
                return BROOK;

            case 'b':
                return BBISHOP;

            case 'n':
                return BKNIGHT;

            case 'p':
                return BPAWN;

            default:
                return NOTHING;
        }
    }

    inline static bool isEmpty( unsigned char value )
    {
        return value == NOTHING;
    }

    inline static bool isWhite( unsigned char value )
    {
        return (value & WHITE) == WHITE;
    }

    inline static bool isBlack( unsigned char value )
    {
        return (value & BLACK) == BLACK;
    }

    inline static bool isKing( unsigned char value )
    {
        return (value & KING) == KING;
    }

    inline static bool isQueen( unsigned char value )
    {
        return (value & QUEEN) == QUEEN;
    }

    inline static bool isRook( unsigned char value )
    {
        return (value & ROOK) == ROOK;
    }

    inline static bool isBishop( unsigned char value )
    {
        return (value & BISHOP) == BISHOP;
    }

    inline static bool isKnight( unsigned char value )
    {
        return (value & KNIGHT) == KNIGHT;
    }

    inline static bool isPawn( unsigned char value )
    {
        return (value & PAWN) == PAWN;
    }

    /*
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
        return toString( piece,
                         piece == Piece::Type::NONE ? 
                             Piece::Color::NONE : 
                             lowercase ? Piece::Color::BLACK : Piece::Color::WHITE );
    }

    static std::string toString( Piece piece )
    {
        return toString( piece.type, piece.color );
    }

    static std::string toString( Piece::Type piece, Piece::Color color )
    {
        switch ( piece )
        {
            case Type::KING:
                return color == Piece::Color::WHITE ? "K" : "k";

            case Type::QUEEN:
                return color == Piece::Color::WHITE ? "Q" : "q";

            case Type::ROOK:
                return color == Piece::Color::WHITE ? "R" : "r";

            case Type::BISHOP:
                return color == Piece::Color::WHITE ? "B" : "b";

            case Type::KNIGHT:
                return color == Piece::Color::WHITE ? "N" : "n";

            case Type::PAWN:
                return color == Piece::Color::WHITE ? "P" : "p";

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

    static Piece fromFENString( char piece )
    {
        switch ( piece )
        {
            case 'K':
                return wk;

            case 'Q':
                return wq;

            case 'R':
                return wr;

            case 'B':
                return wb;

            case 'N':
                return wn;

            case 'P':
                return wp;

            case 'k':
                return bk;

            case 'q':
                return bq;

            case 'r':
                return br;

            case 'b':
                return bb;

            case 'n':
                return bn;

            case 'p':
                return bp;

            default:
                return nn;
        }
        //return Piece( fromString( piece ), 
        //              ( piece >= 'A' && piece <= 'Z' ) ? Piece::Color::WHITE : Piece::Color::BLACK );
    }

    static Piece fromFENString( std::string piece )
    {
        return fromFENString( piece[ 0 ] );
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
        switch ( piece & 0b1111 )
        {
            case 0b0001:
                return wp;

            case 0b0010:
                return wn;

            case 0b0011:
                return wb;

            case 0b0100:
                return wr;

            case 0b0101:
                return wq;

            case 0b0110:
                return wk;

            case 0b1001:
                return bp;

            case 0b1010:
                return bn;

            case 0b1011:
                return bb;

            case 0b1100:
                return br;

            case 0b1101:
                return bq;

            case 0b1110:
                return bk;

            default:
            case 0b0000:
                return nn;
        }
    }

    //static inline Piece xbyteToPiece( const unsigned char piece )
    //{
    //    Piece::Type type;
    //    Piece::Color color = ( piece & 8 ) == 8 ? Piece::Color::BLACK : Piece::Color::WHITE;

    //    switch ( piece & 7 )
    //    {
    //        case 0b001:
    //            type = Piece::Type::PAWN;
    //            break;

    //        case 0b010:
    //            type = Piece::Type::KNIGHT;
    //            break;

    //        case 0b011:
    //            type = Piece::Type::BISHOP;
    //            break;

    //        case 0b100:
    //            type = Piece::Type::ROOK;
    //            break;

    //        case 0b101:
    //            type = Piece::Type::QUEEN;
    //            break;

    //        case 0b110:
    //            type = Piece::Type::KING;
    //            break;

    //        default:
    //        case 0b000:
    //            type = Piece::Type::NONE;
    //            break;
    //    }

    //    return Piece( type, color );
    //}

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
        color( type == Piece::Type::NONE ? Piece::Color::NONE : color )
    {

    }

    Piece( Piece& piece ) :
        type( piece.type ),
        color( piece.color )
    {

    }

    Piece( const Piece& piece ) :
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

    std::string toString()
    {
        return Piece::toString( *this );
    }

    // Constant declarations
    static const Piece wk;
    static const Piece wq;
    static const Piece wr;
    static const Piece wb;
    static const Piece wn;
    static const Piece wp;
    static const Piece bk;
    static const Piece bq;
    static const Piece br;
    static const Piece bb;
    static const Piece bn;
    static const Piece bp;
    static const Piece nn;
    */
};
