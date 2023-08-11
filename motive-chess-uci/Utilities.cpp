#include "Utilities.h"

#include "Logger.h"
#include "Piece.h"

void Utilities::dumpBoard( std::array< unsigned char, 64>& pieces )
{
    LOG_DEBUG << "Board:";
    LOG_DEBUG << "  ABCDEFGH";

    for ( unsigned short rank = 0, rankIndex = 56; rank < 8; rank++, rankIndex -= 8 )
    {
        std::stringstream stream;
        for ( unsigned short index = rankIndex; index < rankIndex + 8; index++ )
        {
            stream << ( Piece::isEmpty( pieces[ index ] ) ?
                        ( ( index & 1 ) == 0 ? "." : " " ) :
                        Piece::toFENString( pieces[ index ] ) );
        }

        LOG_DEBUG << 1 + rankIndex / 8 << " " << stream.str() << " " << 1 + rankIndex / 8;
    }

    LOG_DEBUG << "  ABCDEFGH";
}
