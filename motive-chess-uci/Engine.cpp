#include "Engine.h"

void Engine::uci()
{
    debug( "Received uci" );

    // Not expecting a 'uci' event after having quit, but the protocol doesn't give us a way to decline
    quitting = false;

    initialized = true;

    // TODO do any actual initialization/reset here

    broadcaster.id( "MotiveChess", "Motivesoft");
    // TODO broadcast options, ...

    broadcaster.uciok();

    // TODO Shredder does registration here
}

void Engine::debug( std::vector<std::string>& arguments )
{
    debug( "Received debug" );

    // Unchanged if no args or unrecognised first arg
    if ( arguments.size() > 0 )
    {
        std::string argument = arguments[ 0 ];
        if ( argument == "on" )
        {
            debugging = true;
            debug( "debug on" );
        }
        else if ( argument == "off" )
        {
            debugging = false;
            debug( "debug off" );
        }
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

void Engine::stopImpl()
{

}

void Engine::isreadyImpl()
{

}
