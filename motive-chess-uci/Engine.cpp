#include <cstdlib>
#include <ctime>
#include <thread>
#include <vector>

#include "Board.h"
#include "Engine.h"
#include "GameContext.h"
#include "Move.h"
#include "Utilities.h"

#define UCI_DEBUG Engine::UciLogger( *this, Logger::Level::DEBUG ).log( "" )
#define UCI_INFO  Engine::UciLogger( *this, Logger::Level::INFO ).log( "" )
#define UCI_WARN  Engine::UciLogger( *this, Logger::Level::WARN ).log( "Warning - " )
#define UCI_ERROR Engine::UciLogger( *this, Logger::Level::ERROR ).log( "Error - " )

void Engine::uciCommand()
{
    UCI_DEBUG << "Starting engine";

    // TODO do any actual initialization/reset here

    // TODO Pull this from resources and maybe add major version
    broadcaster.id( "MotiveChess", "Motivesoft" );
    
    // TODO broadcast options,...
    listVisibleOptions();

    // Send OK
    initialized = true;
    broadcaster.uciok();

    // TODO implement copy protection check
    broadcaster.copyprotection( CopyProtection::Status::CHECKING );
    broadcaster.copyprotection( CopyProtection::Status::OK );

    // TODO implement registration check
    broadcaster.registration( Registration::Status::CHECKING );
    broadcaster.registration( Registration::Status::OK );
}

void Engine::debugCommand( std::vector<std::string>& arguments )
{
    UCI_DEBUG << "Received debug";

    if ( arguments.size() > 0 )
    {
        if ( arguments[ 0 ] == "on" )
        {
            debugImpl( Engine::DebugSwitch::ON );
        }
        else if ( arguments[ 0 ] == "off" )
        {
            debugImpl( Engine::DebugSwitch::OFF );
        }
        else
        {
            UCI_ERROR << "Unknown debug argument: " << arguments[ 0 ];
        }
    }
    else
    {
        UCI_ERROR << "Missing debug argument";
    }
}

void Engine::isreadyCommand()
{
    UCI_DEBUG << "Received isready";

    // TODO make sure we are actually ready, once there is something running here
    isreadyImpl();

    broadcaster.readyok();
}

void Engine::setoptionCommand( std::vector<std::string>& arguments )
{
    UCI_DEBUG << "Received setoption";

    // "name x" and, optionally, "value y" where x and y might contain spaces
    std::string name;
    std::string value;
    bool isName = false;
    bool isValue = false;

    // Expect either (name and/or code), or (later), assuming that's what the UCI spec intends
    for ( std::vector<std::string>::iterator it = arguments.begin(); it != arguments.end(); it++ )
    {
        if ( *it == "name" )
        {
            name.clear();
            isName = true;
            isValue = false;
        }
        else if ( *it == "value" )
        {
            value.clear();
            isValue = true;
            isName = false;
        }
        else if ( isName )
        {
            if ( name.length() > 0 )
            {
                name += " ";
            }

            name += *it;
        }
        else if ( isValue )
        {
            if ( value.length() > 0 )
            {
                value += " ";
            }

            value += *it;
        }
        else
        {
            UCI_ERROR << "Unexpected setoption argument: " << *it;
        }
    }

    if ( isName || isValue )
    {
        setoptionImpl( name, value );
    }
}

void Engine::registerCommand( std::vector<std::string>& arguments )
{
    UCI_DEBUG << "Received register";

    // "later", or some combo of "name x" and "code y" where x and y might contain spaces
    std::string name;
    std::string code;
    bool later = false;
    bool isName = false;
    bool isCode = false;

    // Expect either (name and/or code), or (later), assuming that's what the UCI spec intends
    for ( std::vector<std::string>::iterator it = arguments.begin(); it != arguments.end(); it++ )
    {
        if ( *it == "name" )
        {
            name.clear();
            isName = true;
            isCode = false;
        }
        else if ( *it == "code" )
        {
            code.clear();
            isCode = true;
            isName = false;
        }
        else if ( isName )
        {
            if ( name.length() > 0 )
            {
                name += " ";
            }

            name += *it;
        }
        else if ( isCode )
        {
            if ( code.length() > 0 )
            {
                code += " ";
            }

            code += *it;
        }
        else if ( *it == "later" )
        {
            later = true;
            break;
        }
        else
        {
            UCI_ERROR << "Unexpected register argument: " << *it;
        }
    }

    if ( later )
    {
        registerImpl();
    }
    else if ( isName || isCode )
    {
        registerImpl( name, code );
    }
}

void Engine::ucinewgameCommand()
{
    UCI_DEBUG << "Received ucinewgame";

    if ( ucinewgameExpected )
    {
        ucinewgameReceived = true;

        // TODO anything else?
        LOG_TRACE << "Stopping";
        stopImpl();

        LOG_TRACE << "Releasing game context";
        releaseGameContext();
    }
    else
    {
        UCI_WARN << "Command ucinewgame received out of sequence. Ignoring";
    }
}

void Engine::positionCommand( std::vector<std::string>& arguments )
{
    UCI_DEBUG << "Received position";

    if ( !ucinewgameReceived )
    {
        ucinewgameExpected = false;

        // No ucinewgame, so we need to stop and reinitialise
        LOG_TRACE << "Preparing for new game";
        stopImpl();
        releaseGameContext();
    }

    std::string fen;
    std::vector<std::string> moves;
    for ( std::vector<std::string>::iterator it = arguments.begin(); it != arguments.end(); )
    {
        if ( *it == "startpos" )
        {
            fen = Fen::startingPosition;
            it++;
        }
        else if ( *it == "fen" )
        {
            // New FEN string, so clear moves as a safety measure
            moves.clear();

            std::stringstream stream;

            it++;
            for ( ; it != arguments.end(); it++ )
            {
                if ( *it == "moves" )
                {
                    break;
                }

                if ( !stream.str().empty() )
                {
                    stream << " ";
                }

                stream << *it;
            }

            fen = stream.str();
        }
        else if ( *it == "moves" )
        {
            // Reset collection as a safety measure
            moves.clear();

            it++;
            for ( ; it != arguments.end(); it++ )
            {
                moves.push_back( *it );
            }
        }
        else
        {
            // Assuming missing 'moves' section
            LOG_WARN << "Unknown or unexpected position argument " << *it;
            break;
        }
    }

    if ( !fen.empty() )
    {
        LOG_DEBUG << "Position with FEN [" << fen << "] and " << moves.size() << " moves";

        positionImpl( fen, moves );
    }
    else
    {
        UCI_ERROR << "Illegal position command";
    }
}

void Engine::goCommand( std::vector<std::string>& arguments )
{
    UCI_DEBUG << "Received go";

    // TODO implement
    std::vector<std::string> searchMoves;
    bool ponder = false;
    unsigned int wtime = 0;
    unsigned int btime = 0;
    unsigned int winc = 0;
    unsigned int binc = 0;
    unsigned int movestogo = 0;
    unsigned int depth = 0;
    unsigned int nodes = 0;
    unsigned int mate = 0;
    unsigned int movetime = 0;
    bool infinite = false;

    bool parseError = false;

    for ( std::vector<std::string>::iterator it = arguments.begin(); it != arguments.end(); )
    {
        if ( *it == "searchmoves" )
        {
            std::vector<std::string> directives = getGoDirectives();

            for ( ; it != arguments.end(); it++ )
            {
                if ( std::find( directives.begin(), directives.end(), *it ) != directives.end() )
                {
                    break;
                }

                searchMoves.push_back( *it );
            }
        }
        else if ( *it == "ponder" )
        {
            ponder = true;
        }
        else if ( *it == "wtime" )
        {
            it++;
            if ( it != arguments.end() )
            {
                wtime = stoi( *it );
            }
            else
            {
                parseError = true;
            }
        }
        else if ( *it == "btime" )
        {
            it++;
            if ( it != arguments.end() )
            {
                btime = stoi( *it );
            }
            else
            {
                parseError = true;
            }
        }
        else if ( *it == "winc" )
        {
            it++;
            if ( it != arguments.end() )
            {
                winc = stoi( *it );
            }
            else
            {
                parseError = true;
            }
        }
        else if ( *it == "binc" )
        {
            it++;
            if ( it != arguments.end() )
            {
                binc = stoi( *it );
            }
            else
            {
                parseError = true;
            }
        }
        else if ( *it == "movestogo" )
        {
            it++;
            if ( it != arguments.end() )
            {
                movestogo = stoi( *it );
            }
            else
            {
                parseError = true;
            }
        }
        else if ( *it == "depth" )
        {
            it++;
            if ( it != arguments.end() )
            {
                depth = stoi( *it );
            }
            else
            {
                parseError = true;
            }
        }
        else if ( *it == "nodes" )
        {
            it++;
            if ( it != arguments.end() )
            {
                nodes = stoi( *it );
            }
            else
            {
                parseError = true;
            }
        }
        else if ( *it == "mate" )
        {
            it++;
            if ( it != arguments.end() )
            {
                mate = stoi( *it );
            }
            else
            {
                parseError = true;
            }
        }
        else if ( *it == "movetime" )
        {
            it++;
            if ( it != arguments.end() )
            {
                movetime = stoi( *it );
            }
            else
            {
                parseError = true;
            }
        }
        else if ( *it == "infinite" )
        {
            infinite = true;
        }

        if ( parseError )
        {
            break;
        }

        it++;
    }

    if ( !parseError )
    {
        // Be aware that this gets deleted elsewhere - TODO do something about this eventually?
        GoContext* goContext = new GoContext( searchMoves, ponder, wtime, btime, winc, binc, movestogo, depth, nodes, mate, movetime, infinite );

        goImpl( goContext );
    }
    else
    {
        UCI_ERROR << "Parsing issue with go command";
    }
}

void Engine::stopCommand()
{
    UCI_DEBUG << "Received stop";

    stopImpl( ThinkingOutcome::BROADCAST );

    // TODO do something now we've stopped - bestmove and possibly ponder
    if ( !quitting )
    {

    }
}

void Engine::ponderhitCommand()
{
    UCI_DEBUG << "Received ponderhit";

    // TODO implement
}

bool Engine::quitCommand()
{
    UCI_DEBUG << "Received quit";

    quitting = true;

    stopImpl();
    isreadyImpl();

    return true;
}

// Special perft command

void Engine::perftCommand( std::vector<std::string>& arguments )
{
    UCI_DEBUG << "Received perft";

    // Command line syntax:
    //  perft [depth] <fen> <expected>
    //      depth is an integer search depth in half-moves
    //      fen is a starting position FEN string. We will use the default if it comes to it
    //      a set of expected results in the form <;Dx y> where x is depth and y is the expected node count
    // e.g. 
    //      perft 1 4k3/8/8/8/8/8/8/4K2R w K - 0 1 ;D1 15 ;D2 66 ;D3 1197 ;D4 7059 ;D5 133987 ;D6 764643

    int depth = 0;
    std::string fenString;
    std::stringstream stream;
    std::vector<std::pair<unsigned int, unsigned int>> expectedResults;

    std::vector<std::string>::iterator it = arguments.begin();
    if ( it != arguments.end() )
    {
        // Read the depth (mandatory)
        depth = stoi( *(it++) );

        // Read the FEN string (treat as optional, but expected if there is anything later)
        for ( ; it != arguments.end(); it++ )
        {
            if ( ( *it )[ 0 ] == ';' )
            {
                break;
            }

            if ( !stream.str().empty() )
            {
                stream << " ";
            }

            stream << *it;
        }

        fenString = stream.str();

        for ( ; it != arguments.end(); it++ )
        {
            if ( ( *it )[ 0 ] == ';' )
            {
                if ( ( *it ).size() > 2 )
                {
                    if ( ( *it )[ 1 ] == 'D' )
                    {
                        unsigned int expectedDepth = stoi( (*it++).substr( 2 ) );
                        unsigned int expectedCount = stoi( *it );

                        LOG_TRACE << "Noting expected result for depth " << expectedDepth << " of " << expectedCount;
                        expectedResults.push_back( std::pair<unsigned int, unsigned int>( expectedDepth, expectedCount ) );
                    }
                    else
                    {
                        LOG_ERROR << "Unexpected result " << *it;
                    }
                }
                else
                {
                    LOG_ERROR << "Unexpected value " << *it;
                }
            }
            else
            {
                LOG_ERROR << "Unexpected argument" << *it;
            }
        }

        if ( fenString.empty() )
        {
            LOG_DEBUG << "No FEN string specified; using default";

            fenString = Fen::startingPosition;
        }

        Fen fen = Fen::fromPosition( fenString );
        Board board( fen );

        LOG_INFO << "Starting perft run at depth " << depth << " with " << fenString;

        unsigned long nodes = perftImpl( depth, board );

        bool reported = false;
        for ( std::vector<std::pair<unsigned int, unsigned int>>::iterator it = expectedResults.begin(); it != expectedResults.end(); it++ )
        {
            if ( depth == ( *it ).first )
            {
                if ( nodes == ( *it ).second )
                {
                    LOG_INFO << "Total node count at depth " << depth << " is " << nodes;
                }
                else
                {
                    LOG_ERROR << "Total node count at depth " << depth << " is " << nodes << " but expected to be " << (*it).second;
                }

                reported = true;
                break;
            }
        }

        if ( !reported )
        {
            LOG_INFO << "Total node count at depth " << depth << " is " << nodes;
        }
    }
    else
    {
        UCI_ERROR << "Missing depth in perft command";
    }
}

// Helper methods

void Engine::listVisibleOptions()
{
    broadcaster.option( OPTION_BENCH, benchmarking );
}
 
// Silent implementations - do the work, but do not directly communicate over uci, allowing the 
// methods to be called from elsewhere

void Engine::stopImpl( ThinkingOutcome thinkingOutcome )
{
    if ( thinkingThread == nullptr )
    {
        return;
    }

    LOG_TRACE << "Stopping thinking";

    broadcastThinkingOutcome = ( thinkingOutcome == ThinkingOutcome::BROADCAST );
    
    // Set this volatile switch and it will be detected by a thinking thread if one is active
    continueThinking = false;

    // Wait for the thread to stop
    thinkingThread->join();

    LOG_TRACE << "Thread stopped";

    // Housekeeping
    delete thinkingThread;
    thinkingThread = nullptr;

    delete thinkingBoard;
    thinkingBoard = nullptr;
}

void Engine::isreadyImpl()
{

}

void Engine::debugImpl( Engine::DebugSwitch flag )
{
    LOG_INFO << "Setting debug to [" << (flag == DebugSwitch::ON ? "on" : "off") << "]";

    debugging = flag;
}

void Engine::registerImpl()
{
    LOG_INFO << "Register later";

    broadcaster.registration( Registration::Status::CHECKING );

    bool registered = registration.registerLater();

    broadcaster.registration( registered ? Registration::Status::OK : Registration::Status::ERROR );
}

void Engine::registerImpl( std::string& name, std::string& code )
{
    LOG_INFO << "Register with name [" << name << "] and code [" << code << "]";

    broadcaster.registration( Registration::Status::CHECKING );

    bool registered = registration.registerNameCode( name, code );
    
    broadcaster.registration( registered ? Registration::Status::OK : Registration::Status::ERROR );
}

void Engine::setoptionImpl( std::string& name, std::string& value )
{
    LOG_INFO << "SetOption with name [" << name << "] and value [" << value << "]";

    // Store the new value
    if ( name == OPTION_BENCH )
    {
        setBenchmarking( value == "true" );
    }
}

void Engine::positionImpl( const std::string& fenString, std::vector<std::string> moves )
{
    LOG_INFO << "Processing FEN string " << fenString << " and " << moves.size() << " moves";
    Fen fen = Fen::fromPosition( fenString );

    std::vector< Move > moveList;
    if ( moves.size() > 0 )
    {
        LOG_DEBUG << "Initial moves:";

        for ( std::string move : moves )
        {
            Move m = Move::fromString( move );

            moveList.push_back( m );
            LOG_DEBUG << m.toString();
        }
    }

    // Set the new game context
    gameContext = new GameContext( fen, moveList );

    // Reset for next game
    ucinewgameReceived = false;
}

void Engine::goImpl( GoContext* goContext )
{
    LOG_INFO << "Go: depth=" << goContext->getDepth();

    stopImpl();

    // If we get a go without a prior position, go with a default setup
    if ( gameContext == nullptr )
    {
        positionImpl( Fen::startingPosition, std::vector<std::string>() );
    }

    // Report bestmove when thinking is done
    broadcastThinkingOutcome = true;

    // Construct the position to think from
    Board initialBoard( gameContext->getFEN() );
    for ( std::vector<Move>::const_iterator it = gameContext->getMoves().begin(); it != gameContext->getMoves().end(); it++ )
    {
        initialBoard = initialBoard.makeMove( *it );
    }

    // Use this as the board to work from
    thinkingBoard = new Board( initialBoard );
    thinkingThread = new std::thread( &Engine::thinking, this, thinkingBoard, goContext );

    LOG_TRACE << "Thread " << thinkingThread->get_id() << " running";
}

// Special perft command

unsigned long Engine::perftImpl( int depth, Board board )
{
    const std::string padding = "                    ";
    unsigned long nodes = 0;

    if ( depth == 0 )
    {
        // I don't see why this should be made to return 1, but that does seem to be the 
        // accepted way
        return 1;
    }

    std::vector<Move> moves;
    moves = board.getPseudoLegalMoves();

    for ( std::vector<Move>::iterator it = moves.begin(); it != moves.end(); )
    {
        Board tBoard = board.makeMove( *it );
        std::vector<Move> tMoves = tBoard.getPseudoLegalMoves();

        bool refuted = false;
        for ( std::vector<Move>::iterator tIt = tMoves.begin(); tIt != tMoves.end(); tIt++ )
        {
            if ( tBoard.isRefutation( *it, *tIt ) )
            {
                refuted = true;
                break;
            }
        }

        if ( refuted )
        {
            it = moves.erase( it );
        }
        else
        {
            if ( depth > 0 )
            {
                nodes += perftImpl( depth - 1, tBoard );
            }
            
            LOG_DEBUG << padding.substr( 0, depth ) << ( *it ).toString();

            it++;
        }
    }

    return nodes;
}

// Internal methods

class Thoughts
{
private:
    Move bestMove;
    Move ponderMove;
public:
    Thoughts( Move bestMove = Move::nullMove, Move ponderMove = Move::nullMove ) :
        bestMove( bestMove ),
        ponderMove( ponderMove )
    {

    }

    Move getBestMove()
    {
        return bestMove;
    }

    Move getPonderMove()
    {
        return ponderMove;
    }
};

void Engine::thinking( Engine* engine, Board* board, GoContext* context )
{
    // TODO remove this when no longer required
    std::srand( static_cast<unsigned int>( std::time( nullptr ) ) );

    // Test this repeatedly for interuptions
    engine->continueThinking = true;

    // OK, beging the thinking process - we have to test flags, but make sure we have a candidate move
    // before being interrupted - unless we are quitting

    Thoughts thoughts;

    unsigned int loop = 0;
    bool readyToMove = false;
    while ( !readyToMove && !engine->quitting )
    {
        do
        {
            std::vector<Move> candidateMoves = board->getPseudoLegalMoves();
            if ( candidateMoves.empty() )
            {
                LOG_DEBUG << "No candidate moves";
                break;
            }

            for ( std::vector<Move>::iterator it = candidateMoves.begin(); it != candidateMoves.end(); )
            {
                LOG_TRACE << "PseudoLegal move " << ( *it ).toString();

                Board tBoard = board->makeMove( *it );
                std::vector<Move> tMoves = tBoard.getPseudoLegalMoves();

                LOG_TRACE << "Looking for refutations";
                bool refuted = false;
                for ( std::vector<Move>::iterator tIt = tMoves.begin(); tIt != tMoves.end(); tIt++ )
                {
                    if ( tBoard.isRefutation( *it, *tIt ) )
                    {
                        refuted = true;
                        break;
                    }
                }

                if ( refuted )
                {
                    LOG_TRACE << "Move refuted: " << ( *it ).toString();
                    it = candidateMoves.erase( it );
                }
                else
                {
                    it++;
                }
            }

            // List of moves after refutations have been removed
            // TODO reduce this to TRACE
            LOG_DEBUG << "Search list:";
            for ( std::vector<Move>::iterator it = candidateMoves.begin(); it != candidateMoves.end(); it++ )
            {
                LOG_DEBUG << ( *it ).toString();
            }

            if ( thoughts.getBestMove().isNullMove() )
            {
                // TODO don't select target move randomly!
                int random = std::rand();
                int randomMove = random % candidateMoves.size();
                thoughts = Thoughts( candidateMoves[ randomMove ] );

                if ( candidateMoves.size() == 1 )
                {
                    // Don't waste clock time analysing a forced move situation
                    LOG_DEBUG << "Only one move available";
                    readyToMove = true;
                    break;
                }
            }

            // TODO do work here

            // TODO this probably wants to be a better check
            if ( loop++ >= context->getDepth() )
            {
                LOG_DEBUG << "Reached search depth";
                readyToMove = true;
                break;
            }
        }
        while ( engine->continueThinking && !engine->quitting );
    }

    if ( engine->broadcastThinkingOutcome )
    {
        if ( thoughts.getPonderMove().isNullMove() )
        {
            LOG_DEBUG << "Broadcasting best move: " << thoughts.getBestMove().toString();

            engine->broadcaster.bestmove( thoughts.getBestMove().toString() );
        }
        else
        {
            LOG_DEBUG << "Broadcasting best move: " << thoughts.getBestMove().toString() << " with ponder: " << thoughts.getPonderMove().toString();

            engine->broadcaster.bestmove( thoughts.getBestMove().toString(), thoughts.getPonderMove().toString() );
        }
    }

    LOG_DEBUG << "Thinking thread terminating";
    delete context;
}