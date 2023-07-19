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

    void id( std::string name, std::string author )
    {
        stream << "id name " << name << std::endl;
        stream << "id author " << name << std::endl;
    }
};

