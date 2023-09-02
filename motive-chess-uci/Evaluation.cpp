#include "Evaluation.h"

#include <vector>

#include "Board.h"
#include "Move.h"
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
short Evaluation::scorePosition( Board board )
{
    //Utilities::dumpBoard( board.pieces );

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

        if ( ( piece & 0b00000111 ) == 0b00000001 ) // PAWN
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

short Evaluation::minimax( Board board, unsigned short depth, short alphaInput, short betaInput, bool maximising )
{
    static const std::string spaces( "                                                                                                                  " );

    // Make some working values so we are not "editing" method parameters
    short alpha = alphaInput;
    short beta = betaInput;

    // If is win, return max
    // If is loss, return lowest
    // If draw, return 0
    // otherwise iterate

    // Simple win semantics
    short score = 0;
    if ( board.isTerminal( &score ) )
    {
        Log::Debug << spaces.substr( 0, depth ) << "Detected terminal position: " << score << std::endl;

        // Why? Win, Loss or Stalemate
        return score == 0 ? 0 : score * 1000;
    }

    if ( depth == 0 )
    {
        Log::Debug << spaces.substr( 0, depth ) << "Depth 0, scoring position: " << score << std::endl;
        return scorePosition( board );
    }

    if ( maximising )
    {
        Log::Debug << spaces.substr( 0, depth ) << "Maximising at depth " << depth << std::endl;
        score = std::numeric_limits<short>::lowest();
        std::vector<Move> moves = board.getMoves();

        Log::Debug << spaces.substr( 0, depth ) << "Move count: " << moves.size() << std::endl;
        for ( std::vector<Move>::iterator it = moves.begin(); it != moves.end(); it++ )
        {
            Log::Debug << spaces.substr( 0, depth ) << "Move: " << (*it).toString() << std::endl;
            short evaluation = minimax( board.makeMove( *( it ) ), depth - 1, alpha, beta, !maximising );
            Log::Debug << spaces.substr( 0, depth ) << "  - eval: " << evaluation << std::endl;
            if ( evaluation > score )
            {
                score = evaluation;
            }
            if ( evaluation > alpha )
            {
                alpha = evaluation;
            }
            if ( beta <= alpha )
            {
                break;
            }
        }

        Log::Debug << spaces.substr( 0, depth ) << "Done maximising at depth " << depth << " with score " << score << std::endl;
        return score;
    }
    else
    {
        Log::Debug << spaces.substr( 0, depth ) << "Minimising at depth " << depth << std::endl;
        score = std::numeric_limits<short>::max();
        std::vector<Move> moves = board.getMoves();

        Log::Debug << spaces.substr( 0, depth ) << "Move count: " << moves.size() << std::endl;
        for ( std::vector<Move>::iterator it = moves.begin(); it != moves.end(); it++ )
        {
            Log::Debug << spaces.substr( 0, depth ) << "Move: " << (*it).toString() << std::endl;
            short evaluation = minimax( board.makeMove( *( it ) ), depth - 1, alpha, beta, !maximising );
            Log::Debug << spaces.substr( 0, depth ) << "  - eval: " << evaluation << std::endl;
            if ( evaluation < score )
            {
                score = evaluation;
            }
            if ( evaluation < alpha )
            {
                alpha = evaluation;
            }
            if ( beta <= alpha )
            {
                break;
            }
        }

        Log::Debug << spaces.substr( 0, depth ) << "Done minimising at depth " << depth << " with score " << score << std::endl;
        return score;
    }
}
