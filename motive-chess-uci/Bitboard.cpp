#include "Bitboard.h"

#include "Logger.h"
#include "Utilities.h"

std::unique_ptr<Bitboard> Bitboard::instance {};

unsigned long long Bitboard::whitePawnMoves[ 64 ];
unsigned long long Bitboard::whitePawnCaptures[ 64 ];
unsigned long long Bitboard::blackPawnMoves[ 64 ];
unsigned long long Bitboard::blackPawnCaptures[ 64 ];
unsigned long long Bitboard::knightMoves[ 64 ];
unsigned long long Bitboard::bishopMoves[ 64 ];
unsigned long long Bitboard::rookMoves[ 64 ];
unsigned long long Bitboard::queenMoves[ 64 ];
unsigned long long Bitboard::kingMoves[ 64 ];

void Bitboard::buildBitboards()
{
    std::bitset<128> x88;
    std::bitset<128> pieceSquares;

    unsigned long long mask = 1;
    unsigned short path;

    LOG_TRACE << "Creating movement bitboards";

    // Initialise
    for ( int loop = 0; loop < 64; loop++ )
    {
        whitePawnMoves[ loop ] = 0;
        whitePawnCaptures[ loop ] = 0;
        blackPawnMoves[ loop ] = 0;
        blackPawnCaptures[ loop ] = 0;
        knightMoves[ loop ] = 0;
        bishopMoves[ loop ] = 0;
        rookMoves[ loop ] = 0;
        queenMoves[ loop ] = 0;
        kingMoves[ loop ] = 0;
    }

    // Pawns
    // Single square advance
    for ( int loop = 8; loop < 56; loop++ )
    {
        whitePawnMoves[ loop ] |= mask << ( loop + 8 );
    }

    // Add double-square advance to elligible squares
    for ( int loop = 8; loop < 16; loop++ )
    {
        whitePawnMoves[ loop ] |= mask << ( loop + 16 );
    }

    for ( int loop = 55; loop > 7; loop-- )
    {
        blackPawnMoves[ loop ] |= mask << ( loop - 8 );
    }

    // Add double-square advance to elligible squares
    for ( int loop = 55; loop > 47; loop-- )
    {
        blackPawnMoves[ loop ] |= mask << ( loop - 16 );
    }

    // Captures (be wary of the edge of the board)
    // This should start at 8 because pawns will never be on the first rank
    // but we use this for possible attack detection to, so we need it to go all the way
    for ( int loop = 0; loop < 56; loop++ )
    {
        // Set, but with edge detection
        if ( Utilities::indexToFile( loop ) > 0 )
        {
            whitePawnCaptures[ loop ] |= mask << ( loop + 7 );
        }
        if ( Utilities::indexToFile( loop ) < 7 )
        {
            whitePawnCaptures[ loop ] |= mask << ( loop + 9 );
        }
    }

    // Captures (be wary of the edge of the board)
    // This should start at 55 because pawns will never be on the first rank
    // but we use this for possible attack detection to, so we need it to go all the way
    for ( int loop = 63; loop > 7; loop-- )
    {
        // Set, but with edge detection
        if ( Utilities::indexToFile( loop ) > 0 )
        {
            blackPawnCaptures[ loop ] |= mask << ( loop - 9 );
        }
        if ( Utilities::indexToFile( loop ) < 7 )
        {
            blackPawnCaptures[ loop ] |= mask << ( loop - 7 );
        }
    }

    // Knights
    // Represent all knight moves from a central location and then encode that into a 0x88 space
    // 01010
    // 10001
    // 00*00
    // 10001
    // 01010
    pieceSquares = 0;

    pieceSquares |= 0b00001010;
    pieceSquares <<= 16;

    pieceSquares |= 0b00010001;
    pieceSquares <<= 16;

    pieceSquares |= 0b00000000; // The central location being here 0b00000*00
    pieceSquares <<= 16;

    pieceSquares |= 0b00010001;
    pieceSquares <<= 16;

    pieceSquares |= 0b00001010;

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
        x88 = offset < 0 ? pieceSquares >> -offset : pieceSquares << offset;

        knightMoves[ index ] = bitboardFrom0x88( x88 );
    }

    // Bishop and Queens
    for ( unsigned short index = 0; index < 64; index++ )
    {
        path = index;
        while ( Utilities::indexToFile( path ) != 0 && Utilities::indexToRank( path ) != 0 )
        {
            path -= 9;
          
            bishopMoves[ index ] |= mask << path;
            queenMoves[ index ] |= mask << path;
        }

        path = index;
        while ( Utilities::indexToFile( path ) != 0 && Utilities::indexToRank( path ) != 7 )
        {
            path += 7;

            bishopMoves[ index ] |= mask << path;
            queenMoves[ index ] |= mask << path;
        }

        path = index;
        while ( Utilities::indexToFile( path ) != 7 && Utilities::indexToRank( path ) != 0 )
        {
            path -= 7;

            bishopMoves[ index ] |= mask << path;
            queenMoves[ index ] |= mask << path;
        }

        path = index;
        while ( Utilities::indexToFile( path ) != 7 && Utilities::indexToRank( path ) != 7 )
        {
            path += 9;

            bishopMoves[ index ] |= mask << path;
            queenMoves[ index ] |= mask << path;
        }
    }

    // Rooks and Queens
    for ( unsigned short index = 0; index < 64; index++ )
    {
        path = index;
        while ( Utilities::indexToRank( path ) != 0 )
        {
            path -= 8;

            rookMoves[ index ] |= mask << path;
            queenMoves[ index ] |= mask << path;
        }

        path = index;
        while ( Utilities::indexToRank( path ) != 7 )
        {
            path += 8;

            rookMoves[ index ] |= mask << path;
            queenMoves[ index ] |= mask << path;
        }

        path = index;
        while ( Utilities::indexToFile( path ) != 0 )
        {
            path --;

            rookMoves[ index ] |= mask << path;
            queenMoves[ index ] |= mask << path;
        }

        path = index;
        while ( Utilities::indexToFile( path ) != 7 )
        {
            path ++;

            rookMoves[ index ] |= mask << path;
            queenMoves[ index ] |= mask << path;
        }
    }

    // King
    // Represent all king moves from a central location and then encode that into a 0x88 space
    // 111
    // 1*1
    // 111
    pieceSquares = 0;

    pieceSquares |= 0b000000111;
    pieceSquares <<= 16;

    pieceSquares |= 0b000000101; // The central location being here 0b000000*0
    pieceSquares <<= 16;

    pieceSquares |= 0b000000111;

    // In this layout, centre square is 1,1 and the '1's in the mask are move destinations from there
    for ( unsigned short index = 0; index < 64; index++ )
    {
        // Need to move to get mask to 0,0
        short fileOffset = -1;
        short rankOffset = -1;

        // And then this amount to move to 'index' on a normal board
        rankOffset += ( index >> 3 );
        fileOffset += ( index & 7 );

        // Convrted to a movement into the 128-bit structure 
        int offset = ( rankOffset << 4 ) + fileOffset;

        // Shift left or right, accordingly
        x88 = offset < 0 ? pieceSquares >> -offset : pieceSquares << offset;

        kingMoves[ index ] = bitboardFrom0x88( x88 );
    }

    LOG_TRACE << "Done creating bitboards";
}

unsigned long long Bitboard::bitboardFrom0x88( std::bitset<128>& bits )
{
    unsigned long long result = 0;

    // For each line of the 0x88 board
    unsigned long long mask = 1;
    for ( unsigned short rank = 0; rank < 8; rank++ )
    {
        // Next set of bits (rank on the bitboard)
        for ( unsigned short file = 0; file < 8; file++ )
        {
            if ( bits.test( ( rank << 4 ) + file ) )
            {
                result |= mask;
            }

            mask <<= 1;
        }
    }

    return result;
}

/// <summary>
/// Rotate a bitboard by 180 degrees. 
/// Square a1 is mapped to h8, and a8 is mapped to h1.
/// https://www.chessprogramming.org/Flipping_Mirroring_and_Rotating 
/// </summary>
/// <param name="x">a bitboard</param>
/// <returns>the rotated bitboard</returns>
unsigned long long Bitboard::rotate180( unsigned long long x )
{
    const unsigned long long h1 = 0x5555555555555555;
    const unsigned long long h2 = 0x3333333333333333;
    const unsigned long long h4 = 0x0F0F0F0F0F0F0F0F;
    const unsigned long long v1 = 0x00FF00FF00FF00FF;
    const unsigned long long v2 = 0x0000FFFF0000FFFF;
    x = ( ( x >> 1 ) & h1 ) | ( ( x & h1 ) << 1 );
    x = ( ( x >> 2 ) & h2 ) | ( ( x & h2 ) << 2 );
    x = ( ( x >> 4 ) & h4 ) | ( ( x & h4 ) << 4 );
    x = ( ( x >> 8 ) & v1 ) | ( ( x & v1 ) << 8 );
    x = ( ( x >> 16 ) & v2 ) | ( ( x & v2 ) << 16 );
    x = ( x >> 32 ) | ( x << 32 );
    return x;
}
