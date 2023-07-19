#include "Engine.h"

void Engine::uci()
{
    debug( "Received uci" );

    initialized = true;

    broadcaster.uciok();
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

    broadcaster.readyok();
}

void Engine::stop()
{
    debug( "Received stop" );
}