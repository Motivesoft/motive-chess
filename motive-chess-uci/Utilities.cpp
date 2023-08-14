#include "Utilities.h"

#include "Logger.h"
#include "Piece.h"

void Utilities::dumpBoard( std::array< unsigned char, 64>& pieces )
{
    LOG_DEBUG << "Board:";
    LOG_DEBUG << "  ABCDEFGH";

    for ( unsigned short rank = 0, rankIndex = 56; rank < 8; rank++, rankIndex -= 8 )
    {
        std::stringstream stream;
        for ( unsigned short index = rankIndex; index < rankIndex + 8; index++ )
        {
            stream << ( Piece::isEmpty( pieces[ index ] ) ?
                        ( ( index & 1 ) == 0 ? "." : " " ) :
                        Piece::toFENString( pieces[ index ] ) );
        }

        LOG_DEBUG << 1 + rankIndex / 8 << " " << stream.str() << " " << 1 + rankIndex / 8;
    }

    LOG_DEBUG << "  ABCDEFGH";
}

void Utilities::dumpBitboard( unsigned long long pieces )
{
    LOG_DEBUG << "Board:";
    LOG_DEBUG << "  ABCDEFGH";

    unsigned long long mask = 1;
    for ( unsigned short rank = 0, rankIndex = 56; rank < 8; rank++, rankIndex -= 8 )
    {
        std::stringstream stream;
        for ( unsigned short index = rankIndex; index < rankIndex + 8; index++ )
        {
            stream << ( ( pieces & (mask << index) ) ? "." : " " );
        }

        LOG_DEBUG << 1 + rankIndex / 8 << " " << stream.str() << " " << 1 + rankIndex / 8;
    }

    LOG_DEBUG << "  ABCDEFGH";
}

void Utilities::dumpBitmask( unsigned long long bits )
{
    std::stringstream stream;

    unsigned long long mask = 1;
    for ( int index = 63; index >= 0; index-- )
    {
        stream << (( bits & (mask << index) ) ? "1" : "0");
    }

    LOG_DEBUG << stream.str() << " : " << bits;
}

/// <summary>
/// Dump an 0x88 board, which is effectively two 8x8 boards side by side
/// </summary>
/// <param name="bits">the board</param>
void Utilities::dump0x88( std::bitset<128> bits )
{
    LOG_DEBUG << "  01234567 01234567";
    LOG_DEBUG << "  -------- --------";
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
        LOG_DEBUG << stream.str() << "|" << line;
    }
    LOG_DEBUG << "  -------- --------";
    LOG_DEBUG << "  01234567 89ABCDEF";
}
