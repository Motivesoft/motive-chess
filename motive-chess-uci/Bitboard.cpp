#include "Bitboard.h"

#include "Utilities.h"

Bitboard Bitboard::instance = Bitboard();

unsigned long long Bitboard::pawnMoves[ 64 ];
unsigned long long Bitboard::pawnCaptures[ 64 ];
unsigned long long Bitboard::pawnDoubleMoves[ 64 ];
unsigned long long Bitboard::knightMoves[ 64 ];

void Bitboard::buildBitboards()
{
    unsigned long long mask = 1;

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

}
