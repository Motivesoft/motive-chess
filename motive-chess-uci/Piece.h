#pragma once

#include <string>

#include "Logger.h"

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
private:
    static const unsigned char COLOR_MASK         = 0b00011000;
    static const unsigned char INVERSE_COLOR_MASK = 0b11100111;
    static const unsigned char PIECE_MASK         = 0b00000111; // Mask
    static const unsigned char INVERSE_PIECE_MASK = 0b11111000; // Mask

    // Note that these are the non-colored (color unknown) versions
    static const unsigned char NOPIECE   = 0b00000000;
    static const unsigned char KING      = 0b00000110;
    static const unsigned char QUEEN     = 0b00000101;
    static const unsigned char ROOK      = 0b00000100;
    static const unsigned char BISHOP    = 0b00000011;
    static const unsigned char KNIGHT    = 0b00000010;
    static const unsigned char PAWN      = 0b00000001;

public:
    static const unsigned char WHITE      = 0b00001000;
    static const unsigned char BLACK      = 0b00010000;
    static const unsigned char NOCOLOR    = 0b00000000;

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
    
    static std::string toFENString( const unsigned char value )
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

    /// <summary>
    /// Return a lowercase letter representing the provided piece. Useful for 
    /// generating a UCI moves list
    /// </summary>
    /// <param name="value">the piece letter, colored or not</param>
    /// <returns>a single letter string for the piece, or empty string for no piece</returns>
    static std::string toMoveString( unsigned char value )
    {
        switch ( value & PIECE_MASK )
        {
            case KING:
                return "k";

            case QUEEN:
                return "q";

            case ROOK:
                return "r";

            case BISHOP:
                return "b";

            case KNIGHT:
                return "n";

            case PAWN:
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
                LOG_WARN << "Unexpected letter in FEN string: " << value;
                return NOTHING;
        }
    }

    /// <summary>
    /// Takes a 5 letter move string and returns the promotion piece
    /// </summary>
    /// <param name="value">a promotion move, such as a7a8q</param>
    /// <returns>The promotion piece, e.g. Piece::WQUEEN</returns>
    static unsigned char promotionPieceFromMoveString( std::string value )
    {
        LOG_TRACE << "Getting promotion piece from " << value;
        if ( value.length() < 5 )
        {
            return Piece::NOTHING;
        }

        unsigned char color = value[ 3 ] == '8' ? Piece::WHITE : Piece::BLACK;

        switch ( value[ 4 ] )
        {
            case 'k':
            case 'K':
                return color == Piece::WHITE ? Piece::WKING : Piece::BKING;

            case 'q':
            case 'Q':
                return color == Piece::WHITE ? Piece::WQUEEN : Piece::BQUEEN;

            case 'r':
            case 'R':
                return color == Piece::WHITE ? Piece::WROOK : Piece::BROOK;

            case 'b':
            case 'B':
                return color == Piece::WHITE ? Piece::WBISHOP : Piece::BBISHOP;

            case 'n':
            case 'N':
                return color == Piece::WHITE ? Piece::WKNIGHT : Piece::BKNIGHT;

            case 'p':
            case 'P':
                return color == Piece::WHITE ? Piece::WPAWN : Piece::BPAWN;
        }

        LOG_WARN << "Unexpected letter '" << value[ 4 ] << "' in move string : " << value;
        return Piece::NOTHING;
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

    /// <summary>
    /// Takes a non-colored piece, such as a promotion piece from a UCI 'moves' list, and apply a color to it
    /// </summary>
    /// <param name="piece">the piece</param>
    /// <param name="color">the color to apply</param>
    /// <returns>the colorised piece</returns>
    inline static unsigned char toColor( unsigned char piece, unsigned char color )
    {
        // This looks like we could condense to a single line, but the compiler insists
        // we have a logical/bitwise issue however we code it, so go with this version
        switch ( color & COLOR_MASK )
        {
            case WHITE:
                return ( piece & INVERSE_COLOR_MASK ) + WHITE;

            case BLACK:
                return ( piece & INVERSE_COLOR_MASK ) | BLACK;

            default:
                // Unexpected, and nothing obvious to do here, so just strip the color
                return ( piece & INVERSE_COLOR_MASK ); 
        }
    }
};
