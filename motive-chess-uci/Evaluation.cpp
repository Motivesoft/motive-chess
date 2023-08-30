#include "Evaluation.h"

#include "Log.h"

float Evaluation::score( const Board& board )
{
    float score = 0;

    float ownPieceScores[] =
    {
        0, 1, 3, 3, 5, 9, 0, 0
    };
    float enemyPieceScores[] =
    {
        -0, -1, -3, -3, -5, -9, -0, 0
    };

    // This is a simple material differential score - count with white in mind, and return negative if for black
    for ( unsigned short loop = 0; loop < 64; loop++ )
    {
        unsigned char piece = board.pieces[ loop ];

        auto x = ( Piece::isWhite(piece) ? &ownPieceScores : &enemyPieceScores );

        score += (*x)[ piece & 0b00000111 ];
    }

    // Note that the score here is when the candidate move has been made and is therefore 'activeColor' is the opponent
    return Piece::isWhite( board.activeColor ) ? -score : score;
}
