#include "Utilities.h"

#include "Log.h"
#include "Piece.h"

void Utilities::dumpBoard( std::array< unsigned char, 64>& pieces, std::string title, const std::source_location location )
{
    Log::Debug( [&] ( const Log::Logger& logger )
    {
        logger << "  +---+---+---+---+---+---+---+---+ " << title << std::endl;

        bool squareIsDark;
        for ( unsigned short rank = 0, rankIndex = 56; rank < 8; rank++, rankIndex -= 8 )
        {
            squareIsDark = ( rank & 1 );
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

            logger << 1 + rankIndex / 8 << " " << stream.str() << "|" << std::endl;

            logger << "  +---+---+---+---+---+---+---+---+" << std::endl;
        }

        logger << "    a   b   c   d   e   f   g   h    " << std::endl;
    } );
}

void Utilities::dumpBitboard( unsigned long long pieces, std::string title, const std::source_location location )
{
    Log::Debug( [&] ( const Log::Logger& logger )
    {
        logger << "  ABCDEFGH    " << title << std::endl;
        logger << "  --------" << std::endl;

        unsigned long long mask = 1;
        for ( unsigned short rank = 0, rankIndex = 56; rank < 8; rank++, rankIndex -= 8 )
        {
            std::stringstream stream;
            for ( unsigned short index = rankIndex; index < rankIndex + 8; index++ )
            {
                stream << ( ( pieces & (mask << index) ) ? "." : " " );
            }

            logger << 1 + rankIndex / 8 << "|" << stream.str() << "|" << 1 + rankIndex / 8 << std::endl;
        }

        logger << "  --------" << std::endl;
        logger << "  ABCDEFGH" << std::endl;
    } );
}

void Utilities::dumpBitmask( unsigned long long bits, std::string title, const std::source_location location )
{
    Log::Debug( [&] ( const Log::Logger& logger )
    {
        std::stringstream stream;

        unsigned long long mask = 1;
        for ( int index = 63; index >= 0; index-- )
        {
            stream << (( bits & (mask << index) ) ? "1" : "0");
        }

        logger << stream.str() << "    " << title << std::endl;
    } );
}

/// <summary>
/// Dump an 0x88 board, which is effectively two 8x8 boards side by side
/// </summary>
/// <param name="bits">the board</param>
void Utilities::dump0x88( std::bitset<128> bits, std::string title, const std::source_location location )
{
    Log::Debug( [&] ( const Log::Logger& logger )
    {
        logger << "  01234567 89ABCDEF    " << title << std::endl;
        logger << "  -------- --------" << std::endl;
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
            logger << stream.str() << "|" << line << std::endl;
        }
        logger << "  -------- --------" << std::endl;
        logger << "  01234567 89ABCDEF" << std::endl;
    } );
}
