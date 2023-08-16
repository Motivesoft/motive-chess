#pragma once

#include <bitset>
#include <memory>
#include <mutex>

#include "Logger.h"

#define Bitboards Bitboard::getInstance()

class Bitboard
{
private:
    static std::unique_ptr<Bitboard> instance;

    static unsigned long long pawnMoves[ 64 ];
    static unsigned long long pawnCaptures[ 64 ]; // Can include ep by checking the EP square
    static unsigned long long knightMoves[ 64 ];
    static unsigned long long bishopMoves[ 64 ];
    static unsigned long long rookMoves[ 64 ];
    static unsigned long long queenMoves[ 64 ];
    static unsigned long long kingMoves[ 64 ];

    static void buildBitboards();

    inline static std::mutex creationMutex;

    static unsigned long long bitboardFrom0x88( std::bitset<128>& bits );
    static unsigned long long rotate180( unsigned long long x );

public:
    static Bitboard* getInstance()
    {
        creationMutex.lock();
        if ( !instance )
        {
            instance = std::make_unique<Bitboard>();
            buildBitboards();
        }
        creationMutex.unlock();

        return instance.get();
    }

    inline unsigned long long getPawnMoves( unsigned short index )
    {
        return pawnMoves[ index ];
    }

    inline unsigned long long getPawnCaptures( unsigned short index )
    {
        return pawnCaptures[ index ];
    }

    inline unsigned long long getKnightMoves( unsigned short index )
    {
        return knightMoves[ index ];
    }

    inline unsigned long long getBishopMoves( unsigned short index )
    {
        return bishopMoves[ index ];
    }

    inline unsigned long long getRookMoves( unsigned short index )
    {
        return rookMoves[ index ];
    }

    inline unsigned long long getQueenMoves( unsigned short index )
    {
        return queenMoves[ index ];
    }

    inline unsigned long long getKingMoves( unsigned short index )
    {
        return kingMoves[ index ];
    }

    inline unsigned long long getKingsideCastlingMask()
    {
        return 0b01100000;
    }

    inline unsigned long long getKingsideCastlingTo()
    {
        return 0b01000000;
    }

    inline unsigned long long getQueensideCastlingMask()
    {
        return 0b00001110;
    }

    inline unsigned long long getQueensideCastlingTo()
    {
        return 0b00000100;
    }

    unsigned long long getFileMask( unsigned short file )
    {
        unsigned long long value = 0x0101010101010101;

        value <<= file;

        return value;
    }

    unsigned long long getRankMask( unsigned short rank )
    {
        unsigned long long value = 0xff;

        value <<= (rank * 8);

        return value;
    }

    unsigned long long getDiagonalMask( unsigned short file, unsigned short rank )
    {
        unsigned long long value = 0;

        unsigned short forigin = ( file == rank ) ? 0 : ( file < rank ) ? 0 : file-rank;
        unsigned short rorigin = ( file == rank ) ? 0 : ( file < rank ) ? rank-file : 0;

        for ( ; forigin < 8 && rorigin < 8; forigin++, rorigin++ )
        {
            value |= 1ull << ((rorigin<<3)+forigin);
        }

        return value;
    }

    unsigned long long getAntiDiagonalMask( unsigned short file, unsigned short rank )
    {
        unsigned long long value = 0;

        // Use short here, not unsigned short, as we need to check for wandering over the left-hand edge

        short rorigin = rank == ( 7 - file ) ? 0 : rank > ( 7 - file ) ? rank - ( 7 - file ) : 0;
        short forigin = rank == ( 7 - file ) ? 7 : rank > ( 7 - file ) ? 7 : file + rank;
        for ( ; rorigin < 8 && forigin >= 0; forigin--, rorigin++ )
        {
            value |= 1ull << ( ( rorigin << 3 ) + forigin );
        }
        /*
        if ( rank == ( 7 - file ) )
        {
            LOG_DEBUG << "PATH1";
            // Use signed here so we can check for going over the left-hand edge
            short rorigin = 0;
            short forigin = 7; // (effectively, 7-0)
            for ( ; rorigin < 8 && forigin >= 0; forigin--, rorigin++ )
            {
                value |= 1ull << ( ( rorigin << 3 ) + forigin );
            }
        }
        else if ( rank > ( 7 - file ) )
        {
            LOG_DEBUG << "PATH2";
            // Use signed here so we can check for going over the left-hand edge
            short rorigin = rank - ( 7 - file );
            short forigin = 7; // (effectively, 7-0)
            for ( ; rorigin < 8 && forigin >= 0; forigin--, rorigin++ )
            {
                value |= 1ull << ( ( rorigin << 3 ) + forigin );
            }
        }
        else if ( rank < ( 7 - file ) )
        {
            LOG_DEBUG << "PATH3";
            short rorigin = 0;
            short forigin = file + rank;
            for ( ; rorigin < 8 && forigin >= 0; forigin--, rorigin++ )
            {
                value |= 1ull << ( ( rorigin << 3 ) + forigin );
            }
        }
        */
        return value;
    }
};
