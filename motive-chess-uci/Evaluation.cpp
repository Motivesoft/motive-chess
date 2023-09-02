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

//        score += ( Piece::isColor( piece, board.activeColor ) ? -pieceWeights[ piece & 0b00000111 ] : pieceWeights[ piece & 0b00000111 ] );
        // Score this from one player's perspective, always
        score += ( Piece::isWhite( piece ) ? pieceWeights[ piece & 0b00000111 ] : -pieceWeights[ piece & 0b00000111 ] );
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
//                score += advancement[ Utilities::indexToRank( index ) ] * pawnAdvancementFile[ Utilities::indexToFile( index ) ];
            }
        }
    }

    return score;
}

short Evaluation::minimax( Board board, unsigned short depth, short alphaInput, short betaInput, bool maximising )
{
    static const std::string spaces( "                                                                                                                  " );
    
    std::string space = spaces.substr( 0, depth * 2 );

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
        Log::Debug << space << "Detected terminal position: " << score << std::endl;

        // Why? Win (+1), Loss (-1) or Stalemate (0)
        if ( score == 0 )
        {
            return 0;
        }
        else
        {
            // Give it a critially large value, but not quite at lowest/highest...
            score = score < 0 ? std::numeric_limits<short>::lowest() + 1000 : std::numeric_limits<short>::max() - 1000;

            // Adjusting the return with the depth means that it'll chase shorter lines to terminal positions rather
            // than just settling for a forced mate being something it can commit to at any time
            if ( score < 0 )
            {
                score -= depth;
            }
            else
            {
                score += depth;
            }

            return score;
        }
    }

    if ( depth == 0 )
    {
        score = scorePosition( board );
        //Log::Debug << space << "Depth 0, scoring position: " << score << std::endl;
        return score;
    }

    if ( maximising )
    {
        Log::Debug << space << "Maximising at depth " << depth << std::endl;
        score = std::numeric_limits<short>::lowest();
        std::vector<Move> moves = board.getMoves();

        Log::Debug << space << "Move count: " << moves.size() << std::endl;
        for ( std::vector<Move>::iterator it = moves.begin(); it != moves.end(); it++ )
        {
            Log::Debug << space << "Move: " << (*it).toString() << std::endl;
            short evaluation = minimax( board.makeMove( *( it ) ), depth - 1, alpha, beta, !maximising );
            Log::Debug << space << "  - eval: " << evaluation << std::endl;
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

        Log::Debug << space << "Done maximising at depth " << depth << " with score " << score << std::endl;
        return score;
    }
    else
    {
        Log::Debug << space << "Minimising at depth " << depth << std::endl;
        score = std::numeric_limits<short>::max();
        std::vector<Move> moves = board.getMoves();

        Log::Debug << space << "Move count: " << moves.size() << std::endl;
        for ( std::vector<Move>::iterator it = moves.begin(); it != moves.end(); it++ )
        {
            Log::Debug << space << "Move: " << (*it).toString() << std::endl;
            short evaluation = minimax( board.makeMove( *( it ) ), depth - 1, alpha, beta, !maximising );
            Log::Debug << space << "  - eval: " << evaluation << std::endl;
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

        Log::Debug << space << "Done minimising at depth " << depth << " with score " << score << std::endl;
        return score;
    }
}
