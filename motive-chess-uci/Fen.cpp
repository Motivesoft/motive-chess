#include <sstream>

#include "Fen.h"

Fen::Fen( std::string position )
{
    Log::Trace << "Processing FEN string " << position << std::endl;

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
                Log::Trace << "Placing " << *it << " at " << Utilities::indexToSquare( index ) << " (" << index << ")" << std::endl;
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

                Log::Trace << "Skipping " << digit.str() << " from " << index << std::endl;
                index += atoi( digit.str().c_str() );
                Log::Trace << " to " << index << std::endl;

                break;
            }

            case '/':
                // If we are not currently at the start of the next line, jump to it
                if ( ( index & 0b0111 ) != 0 )
                {
                    Log::Trace << "Jumping to end of line from " << index << std::endl;
                    index = ( index & ~0b0111 ) + 8;
                    Log::Trace << " to " << index << std::endl;
                }

                Log::Trace << "Stepping to next line from " << index << std::endl;
                index -= 16;
                Log::Trace << " to " << index << std::endl;

            default:
            case ' ':
                break;
        }

        if ( *it == ' ' )
        {
            Log::Trace << "Finished board extraction with index at " << index << " (expected at 8)" << std::endl;
            break;
        }
    }

    // The fields below are mandatory in a FEN string, but it is not uncommon to see examples
    // where some values (e.g. ep square) are excluded from the end. Try not to crash

    // Active color

    std::string activeColorString = nextWord( it, end );

    if ( activeColorString.empty() )
    {
        activeColor = Piece::getStartingColor();

        Log::Error << "FEN string is missing the active color value. Assuming " << Piece::toColorString( activeColor ) << std::endl;
    }
    else
    {
        activeColor = Piece::colorFrom( activeColorString );

        Log::Trace << "Active color: " << Piece::toColorString( activeColor ) << std::endl;
    }

    // Castling rights (whichever still available are presented from KQkq with '-' for none)

    std::string castling = nextWord( it, end );

    if ( castling == "-" || castling.empty() )
    {
        castlingRights = CastlingRights( false );

        Log::Trace << "No castling rights" << std::endl;
    }
    else
    {
        castlingRights = CastlingRights::fromFENString( castling );

        Log::Trace << "Castling rights for " << castling << " are " << castlingRights.toString() << std::endl;
    }

    // En passant target square or '-' for none

    std::string enPassantValue = nextWord( it, end );

    enPassantIndex = enPassantValue == "-" ? Utilities::getOffboardLocation() : Utilities::squareToIndex( enPassantValue );

    Log::Trace << "En passant square: " << ( Utilities::isOffboard( enPassantIndex ) ? "none" : Utilities::indexToSquare( enPassantIndex ) ) << std::endl;

    // Halfmove clock

    std::string halfmoveClockValue = nextWord( it, end );

    halfmoveClock = atoi( halfmoveClockValue.c_str() );

    Log::Trace << "Halfmove clock " << halfmoveClock << std::endl;

    // Fullmove number

    std::string fullmoveValue = nextWord( it, end );

    fullmoveNumber = atoi( fullmoveValue.c_str() );

    Log::Trace << "Fullmove number " << fullmoveNumber << std::endl;

    // Should be at end now

    if ( it != position.end() )
    {
        Log::Error << "Unexpected data at end of FEN string" << std::endl;
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

    skipSpace( it, end );

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
