#include <cstdlib>
#include <ctime>
#include <fstream>
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

    VersionInfo* versionInfo = VersionInfo::getVersionInfo();
    if ( versionInfo->isAvailable() )
    {
        broadcaster.id( versionInfo->getProductNameAndVersion(), versionInfo->getCompanyName() );
    }
    else
    {
        broadcaster.id( "MotiveChess", "Motivesoft" );
    }

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

    // TODO do something now we've stopped - bestmove and possibly ponder - currently we are doing this elsewhere
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

void Engine::perftCommand( std::vector<std::string>& arguments, bool expectsDepth )
{
    UCI_DEBUG << "Received perft";

    // Command line syntax:
    //  perft [depth] <fen> <expected>
    //      depth is an integer search depth in half-moves
    //      fen is a starting position FEN string. We will use the default if it comes to it
    //        fen may or may not include halfmove and fullmove numbers as they are part of FEN, but not EPD
    //      a set of expected results in one of the following formats:
    //        <;Dx y> where x is depth and y is the expected node count
    //        <,x,y,z,...> where x,y,z,... are the expected node counts for the associated depths (1, 2, 3, ...)
    // e.g. 
    //      perft 1 4k3/8/8/8/8/8/8/4K2R w K - 0 1 ;D1 15 ;D2 66 ;D3 1197 ;D4 7059 ;D5 133987 ;D6 764643
    //      perft 1 4k3/8/8/8/8/8/8/4K2R w K -,15,66,1197,7059,133987,764643

    int depth = 0;
    std::string fenString;
    std::stringstream stream;
    std::vector<std::pair<unsigned int, unsigned int>> expectedResults;

    // Rewrite 'arguments' to split anything containing a ';' or ',' because these are used
    // to add perft hints to the FEN string and need to be parsed out here

    for ( std::vector<std::string>::iterator it = arguments.begin(); it != arguments.end(); )
    {
        // Looking for either xxx,yyy or xxx;yyy - but not ,yyy or ;yyy
        
        // Comma?
        size_t splitPoint = ( *it ).find( "," );
        if ( splitPoint == 0 )
        {
            splitPoint = ( *it ).find( ",", 1 );
        }

        // No comma. Semi-colon?
        if ( splitPoint == std::string::npos )
        {
            splitPoint = ( *it ).find( ";" );
            if ( splitPoint == 0 )
            {
                splitPoint = ( *it ).find( ";", 1 );
            }
        }

        // Did we find either punctuation mark?
        if ( splitPoint != std::string::npos )
        {
            std::string part1 = ( *it ).substr( 0, splitPoint );
            std::string part2 = ( *it ).substr( splitPoint );

            // Remove the unsplit argument and replace with the split version - adding in
            // this order to preserve the overall structure
            it = arguments.erase( it );
            it = arguments.insert( it, part2 );
            it = arguments.insert( it, part1 );
        }
        else
        {
            it++;
        }
    }

    std::vector<std::string>::iterator it = arguments.begin();
    if ( it != arguments.end() )
    {
        if ( *it == "file" )
        {
            it++;

            if ( it == arguments.end() )
            {
                LOG_ERROR << "Missing filename";
            }
            else
            {
                std::string filename = *it;

                // Strip quotes - added when pasting a file path containing spaces
                while ( filename[ 0 ] == '"' )
                {
                    filename = filename.substr( 1 );
                }
                while ( filename[ filename.length() - 1 ] == '"' )
                {
                    filename = filename.substr( 0, filename.length() - 1 );
                }

                if ( filename.empty() )
                {
                    LOG_ERROR << "Empty filename";
                    return;
                }

                LOG_INFO << "Starting perft run from file " << filename;

                perftFile( filename );
            }

            return;
        }

        // Read the depth (if mandatory)
        if ( expectsDepth )
        {
            depth = stoi( *( it++ ) );
        }

        // Read the FEN string (treat as optional, but expected if there is anything later)
        for ( ; it != arguments.end(); it++ )
        {
            if ( ( *it )[ 0 ] == ';' || ( *it )[ 0 ] == ',' )
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

        unsigned int expectedDepth = 0;
        unsigned int expectedCount = 0;

        for ( ; it != arguments.end(); it++ )
        {
            switch ( ( *it )[ 0 ] )
            {
                case ';':// Expected structure: "<FEN>;D1 20; D2 400; D3 8902;..."
                    if ( ( *it ).size() > 2 )
                    {
                        if ( ( *it )[ 1 ] == 'D' )
                        {
                            expectedDepth = stoi( (*it++).substr( 2 ) );
                            expectedCount = stoi( *it );

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
                    break;

                case ',': // Expected structure: "<FEN>,20,400,8902,..."
                    expectedCount = stoi( ( *it ).substr( 1 ) );
                    expectedDepth++;

                    LOG_TRACE << "Noting expected result for depth " << expectedDepth << " of " << expectedCount;
                    expectedResults.push_back( std::pair<unsigned int, unsigned int>( expectedDepth, expectedCount ) );
                    break;

                default:
                    LOG_ERROR << "Unexpected argument" << *it;
                    break;
            }
        }

        if ( fenString.empty() )
        {
            LOG_DEBUG << "No FEN string specified; using default";

            fenString = Fen::startingPosition;
        }

        Fen fen = Fen::fromPosition( fenString );
        Board board( fen );


        if ( expectedResults.empty() )
        {
            LOG_INFO << "Starting perft run at depth " << depth << " with " << fenString;
            
            perftDepth( board, depth );
        }
        else
        {
            LOG_INFO << "Starting perft run with " << fenString;

            LOG_TRACE << "Expected results:";
            for ( std::vector<std::pair<unsigned int, unsigned int>>::iterator it = expectedResults.begin(); it != expectedResults.end(); it++ )
            {
                LOG_TRACE << "  Depth " << ( *it ).first << ". Count " << ( *it ).second;
            }

            perftRange( board, expectedResults );
        }
    }
    else
    {
        UCI_ERROR << "Missing depth in perft command";
    }
}

// Helper methods
void Engine::perftDepth( Board& board, int depth )
{
    clock_t now = clock();

    unsigned long nodes = perftImpl( depth, board, true );

    float elapsed = static_cast<float>( clock() - now ) / CLOCKS_PER_SEC;
    float nps = nodes / elapsed;

    LOG_INFO << "Total node count at depth " << depth << " is " << nodes << ". Time " << elapsed << "s (" << nps << " nps)";
}

void Engine::perftRange( Board& board, std::vector<std::pair<unsigned int, unsigned int>> expectedResults )
{
    for ( std::vector<std::pair<unsigned int, unsigned int>>::iterator it = expectedResults.begin(); it != expectedResults.end(); it++ )
    {
        unsigned int depth = ( *it ).first;
        unsigned int count = ( *it ).second;

        clock_t now = clock();

        unsigned long nodes = perftImpl( depth, board, true );

        float elapsed = static_cast<float>( clock() - now ) / CLOCKS_PER_SEC;
        float nps = nodes / elapsed;

        if ( nodes == count )
        {
            LOG_INFO << "Total node count at depth " << depth << " is " << nodes << ". Time " << elapsed << "s (" << nps << " nps)";
        }
        else
        {
            LOG_ERROR << "Total node count at depth " << depth << " is " << nodes << " but expected to be " << count << ". Time " << elapsed << "s (" << nps << " nps)";
        }
    }
}

void Engine::perftFile( std::string& filename )
{
    std::fstream file;
    file.open( filename, std::ios::in );

    if ( file.is_open() )
    {
        std::string line;
        while ( std::getline( file, line ) )
        {
            if ( line.empty() || line[ 0 ] == '#' )
            {
                LOG_TRACE << "Skipping empty or comment line";
                continue;
            }

            LOG_TRACE << "Read: " << line;

            // Split the line into tokens
            std::vector<std::string> arguments;
            std::string argument;
            for ( std::string::iterator it = line.begin(); it != line.end(); it++ )
            {
                if ( *it == ' ' )
                {
                    if ( !argument.empty() )
                    {
                        arguments.push_back( argument );
                        argument.clear();
                    }
                }
                else
                {
                    argument.push_back( *it );
                }
            }

            if ( !argument.empty() )
            {
                arguments.push_back( argument );
                argument.clear();
            }

            // This just happens to do the processing we want, although we are not providing a depth this way
            perftCommand( arguments, false );
        }
    }
    else
    {
        LOG_ERROR << "Failed to read file " << filename;
    }
}

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

unsigned long Engine::perftImpl( int depth, Board board, bool divide )
{
    unsigned long nodes = 0;

    if ( depth == 0 )
    {
        return 1;
    }

    std::vector<Move> moves = board.getMoves();

    for ( std::vector<Move>::iterator it = moves.begin(); it != moves.end(); it++ )
    {
        Board tBoard = board.makeMove( *it );

        if ( divide )
        {
            unsigned long moveNodes = perftImpl( depth - 1, tBoard );
            nodes += moveNodes;

            LOG_DEBUG << ( *it ).toString() << " : " << moveNodes << " " << tBoard.toFENString();
        }
        else
        {
            nodes += perftImpl( depth - 1, tBoard );
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
            std::vector<Move> candidateMoves = board->getMoves();
            if ( candidateMoves.empty() )
            {
                LOG_DEBUG << "No candidate moves";
                break;
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