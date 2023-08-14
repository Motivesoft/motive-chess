#include "Bitboard.h"

#include <bitset>

#include "Logger.h"
#include "Utilities.h"

std::unique_ptr<Bitboard> Bitboard::instance {};

unsigned long long Bitboard::pawnMoves[ 64 ];
unsigned long long Bitboard::pawnCaptures[ 64 ];
unsigned long long Bitboard::pawnDoubleMoves[ 64 ];
unsigned long long Bitboard::knightMoves[ 64 ];

unsigned long long bitboardFrom0x88( std::bitset<128>& bits )
{
    unsigned long long result = 0;

    // For each line of the 0x88 board
    unsigned long long mask = 1;
    for ( unsigned short rank = 0; rank < 8; rank++ )
    {
        // Next set of bits (rank on the bitboard)
        for ( unsigned short file = 0; file < 8; file++ )
        {
            if ( bits.test( (rank << 4) + file ) )
            {
                result |= mask;
            }

            mask <<= 1;
        }
    }

    return result;
}

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
    std::bitset<128> x88;
    std::bitset<128> knightSquares;

    // Represent all knight moves from a central location and then encode that into a 0x88 space
    // 01010
    // 10001
    // 00*00
    // 10001
    // 01010
    knightSquares |= 0b00001010;
    knightSquares <<= 16;

    knightSquares |= 0b00010001;
    knightSquares <<= 16;

    knightSquares |= 0b00000000; // The central location being here 0b00000*00
    knightSquares <<= 16;

    knightSquares |= 0b00010001;
    knightSquares <<= 16;

    knightSquares |= 0b00001010;

    // In this layout, centre square is 2,2 and the '1's in the mask are move destinations from there
    for ( unsigned short index = 0; index < 64; index++ )
    {
        // Need to move -2,-2 to get mask to 0,0
        short fileOffset = -2;
        short rankOffset = -2;

        // And then this amount to move to 'index' on a normal board
        rankOffset += ( index >> 3 );
        fileOffset += ( index & 7 );

        // Convrted to a movement into the 128-bit structure 
        int offset = ( rankOffset << 4 ) + fileOffset;

        // Shift left or right, accordingly
        x88 = offset < 0 ? knightSquares >> -offset : knightSquares << offset;

        knightMoves[ index ] = bitboardFrom0x88( x88 );
        LOG_DEBUG << Utilities::indexToSquare( index );
        Utilities::dumpBitboard( knightMoves[ index ] );
    }

    // Bishops
    // Rooks
    // Queens
    // King

    LOG_DEBUG << "Done creating bitboards";
}