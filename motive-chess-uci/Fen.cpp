#include <sstream>

#include "Fen.h"

Fen::Fen( std::string position )
{
    LOG_TRACE << "Processing FEN string " << position;

    std::string::iterator it;
    std::string::iterator end = position.end();

    // Default setup before processing

    std::fill( pieces.begin(), pieces.end(), Piece::emptyPiece() );

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
                pieces[ index++ ] = Piece::fromFENString( *it );
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

    Utilities::dumpBoard( pieces );

    skipSpace( it, end );

    // Active color

    activeColor = Piece::colorFrom( *it++ );

    LOG_TRACE << "Active color: " << Piece::toColorString( activeColor );

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

        LOG_TRACE << "No castling rights";
    }
    else
    {
        castlingRights = CastlingRights::fromFENString( castling.str() );

        LOG_TRACE << "Castling rights for " << castling.str() << " are " << castlingRights.toString();
    }

    skipSpace( it, end );

    // En passant target square or '-' for none

    std::string enPassantValue = nextWord( it, end );

    enPassantIndex = enPassantValue == "-" ? Utilities::getOffboardLocation() : Utilities::squareToIndex( enPassantValue );

    LOG_TRACE << "En passant square: " << ( Utilities::isOffboard( enPassantIndex ) ? "none" : Utilities::indexToSquare( enPassantIndex ) );

    skipSpace( it, end );

    // Halfmove clock

    std::string halfmoveClockValue = nextWord( it, end );

    halfmoveClock = atoi( halfmoveClockValue.c_str() );

    LOG_TRACE << "Halfmove clock " << halfmoveClock;

    skipSpace( it, end );

    // Fullmove number

    std::string fullmoveValue = nextWord( it, end );

    fullmoveNumber = atoi( fullmoveValue.c_str() );

    LOG_TRACE << "Fullmove number " << fullmoveNumber;

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

std::string Fen::toString()
{
    std::stringstream fenString;

    // Component parts
    //   - position
    //   - active color
    //   - castling rights
    //   - en passant square
    //   - halfmove clock
    //   - fullmove number
    for ( unsigned short rank = 8; rank > 0; rank -- )
    {
        // Slash between ranks
        if ( rank < 8 )
        {
            fenString << "/";
        }

        unsigned short spaceCount = 0;
        for ( unsigned short file = 0; file < 8; file++ )
        {
            // From A8-H8 and then onto A7...
            unsigned short index = ((rank-1)<<3) + file;
            if ( Piece::isEmpty( pieces[ index ] ) )
            {
                spaceCount++;
                continue;
            }
            
            if ( spaceCount > 0 )
            {
                fenString << spaceCount;
                spaceCount = 0;
            }

            fenString << Piece::toFENString( pieces[ index ] );
        }

        if ( spaceCount > 0 )
        {
            fenString << spaceCount;
            spaceCount = 0;
        }
    }

    fenString << " " << Piece::toColorLetter( activeColor );
    fenString << " " << castlingRights.toString();
    fenString << " " << (Utilities::isOffboard( enPassantIndex ) ? "-" : Utilities::indexToSquare( enPassantIndex ));
    fenString << " " << halfmoveClock;
    fenString << " " << fullmoveNumber;

    return fenString.str();
}
