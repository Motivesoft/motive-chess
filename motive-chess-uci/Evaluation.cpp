#include "Evaluation.h"

#include "Log.h"
#include "Utilities.h"

// Array of 8 where we will ignore 0 and 7 (empty and unused, respecitively, from Piece definitions)
short Evaluation::pieceWeights[] =
{
    0, 100, 300, 300, 500, 900, 0, 0
};

short Evaluation::pawnAdvancementWhite[] =
{
    0, 0, 10, 15, 20, 25, 30, 40
};

short Evaluation::pawnAdvancementBlack[] =
{
    40, 30, 25, 20, 15, 10, 0, 0
};

short Evaluation::pawnAdvancementFile[] =
{
    1, 1, 2, 3, 3, 2, 1, 1
};

/// <summary>
/// Score the position from the perspective of NOT the activeColor player
/// </summary>
/// <param name="board">the board</param>
/// <returns>a centipawn score</returns>
short Evaluation::score( const Board& board )
{
    Utilities::dumpBoard( board.pieces );

    short score = 0;

    // Piece differential
    for ( int index = 0; index < 64; index++ )
    {
        unsigned char piece = board.pieceAt( index );

        score += ( Piece::isColor( piece, board.activeColor ) ? -pieceWeights[ piece & 0b00000111 ] : pieceWeights[ piece & 0b00000111 ] );
    }

    // Placement
    for ( int index = 0; index < 64; index++ )
    {
        unsigned char piece = board.pieceAt( index );

        if ( (piece & 0b00000111) == 0b00000001 ) // PAWN
        {
            auto& advancement = Piece::isWhite( piece ) ? pawnAdvancementWhite : pawnAdvancementBlack;

            if ( !Piece::isColor( piece, board.activeColor ) )
            {
                score += advancement[ Utilities::indexToRank( index ) ] * pawnAdvancementFile[ Utilities::indexToFile( index ) ];
            }
        }
    }

    return score;
}
