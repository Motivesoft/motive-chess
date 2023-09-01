#include "Piece.h"

#include "Log.h"

std::string Piece::toFENString( const unsigned char value )
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

std::string Piece::toMoveString( unsigned char value )
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

unsigned char Piece::fromFENString( std::string value )
{
    return fromFENString( value[ 0 ] );
}

unsigned char Piece::fromFENString( char value )
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
            Log::Warn << "Unexpected letter in FEN string: " << value << std::endl;
            return NOTHING;
    }
}

unsigned char Piece::promotionPieceFromMoveString( std::string value )
{
    Log::Trace << "Getting promotion piece from " << value << std::endl;

    if ( value.length() < 5 )
    {
        return Piece::NOTHING;
    }

    // Work out who is promoting based on the destination rank
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

    Log::Warn << "Unexpected letter '" << value[ 4 ] << "' in move string : " << value << std::endl;
    return Piece::NOTHING;
}

std::string Piece::toColorString( unsigned char color )
{
    if ( isWhite( color ) )
    {
        return "white";
    }
    else if ( isBlack( color ) )
    {
        return "black";
    }

    Log::Warn << "Failed to get color name for " << color << std::endl;
    return "";
}

std::string Piece::toColorLetter( unsigned char color )
{
    if ( isWhite( color ) )
    {
        return "w";
    }
    else if ( isBlack( color ) )
    {
        return "b";
    }

    Log::Warn << "Failed to get color name for " << color << std::endl;
    return "";
}

unsigned char Piece::colorFrom( std::string color )
{
    return colorFrom( color[ 0 ] );
}

unsigned char Piece::colorFrom( unsigned char color )
{
    switch ( color )
    {
        case 'w':
        case 'W':
            return Piece::WHITE;

        case 'b':
        case 'B':
            return Piece::BLACK;
    }

    Log::Warn << "Failed to get color from " << color << std::endl;
    return Piece::NOCOLOR;
}
