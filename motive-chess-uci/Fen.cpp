#include <sstream>

#include "Fen.h"

Fen::Fen( std::string position )
{
    LOG_DEBUG << "Processing FEN string " << position;

    std::string::iterator it;
    std::string::iterator end = position.end();

    // Default setup before processing

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
            {
                LOG_TRACE << "Placing " << *it << " at " << Utilities::indexToSquare( index ) << " (" << index << ")";
                LOG_TRACE << "PRE: Board[ " << index << " ] = " << board[ index ].toString() << ".Setting to " << *it;
                board[ index ] = Piece::fromFENString( *it );
                LOG_TRACE << "SET: Board[ " << index << " ] = " << board[ index ].toString() << ". Should be " << Piece::fromFENString( *it ).toString();
                index++;
                break;
            }
            //{
            //    LOG_TRACE << "Placing " << *it << " at " << Utilities::indexToSquare( index ) << " (" << index << ")";
            //    LOG_TRACE << "PRE: Board[ " << index << " ] = " << board[ index ] << ".Setting to " << *it;
            //    board[ index ] = Piece::pieceToByte( Piece::fromFENString( *it ) );
            //    LOG_TRACE << "SET: Board[ " << index << " ] = " << board[ index ] << ". Should be " << Piece::fromFENString( *it ).toString();
            //    index++;
            //    break;
            //}
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
            stream << ( board[ index ] == Piece::nn ?
                        ( ( index & 1 ) == 0 ? "." : " " ) :
                        Piece::toString( board[ index ] ) );
            //stream << ( board[ index ] == Piece::pieceToByte( Piece::nn ) ?
            //                ((index & 1) == 0 ? "." : " ") :
            //                Piece::toString( Piece::byteToPiece( board[ index ] ) ) );
        }

        LOG_DEBUG << 1 + rankIndex / 8 << " " << stream.str() << " " << 1 + rankIndex / 8;
    }
    LOG_DEBUG << "  ABCDEFGH";

    skipSpace( it, end );

    // Active color

    activeColor = *it++ == 'w' ? Piece::Color::WHITE : Piece::Color::BLACK;

    LOG_DEBUG << "Active color: " << ( activeColor == Piece::Color::WHITE ? "White" : "Black" );

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

    LOG_DEBUG << "En passant square: " << ( enPassantIndex == USHRT_MAX ? "none" : Utilities::indexToSquare( enPassantIndex ) );

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
