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
/// bit 3-4 represent color
/// bit 4-7 unused/available for other properties (e.g. has moved, can be en-passanted, ...)
/// </summary>
class Piece
{
private:
    // Masks
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

    static const unsigned char WHITE      = 0b00001000;
    static const unsigned char BLACK      = 0b00010000;
    static const unsigned char NOCOLOR    = 0b00000000;

    static const unsigned char NOTHING = 0b00000000; // NOCOLOR | NOPIECE

public:
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
    
    static std::string toFENString( const unsigned char value );

    /// <summary>
    /// Return a lowercase letter representing the provided piece. Useful for 
    /// generating a UCI moves list
    /// </summary>
    /// <param name="value">the piece letter, colored or not</param>
    /// <returns>a single letter string for the piece, or empty string for no piece</returns>
    static std::string toMoveString( unsigned char value );

    static unsigned char fromFENString( std::string value );

    static unsigned char fromFENString( char value );

    /// <summary>
    /// Takes a 5 letter move string and returns the promotion piece
    /// </summary>
    /// <param name="value">a promotion move, such as a7a8q</param>
    /// <returns>The promotion piece, e.g. Piece::WQUEEN</returns>
    static unsigned char promotionPieceFromMoveString( std::string value );

    inline static bool isEmpty( unsigned char value )
    {
        return value == NOTHING;
    }

    inline static bool isWhite( unsigned char value )
    {
        return (value & COLOR_MASK) == WHITE;
    }

    inline static bool isBlack( unsigned char value )
    {
        return (value & COLOR_MASK) == BLACK;
    }

    inline static bool isColor( unsigned char value, unsigned char color )
    {
        return ( value & COLOR_MASK ) == color;
    }

    inline static bool isKing( unsigned char value )
    {
        return (value & PIECE_MASK) == KING;
    }

    inline static bool isQueen( unsigned char value )
    {
        return (value & PIECE_MASK) == QUEEN;
    }

    inline static bool isRook( unsigned char value )
    {
        return (value & PIECE_MASK) == ROOK;
    }

    inline static bool isBishop( unsigned char value )
    {
        return (value & PIECE_MASK) == BISHOP;
    }

    inline static bool isKnight( unsigned char value )
    {
        return (value & PIECE_MASK) == KNIGHT;
    }

    inline static bool isPawn( unsigned char value )
    {
        return (value & PIECE_MASK) == PAWN;
    }

    inline static unsigned char emptyPiece()
    {
        return Piece::NOTHING;
    }

    inline static unsigned char oppositeColor( unsigned char color )
    {
        return color ^ COLOR_MASK;
    }

    inline static unsigned char getStartingColor()
    {
        return Piece::WHITE;
    }

    static std::string toColorString( unsigned char color );

    static std::string toColorLetter( unsigned char color );

    static unsigned char colorFrom( std::string color );

    static unsigned char colorFrom( unsigned char color );
};
