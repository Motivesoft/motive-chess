#include <sstream>

#include "Fen.h"

Fen::Fen( std::string position )
{
    LOG_DEBUG << "Processing FEN string " << position;

    std::string::iterator it;
    std::string::iterator end = position.end();

    // Default setup before processing

    std::fill( board.begin(), board.end(), Piece::NOTHING );

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
            {
                LOG_TRACE << "Placing " << *it << " at " << Utilities::indexToSquare( index ) << " (" << index << ")";
                board[ index++ ] = Piece::fromFENString( *it );
                break;
            }

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

                LOG_TRACE << "Skipping " << digit.str() << " from " << index;
                index += atoi( digit.str().c_str() );
                LOG_TRACE << " to " << index;

                break;
            }

            case '/':
                // If we are not currently at the start of the next line, jump to it
                if ( ( index & 0b0111 ) != 0 )
                {
                    LOG_TRACE << "Jumping to end of line from " << index;
                    index = ( index & ~0b0111 ) + 8;
                    LOG_TRACE << " to " << index;
                }

                LOG_TRACE << "Stepping to next line from " << index;
                index -= 16;
                LOG_TRACE << " to " << index;

            default:
            case ' ':
                break;
        }

        if ( *it == ' ' )
        {
            LOG_TRACE << "Finished board extraction with index at " << index << " (expected at 8)";
            break;
        }
    }

    LOG_DEBUG << "Board:";
    LOG_DEBUG << "  ABCDEFGH";
    for ( unsigned short rank = 0, rankIndex = 56; rank < 8; rank++, rankIndex -= 8 )
    {
        std::stringstream stream;
        for ( index = rankIndex; index < rankIndex + 8; index++ )
        {
            stream << ( board[ index ] == Piece::NOTHING ?
                        ( ( index & 1 ) == 0 ? "." : " " ) :
                        Piece::toFENString( board[ index ] ) );
        }

        LOG_DEBUG << 1 + rankIndex / 8 << " " << stream.str() << " " << 1 + rankIndex / 8;
    }
    LOG_DEBUG << "  ABCDEFGH";

    skipSpace( it, end );

    // Active color

    activeColor = Piece::colorFrom( *it++ );

    LOG_DEBUG << "Active color: " << Piece::toColorString( activeColor );

    skipSpace( it, end );

    // Castling rights (whichever still available are presented from KQkq with '-' for none)

    std::stringstream castling;

    while ( *it != ' ' )
    {
        castling << *it++;
    }

    if ( castling.str() == "-" )
    {
        castlingRights = CastlingRights( false );

        LOG_DEBUG << "No castling rights";
    }
    else
    {
        castlingRights = CastlingRights::fromFENString( castling.str() );

        LOG_DEBUG << "Castling rights for " << castling.str() << " are " << castlingRights.toString();
    }

    skipSpace( it, end );

    // En passant target square or '-' for none

    std::string enPassantValue = nextWord( it, end );

    enPassantIndex = enPassantValue == "-" ? Utilities::getOffboardLocation() : Utilities::squareToIndex( enPassantValue );

    LOG_DEBUG << "En passant square: " << ( Utilities::isOffboard( enPassantIndex ) ? "none" : Utilities::indexToSquare( enPassantIndex ) );

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

void Fen::skipSpace( std::string::iterator& it, std::string::iterator& end )
{
    while ( it != end && *it == ' ' )
    {
        it++;
    }
}

std::string Fen::nextWord( std::string::iterator& it, std::string::iterator& end )
{
    std::stringstream stream;

    stream.clear();

    while ( it != end && *it != ' ' )
    {
        stream << *it++;
    }

    return stream.str();
}
