#pragma once

#include <iostream>

class Broadcaster
{
private:
    std::ostream& stream;

public:
    Broadcaster( std::ostream& stream ) : 
        stream( stream )
    {
    }

    virtual ~Broadcaster()
    {
        // Do nothing
    }

    void info( std::string message )
    {
        stream << "info string " << message << std::endl;
    }

    void uciok()
    {
        stream << "uciok" << std::endl;
    }

    void readyok()
    {
        stream << "readyok" << std::endl;
    }
};

