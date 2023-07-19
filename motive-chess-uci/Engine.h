#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "Broadcaster.h"

class Engine
{
private:
    Broadcaster& broadcaster;
    CopyProtection copyProtection;

    bool initialized;

    volatile bool quitting;
    volatile bool debugging;

    void debug( std::string message )
    {
        if ( debugging )
        {
            broadcaster.info( message );
        }
    }

    void stopImpl();
    void isreadyImpl();

public:
    Engine( Broadcaster& broadcaster ) : 
        broadcaster( broadcaster ), 
        initialized( false ),
        quitting( false ),
        debugging( false )
    {
        broadcaster.info( "MotiveChess" );
    }

    virtual ~Engine()
    {
        stop();
        initialized = false;
    }

    void uci();
    void debug( std::vector<std::string>& arguments );
    void isready();
    void stop();
    bool quit();
};

