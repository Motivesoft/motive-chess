#include "Utilities.h"

#include "Logger.h"
#include "Piece.h"

void Utilities::dumpBoard( std::array< unsigned char, 64>& pieces, std::string title, const std::source_location location )
{
    if ( Logger::isExcluded( Logger::Level::DEBUG ) )
    {
        return;
    }

    LOG_DEBUG_LOC << "  +---+---+---+---+---+---+---+---+ " << title;
    
    bool squareIsDark;
    for ( unsigned short rank = 0, rankIndex = 56; rank < 8; rank++, rankIndex -= 8 )
    {
        squareIsDark = (rank & 1);
        std::stringstream stream;

        for ( unsigned short index = rankIndex; index < rankIndex + 8; index++, squareIsDark = !squareIsDark )
        {
            if ( Piece::isEmpty( pieces[ index ] ) )
            {
                stream << "|" << ( squareIsDark ? ":::" : "   " );
            }
            else
            {
                if ( Piece::isWhite( pieces[ index ] ) )
                {
                    stream << "| " << Piece::toFENString( pieces[ index ] ) << " ";
                }
                else
                {
                    stream << "|*" << Piece::toFENString( pieces[ index ] ) << "*";
                }
            }
        }

        LOG_DEBUG_LOC << 1 + rankIndex / 8 << " " << stream.str() << "|";

        LOG_DEBUG_LOC << "  +---+---+---+---+---+---+---+---+";
    }

    LOG_DEBUG_LOC << "    a   b   c   d   e   f   g   g    ";
}

void Utilities::dumpBitboard( unsigned long long pieces, std::string title, const std::source_location location )
{
    if ( Logger::isExcluded( Logger::Level::DEBUG ) )
    {
        return;
    }

    LOG_DEBUG_LOC << "  ABCDEFGH    " << title;
    LOG_DEBUG_LOC << "  --------";

    unsigned long long mask = 1;
    for ( unsigned short rank = 0, rankIndex = 56; rank < 8; rank++, rankIndex -= 8 )
    {
        std::stringstream stream;
        for ( unsigned short index = rankIndex; index < rankIndex + 8; index++ )
        {
            stream << ( ( pieces & (mask << index) ) ? "." : " " );
        }

        LOG_DEBUG_LOC << 1 + rankIndex / 8 << "|" << stream.str() << "|" << 1 + rankIndex / 8;
    }

    LOG_DEBUG_LOC << "  --------";
    LOG_DEBUG_LOC << "  ABCDEFGH";
}

void Utilities::dumpBitmask( unsigned long long bits, std::string title, const std::source_location location )
{
    if ( Logger::isExcluded( Logger::Level::DEBUG ) )
    {
        return;
    }

    std::stringstream stream;

    unsigned long long mask = 1;
    for ( int index = 63; index >= 0; index-- )
    {
        stream << (( bits & (mask << index) ) ? "1" : "0");
    }

    LOG_DEBUG_LOC << stream.str() << "    " << title;
}

/// <summary>
/// Dump an 0x88 board, which is effectively two 8x8 boards side by side
/// </summary>
/// <param name="bits">the board</param>
void Utilities::dump0x88( std::bitset<128> bits, std::string title, const std::source_location location )
{
    if ( Logger::isExcluded( Logger::Level::DEBUG ) )
    {
        return;
    }

    LOG_DEBUG_LOC << "  01234567 01234567    " << title;
    LOG_DEBUG_LOC << "  -------- --------";
    for ( unsigned short rank = 8; rank > 0; rank-- )
    {
        unsigned short line = rank - 1;

        std::stringstream stream;

        stream << line << "|";
        for ( unsigned short file = 0; file < 16; file++ )
        {
            unsigned short column = file;

            stream << ( bits[ (line<<4) + column ] ? "*" : " " );
            if ( file == 8 )
            {
                stream << "|";
            }
        }
        LOG_DEBUG_LOC << stream.str() << "|" << line;
    }
    LOG_DEBUG_LOC << "  -------- --------";
    LOG_DEBUG_LOC << "  01234567 89ABCDEF";
}
