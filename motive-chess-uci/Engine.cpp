#include "Engine.h"

void Engine::uci()
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

void Engine::debug( std::vector<std::string>& arguments )
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

void Engine::isready()
{
    debug( "Received isready" );

    // TODO make sure we are actually ready, once there is something running here
    isreadyImpl();

    broadcaster.readyok();
}

void Engine::stop()
{
    debug( "Received stop" );

    stopImpl();

    // TODO do something now we've stopped
    if ( !quitting )
    {

    }
}

bool Engine::quit()
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