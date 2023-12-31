#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <thread>
#include <vector>

#include "Bitboard.h"
#include "Board.h"
#include "Engine.h"
#include "Evaluation.h"
#include "GameContext.h"
#include "Log.h"
#include "Move.h"
#include "Utilities.h"

#define UCI_DEBUG Engine::UciLogger( *this, Log::Level::DEBUG ).log( "" )
#define UCI_INFO  Engine::UciLogger( *this, Log::Level::INFO ).log( "" )
#define UCI_WARN  Engine::UciLogger( *this, Log::Level::WARN ).log( "Warning - " )
#define UCI_ERROR Engine::UciLogger( *this, Log::Level::ERROR ).log( "Error - " )

void Engine::uciCommand()
{
    UCI_DEBUG << "Starting engine";

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

        Log::Trace << "Stopping" << std::endl;
        stopImpl();

        Log::Trace << "Releasing game context" << std::endl;
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
        Log::Trace << "Preparing for new game" << std::endl;
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
            Log::Warn << "Unknown or unexpected position argument " << *it << std::endl;
            break;
        }
    }

    if ( !fen.empty() )
    {
        Log::Debug << "Position with FEN [" << fen << "] and " << moves.size() << " moves" << std::endl;

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

    std::vector<Move> searchMoves;
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

            // Capture the next few strings as moves unless they are other 'go' keywords
            for ( ; ++it != arguments.end(); )
            {
                if ( std::find( directives.begin(), directives.end(), *it ) != directives.end() )
                {
                    // Recognised keyword, break out of this loop and process below
                    break;
                }

                searchMoves.push_back( Move::fromString( *it ) );
            }
        }
        
        // Don't 'else' this with searchmoves as it may have moved the iterator along to one of the following
        if ( *it == "ponder" )
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
                Log::Error << "Missing filename" << std::endl;
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
                    Log::Error << "Empty filename" << std::endl;
                    return;
                }

                Log::Info << "Starting perft run from file " << filename << std::endl;

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

                            Log::Trace << "Noting expected result for depth " << expectedDepth << " of " << expectedCount << std::endl;
                            expectedResults.push_back( std::pair<unsigned int, unsigned int>( expectedDepth, expectedCount ) );
                        }
                        else
                        {
                            Log::Error << "Unexpected result " << *it << std::endl;
                        }
                    }
                    else
                    {
                        Log::Error << "Unexpected value " << *it << std::endl;
                    }
                    break;

                case ',': // Expected structure: "<FEN>,20,400,8902,..."
                    expectedCount = stoi( ( *it ).substr( 1 ) );
                    expectedDepth++;

                    Log::Trace << "Noting expected result for depth " << expectedDepth << " of " << expectedCount << std::endl;
                    expectedResults.push_back( std::pair<unsigned int, unsigned int>( expectedDepth, expectedCount ) );
                    break;

                default:
                    Log::Error << "Unexpected argument" << *it << std::endl;
                    break;
            }
        }

        if ( fenString.empty() )
        {
            Log::Debug << "No FEN string specified; using default" << std::endl;

            fenString = Fen::startingPosition;
        }

        Fen fen = Fen::fromPosition( fenString );
        Board board( fen );


        if ( expectedResults.empty() )
        {
            Log::Info << "Starting perft run at depth " << depth << " with " << fenString << std::endl;
            
            perftDepth( board, depth );
        }
        else
        {
            Log::Info << "Starting perft run with " << fenString << std::endl;

            Log::Trace( [&] ( const Log::Logger& logger )
            {
                logger << "Expected results:" << std::endl;
                for ( std::vector<std::pair<unsigned int, unsigned int>>::iterator it = expectedResults.begin(); it != expectedResults.end(); it++ )
                {
                    logger << "  Depth " << ( *it ).first << ". Count " << ( *it ).second << std::endl;
                }
            } );

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
    clock_t start = clock();
    unsigned long nodes = perftImpl( depth, board, true );
    clock_t end = clock();

    float elapsed = static_cast<float>( end - start ) / CLOCKS_PER_SEC;
    float nps = elapsed == 0 ? 0 : static_cast<float>( nodes ) / elapsed;

    // This will give 0 if elapsed is close to zero - but not sure what to do with that other than continue
    unsigned long longNPS = std::lround( nps );

    Log::Info << "Total node count at depth " << depth << " is " << nodes << ". Time " << elapsed << "s (" << longNPS << " nps)" << std::endl;
}

void Engine::perftRange( Board& board, std::vector<std::pair<unsigned int, unsigned int>> expectedResults )
{
    for ( std::vector<std::pair<unsigned int, unsigned int>>::iterator it = expectedResults.begin(); it != expectedResults.end(); it++ )
    {
        unsigned int depth = ( *it ).first;
        unsigned int count = ( *it ).second;

        clock_t start = clock();
        unsigned long nodes = perftImpl( depth, board, true );
        clock_t end = clock();

        float elapsed = static_cast<float>( end - start ) / CLOCKS_PER_SEC;
        float nps = elapsed == 0 ? 0 : static_cast<float>( nodes ) / elapsed;

        // This will give 0 if elapsed is close to zero - but not sure what to do with that other than continue
        unsigned long longNPS = std::lround( nps );

        if ( nodes == count )
        {
            Log::Info << "Total node count at depth " << depth << " is " << nodes << ". Time " << elapsed << "s (" << longNPS << " nps)" << std::endl;
        }
        else
        {
            Log::Error << "Total node count at depth " << depth << " is " << nodes << " but expected to be " << count << ". Time " << elapsed << "s (" << longNPS << " nps)" << std::endl;
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
                Log::Trace << "Skipping empty or comment line" << std::endl;
                continue;
            }

            Log::Trace << "Read: " << line << std::endl;

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
        Log::Error << "Failed to read file " << filename << std::endl;
    }
}

void Engine::listVisibleOptions()
{
    broadcaster.option( OPTION_BENCH, benchmarking );
}
 
// Silent implementations - do the work, but do not directly communicate over uci, allowing the 
// methods to be called from elsewhere

void Engine::initializeImpl()
{
    Bitboard::initialize();
    initialized = true;
}

void Engine::stopImpl( ThinkingOutcome thinkingOutcome )
{
    if ( thinkingThread == nullptr )
    {
        return;
    }

    Log::Trace << "Stopping thinking" << std::endl;

    broadcastThinkingOutcome = ( thinkingOutcome == ThinkingOutcome::BROADCAST );
    
    // Set this volatile switch and it will be detected by a thinking thread if one is active
    continueThinking = false;

    // Wait for the thread to stop
    thinkingThread->join();

    Log::Trace << "Thread stopped" << std::endl;

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
    Log::Info << "Setting debug to [" << (flag == DebugSwitch::ON ? "on" : "off") << "]" << std::endl;

    debugging = flag;
}

void Engine::registerImpl()
{
    Log::Info << "Register later" << std::endl;

    broadcaster.registration( Registration::Status::CHECKING );

    bool registered = registration.registerLater();

    broadcaster.registration( registered ? Registration::Status::OK : Registration::Status::ERROR );
}

void Engine::registerImpl( std::string& name, std::string& code )
{
    Log::Info << "Register with name [" << name << "] and code [" << code << "]" << std::endl;

    broadcaster.registration( Registration::Status::CHECKING );

    bool registered = registration.registerNameCode( name, code );
    
    broadcaster.registration( registered ? Registration::Status::OK : Registration::Status::ERROR );
}

void Engine::setoptionImpl( std::string& name, std::string& value )
{
    Log::Info << "SetOption with name [" << name << "] and value [" << value << "]" << std::endl;

    // Store the new value
    if ( name == OPTION_BENCH )
    {
        setBenchmarking( value == "true" );
    }
}

void Engine::positionImpl( const std::string& fenString, std::vector<std::string> moves )
{
    Log::Info << "Processing FEN string " << fenString << " and " << moves.size() << " moves" << std::endl;
    Fen fen = Fen::fromPosition( fenString );
    
    Log::Debug << "Position:" << std::endl;
    fen.dumpBoard();

    std::vector< Move > moveList;
    if ( moves.size() > 0 )
    {
        Log::Debug << "Initial moves:" << std::endl;

        for ( std::string move : moves )
        {
            Move m = Move::fromString( move );

            moveList.push_back( m );
            Log::Debug << "  " << m.toString() << std::endl;
        }
    }

    // Set the new game context
    gameContext = new GameContext( fen, moveList );

    // Reset for next game
    ucinewgameReceived = false;
}

void Engine::goImpl( GoContext* goContext )
{
    Log::Info << "Go: depth=" << goContext->getDepth() << std::endl;

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

    Log::Trace << "Thread " << thinkingThread->get_id() << " running" << std::endl;
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
        Move& move = *it;
        Board tBoard = board.makeMove( move );

        if ( divide )
        {
            unsigned long moveNodes = perftImpl( depth - 1, tBoard );
            nodes += moveNodes;

            Log::Debug << move.toString() << " : " << moveNodes << " " << tBoard.toFENString() << std::endl;
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

    unsigned short depth = context->getDepth();

    // OK, beging the thinking process - we have to test flags, but make sure we have a candidate move
    // before being interrupted - unless we are quitting

    Thoughts thoughts;

    // TODO This is debug code. Remove when we're happy to lose it
    Log::Debug << "Current position scoring: " << Evaluation::scorePosition( *board, board->getActiveColor() ) << std::endl;

    unsigned int loop = 0;
    //while ( engine->continueThinking && !engine->quitting )
    while ( !engine->quitting && (engine->continueThinking || thoughts.getBestMove().isNullMove() ))
    {
        do
        {
            std::vector<Move> candidateMoves = board->getMoves();

            // Filter the moves down to the requested 'searchmoves' subset, if there is one
            if ( !context->getSearchMoves().empty() )
            {
                for ( std::vector<Move>::iterator it = candidateMoves.begin(); it != candidateMoves.end(); )
                {
                    if( std::find( context->getSearchMoves().begin(), context->getSearchMoves().end(), *it ) == context->getSearchMoves().end() )
                    {
                        it = candidateMoves.erase( it );
                    }
                    else
                    {
                        it++;
                    }
                }
            }

            if ( candidateMoves.empty() )
            {
                // TODO we need to decide what to do here. Return nullmove? something based on win/loss/draw?
                Log::Info << "No candidate moves" << ( context->getSearchMoves().empty() ? "" : " match with searchmove list" ) << std::endl;
                engine->continueThinking = false;
                break;
            }

            if ( candidateMoves.size() == 1 || depth == 0 )
            {
                // Don't waste clock time analysing a forced move situation
                thoughts = Thoughts( candidateMoves[ 0 ] );

                Log::Debug << "Only one move available" << std::endl;

                engine->continueThinking = false;
                break;
            }

            // Sort moves - this is a bit coarse grained
            std::sort( candidateMoves.begin(), candidateMoves.end(), [&] (Move a, Move b)
            {
                if ( a.isCapture() != b.isCapture() ) // includes en passant
                {
                    return a.isCapture();
                }
                if ( a.isPromotion() != b.isPromotion() )
                {
                    return a.isPromotion();
                }
                if ( a.isCastling() != b.isCastling() )
                {
                    return a.isCastling();
                }
                return false;
            } );

            // Start of minmax/alphabeta/negamax/whatever
            // For each move at this level, use the recursive algorithm to arrive at a score and then go with the best

            Move bestMove = Move::nullMove;
            short bestScore = std::numeric_limits<short>::lowest();
            for ( std::vector<Move>::const_iterator it = candidateMoves.cbegin(); it != candidateMoves.cend(); it++ )
            {
                Log::Debug( [&] ( const Log::Logger& logger) 
                {
                    logger << "Considering " << ( *it ).toString() << std::endl;
                } ); 

                short score = Evaluation::minimax( board->makeMove( *it ),
                                                   depth,
                                                   std::numeric_limits<short>::lowest(),
                                                   std::numeric_limits<short>::max(),
                                                   false, 
                                                   board->getActiveColor() );

                if ( score > bestScore )
                {
                    bestScore = score;
                    bestMove = *it;
                }

                Log::Debug( [&] ( const Log::Logger& logger )
                {
                    logger << "--Score for " << ( *it ).toString() << " is " << score << std::endl;
                } ); 
            }

            // If we haven't got a move in mind, establish one
            if ( thoughts.getBestMove().isNullMove() )
            {
                thoughts = Thoughts( bestMove );

                engine->continueThinking = false;
            }

            // TODO this probably wants to be a better check
            if ( --depth == 0 )
            {
                Log::Debug << "Reached search depth" << std::endl;
                engine->continueThinking = false;
                break;
            }
        }
        while ( engine->continueThinking && !engine->quitting );
    }

    if ( engine->broadcastThinkingOutcome )
    {
        if ( thoughts.getPonderMove().isNullMove() )
        {
            Log::Info << "Broadcasting best move: " << thoughts.getBestMove().toString() << std::endl;

            engine->broadcaster.bestmove( thoughts.getBestMove().toString() );
        }
        else
        {
            Log::Info << "Broadcasting best move: " << thoughts.getBestMove().toString() << " with ponder: " << thoughts.getPonderMove().toString() << std::endl;

            engine->broadcaster.bestmove( thoughts.getBestMove().toString(), thoughts.getPonderMove().toString() );
        }
    }

    Log::Debug << "Thinking thread terminating" << std::endl;
    delete context;
}