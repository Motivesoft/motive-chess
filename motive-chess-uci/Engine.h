#pragma once

#include <iostream>
#include <string>

class Engine
{
private:
    bool initialized;

    void broadcast( std::string output )
    {
        std::cout << output << std::endl;
    }

public:
    Engine()
    {
        broadcast( "info string MotiveChess" );
    }

    virtual ~Engine()
    {
        stop();
        initialized = false;
    }

    void initialize()
    {
        initialized = true;

        broadcast( "uciok" );
    }

    void stop();
};

