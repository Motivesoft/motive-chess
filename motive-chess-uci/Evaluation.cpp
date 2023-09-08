#include "Evaluation.h"

#include <list>

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
short Evaluation::scorePosition( Board board, unsigned char color )
{
    short score = 0;

    // Piece differential
    for ( int index = 0; index < 64; index++ )
    {
        unsigned char piece = board.pieceAt( index );

        // Score this from one player's perspective, always
        score += ( Piece::isColor( piece, color ) ? pieceWeights[ piece & 0b00000111 ] : -pieceWeights[ piece & 0b00000111 ] );
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

short Evaluation::minimax( Board board, unsigned short depth, short alphaInput, short betaInput, bool maximising, unsigned char color )
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
        // Why? Win (+1), Loss (-1) or Stalemate (0)
        if ( score == 0 )
        {
            Log::Debug << "Score : 0" << std::endl;
            return 0;
        }
        else
        {
            if ( board.getActiveColor() != color )
            {
                score = -score;
            } 

            Log::Debug( [&] ( const Log::Logger logger )
            {
                logger << "Score: " << score << " Active color: " << Piece::toColorString( board.getActiveColor() ) << " Provided color: " << Piece::toColorString( color ) << std::endl;
            } );
            
            // Give it a critially large value, but not quite at lowest/highest...
            // so we have some wiggle room so we can make one winning line seem preferable to another
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
        score = scorePosition( board, color );
        return score;
    }

    if ( maximising )
    {
        score = std::numeric_limits<short>::lowest();
        std::unique_ptr<std::vector<Move*>> moves = board.getMoves();

        int count = 0;
        for ( std::vector<Move*>::iterator it = moves->begin(); it != moves->end(); it++, count++ )
        {
            short evaluation = minimax( board.makeMove( *it ), depth - 1, alpha, beta, !maximising, color );

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
                Log::Debug( [&] ( const Log::Logger& logger )
                {
                    logger << "Exiting maximising after " << count << "/" << moves->size() << " moves considered" << std::endl;
                } );
                break;
            }
        }

        return score;
    }
    else
    {
        score = std::numeric_limits<short>::max();
        std::unique_ptr<std::vector<Move*>> moves = board.getMoves();

        int count = 0;
        for ( std::vector<Move*>::iterator it = moves->begin(); it != moves->end(); it++, count++ )
        {
            short evaluation = minimax( board.makeMove( *it ), depth - 1, alpha, beta, !maximising, color );

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
                Log::Debug( [&] ( const Log::Logger& logger )
                {
                    logger << "Exiting minimising after " << count << "/" << moves->size() << " moves considered" << std::endl;
                } );
                break;
            }
        }

        return score;
    }
}
