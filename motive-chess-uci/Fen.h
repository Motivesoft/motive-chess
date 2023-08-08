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

    Fen( std::string position )
    {
        // Fen starts at eighth rank and first file, so the indexing here looks a little goofy
        std::string::iterator it;
        short index = 56;

        LOG_DEBUG << "Processing FEN string " << position;

        std::fill( board.begin(), board.end(), Piece::nn );

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

            if ( index < 0 || *it == ' ' )
            {
                LOG_DEBUG << "Finished board extraction with index at " << index << " (expected at 8)";
                break;
            }
        }
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

