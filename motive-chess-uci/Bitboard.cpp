#include "Bitboard.h"

#include <bitset>

#include "Logger.h"
#include "Utilities.h"

std::unique_ptr<Bitboard> Bitboard::instance {};

unsigned long long Bitboard::pawnMoves[ 64 ];
unsigned long long Bitboard::pawnCaptures[ 64 ];
unsigned long long Bitboard::pawnDoubleMoves[ 64 ];
unsigned long long Bitboard::knightMoves[ 64 ];

void Bitboard::buildBitboards()
{
    unsigned long long mask = 1;

    LOG_DEBUG << "Creating movement bitboards";

    // Pawns

    for ( int loop = 8; loop < 56; loop++ )
    {
        pawnMoves[ loop ] = mask << ( loop + 8 );
    }

    for ( int loop = 8; loop < 16; loop++ )
    {
        pawnDoubleMoves[ loop ] = mask << ( loop + 16 );
    }

    for ( int loop = 8; loop < 56; loop++ )
    {
        // Edge detection
        if ( Utilities::indexToFile( loop ) > 0 )
        {
            pawnCaptures[ loop ] = mask << ( loop + 7 );
        }
        if ( Utilities::indexToFile( loop ) < 7 )
        {
            pawnCaptures[ loop ] = mask << ( loop + 9 );
        }
    }

    // Knights

    for ( int loop = 8; loop < 56; loop++ )
    {
        // TODO for now...
        knightMoves[ loop ] = 0;
    }

    std::bitset<128> x88;
    std::bitset<128> knightSquares { 1 };
    knightSquares |= 0b01010000;
    knightSquares <<= 16;

    knightSquares |= 0b10001000;
    knightSquares <<= 16;

    knightSquares |= 0b00000000;
    knightSquares <<= 16;

    knightSquares |= 0b10001000;
    knightSquares <<= 16;

    knightSquares |= 0b01010000;

        /*
        ( 0b01010000 << 8 ) |
        ( 0b10001000 << 8 ) << 16 |
        ( 0b00000000 << 8 ) << 32 | // the piece position is in the middle of the pattern
        ( 0b10001000 << 8 ) << 48 |
        ( 0b01010000 << 8 ) << 64;
        */
    LOG_DEBUG << "Empty";
    Utilities::dump0x88( x88 );
    LOG_DEBUG << "and knight squares";
    Utilities::dump0x88( x88 | knightSquares );
    LOG_DEBUG << "only knight squares";
    Utilities::dump0x88( knightSquares );
    LOG_DEBUG << "only knight squares shifted";
    Utilities::dump0x88( knightSquares << 8 );
    LOG_DEBUG << "Shifted to a1";
    Utilities::dump0x88( knightSquares >> 35 ); // Where 35 is 2*16 for the rows and 3 for the square

    LOG_DEBUG << "Done creating bitboards";
}
