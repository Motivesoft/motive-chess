#pragma once

#include <array>
#include <sstream>
#include <string>

#include "Logger.h"
#include "Piece.h"
#include "Utilities.h"

class Fen
{
private:
    std::array< Piece, 64 > board;
    Piece::Color activeColor;
    bool castlingWK;
    bool castlingWQ;
    bool castlingBK;
    bool castlingBQ;
    unsigned short enPassantIndex;
    unsigned short halfmoveClock;
    unsigned short fullmoveNumber;

    Fen( std::string position )
    {
        LOG_DEBUG << "Processing FEN string " << position;

        std::string::iterator it;
        std::string::iterator end = position.end();

        // Default setup before processing

        std::fill( board.begin(), board.end(), Piece::nn );

        castlingWK = false;
        castlingWQ = false;
        castlingBK = false;
        castlingBQ = false;

        // Board contents - starts at eighth rank and first file, so the indexing here looks a little goofy

        short index = 56;

        for ( it = position.begin(); it != position.end(); it++ )
        {
            switch ( *it )
            {
                case 'K':
                case 'Q':
                case 'R':
                case 'B':
                case 'N':
                case 'P':
                case 'k':
                case 'q':
                case 'r':
                case 'b':
                case 'n':
                case 'p':
                    LOG_DEBUG << "Placing " << *it << " at " << Utilities::indexToSquare( index ) << " (" << index << ")";
                    board[ index++ ] = Piece::fromFENString( *it );
                    break;

                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                {
                    std::stringstream digit;
                    digit << *it;

                    LOG_DEBUG << "Skipping " << digit.str() << " from " << index;
                    index += atoi( digit.str().c_str() );
                    LOG_DEBUG << " to " << index;

                    break;
                }

                case '/':
                    // If we are not currently at the start of the next line, jump to it
                    if ( (index & 0b0111) != 0 )
                    {
                        LOG_DEBUG << "Jumping to end of line from " << index;
                        index = ( index & ~0b0111 ) + 8;
                        LOG_DEBUG << " to " << index;
                    }

                    LOG_DEBUG << "Stepping to next line from " << index;
                    index -= 16;
                    LOG_DEBUG << " to " << index;

                default:
                case ' ':
                    break;
            }

            if ( *it == ' ' )
            {
                LOG_DEBUG << "Finished board extraction with index at " << index << " (expected at 8)";
                break;
            }
        }

        skipSpace( it, end );

        // Active color

        activeColor = *it++ == 'w' ? Piece::Color::WHITE : Piece::Color::BLACK;

        LOG_DEBUG << "Active color: " << (activeColor == Piece::Color::WHITE ? "White" : "Black" );

        skipSpace( it, end );

        // Castling rights (whichever still available are presented from KQkq with '-' for none)

        while ( *it != ' ' )
        {
            if ( *it == 'K' )
            {
                castlingWK = true;

                LOG_DEBUG << "King-side castling available for White";
            }
            else if ( *it == 'Q' )
            {
                castlingWQ = true;

                LOG_DEBUG << "Queen-side castling available for White";
            }
            else if ( *it == 'k' )
            {
                castlingBK = true;

                LOG_DEBUG << "King-side castling available for Black";
            }
            else if ( *it == 'q' )
            {
                castlingBQ = true;

                LOG_DEBUG << "Queen-side castling available for Black";
            }
            else if ( *it != '-' )
            {
                LOG_ERROR << "Unexpected entry in castling section of FEN string " << *it;
            }

            it++;
        }

        skipSpace( it, end );

        // En passant target square or '-' for none

        std::string enPassantValue = nextWord( it, end );

        enPassantIndex = enPassantValue == "-" ? USHRT_MAX : Utilities::squareToIndex( enPassantValue );

        LOG_DEBUG << "En passant square: " << (enPassantIndex == USHRT_MAX ? "none" : Utilities::indexToSquare( enPassantIndex ) );

        skipSpace( it, end );

        // Halfmove clock

        std::string halfmoveClockValue = nextWord( it, end );

        halfmoveClock = atoi( halfmoveClockValue.c_str() );

        LOG_DEBUG << "Halfmove clock " << halfmoveClock;

        skipSpace( it, end );

        // Fullmove number

        std::string fullmoveValue = nextWord( it, end );

        fullmoveNumber = atoi( fullmoveValue.c_str() );

        LOG_DEBUG << "Fullmove number " << fullmoveNumber;

        // Should be at end now

        if ( it != position.end() )
        {
            LOG_ERROR << "Unexpected data at end of FEN string";
        }
    }

    void skipSpace( std::string::iterator& it, std::string::iterator& end )
    {
        while ( it != end && *it == ' ' )
        {
            it++;
        }
    }

    std::string nextWord( std::string::iterator& it, std::string::iterator& end )
    {
        std::stringstream stream;

        stream.clear();

        while ( it != end && *it != ' ' )
        {
            stream << *it++;
        }

        return stream.str();
    }

public:
    inline static const std::string startingPosition = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    static Fen fromStartingPosition()
    {
        return fromPosition( startingPosition );
    }

    static Fen fromPosition( std::string position )
    {
        return Fen( position );
    }

    Fen( Fen& fen )
    {
        //fen.board = board;
    }

    virtual ~Fen()
    {

    }


};

