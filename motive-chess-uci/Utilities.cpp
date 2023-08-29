#include "Utilities.h"

#include "Log.h"
#include "Piece.h"

std::string Utilities::lowerSquareNames[ 64 ] = 
{ 
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
};

void Utilities::dumpBoard( std::array< unsigned char, 64>& pieces, std::string title )
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

void Utilities::dumpBitboard( unsigned long long pieces, std::string title )
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

void Utilities::dumpBitmask( unsigned long long bits, std::string title )
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
void Utilities::dump0x88( std::bitset<128> bits, std::string title )
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
