#include "Engine.h"

void Engine::uciCommand()
{
    debug( "Received uci" );

    // TODO do any actual initialization/reset here

    // TODO Pull this from resources and maybe add major version
    broadcaster.id( "MotiveChess", "Motivesoft" );
    
    // TODO broadcast options,...

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
    debug( "Received debug" );

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
            error( "Unknown debug argument", arguments[ 0 ] );
        }
    }
    else
    {
        error( "Missing debug argument" );
    }
}

void Engine::isreadyCommand()
{
    debug( "Received isready" );

    // TODO make sure we are actually ready, once there is something running here
    isreadyImpl();

    broadcaster.readyok();
}

void Engine::setoptionCommand( std::vector<std::string>& arguments )
{
    debug( "Received setoption" );

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
            error( "Unexpected setoption argument", *it );
        }
    }

    if ( isName || isValue )
    {
        setoptionImpl( name, value );
    }
}

void Engine::registerCommand( std::vector<std::string>& arguments )
{
    debug( "Received register" );

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
            error( "Unexpected register argument", *it );
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
    debug( "Received ucinewgame" );

    if ( ucinewgameExpected )
    {
        ucinewgameReceived = true;

        // TODO implement
    }
    else
    {
        error( "ucinewgame received out of sequence. Ignoring" );
    }
}

void Engine::positionCommand( std::vector<std::string>& arguments )
{
    debug( "Received position" );

    if ( !ucinewgameReceived )
    {
        ucinewgameExpected = false;
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
    }

    if ( !fen.empty() )
    {
        // TODO implement
    }
    else
    {
        error( "Illegal position command" );
    }
}

void Engine::goCommand( std::vector<std::string>& arguments )
{
    debug( "Received go" );

    // TODO implement
}

void Engine::stopCommand()
{
    debug( "Received stop" );

    stopImpl();

    // TODO do something now we've stopped - bestmove and possibly ponder
    if ( !quitting )
    {

    }
}

void Engine::ponderhitCommand()
{
    debug( "Received ponderhit" );

    // TODO implement
}

bool Engine::quitCommand()
{
    debug( "Received quit" );

    quitting = true;

    stopImpl();
    isreadyImpl();

    return true;
}

// Silent implementations - do the work, but do not directly communicate over uci, allowing the 
// methods to be called from elsewhere

void Engine::stopImpl()
{

}

void Engine::isreadyImpl()
{

}

void Engine::debugImpl( Engine::DebugSwitch flag )
{
    debug( "Setting debug to " + std::string( flag == DebugSwitch::ON ? "on" : "off" ) );

    debugging = flag;
}

void Engine::registerImpl()
{
    debug( "Register later" );

    broadcaster.registration( Registration::Status::CHECKING );

    bool registered = registration.registerLater();

    broadcaster.registration( registered ? Registration::Status::OK : Registration::Status::ERROR );
}

void Engine::registerImpl( std::string& name, std::string& code )
{
    debug( "Register with name [" + name + "] and code [" + code + "]" );

    broadcaster.registration( Registration::Status::CHECKING );

    bool registered = registration.registerNameCode( name, code );
    
    broadcaster.registration( registered ? Registration::Status::OK : Registration::Status::ERROR );
}

void Engine::setoptionImpl( std::string& name, std::string& value )
{
    debug( "SetOption with name [" + name + "] and value [" + value + "]" );

    // TODO store the value
}
