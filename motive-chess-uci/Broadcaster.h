#pragma once

#include <iostream>

#include "CopyProtection.h"

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
        stream << "id author " << author << std::endl;
    }

    void bestmove( std::string bestmove )
    {
        stream << "bestmove " << bestmove << std::endl;
    }

    void bestmove( std::string bestmove, std::string ponder )
    {
        stream << "bestmove " << bestmove << " ponder " << ponder << std::endl;
    }

    void copyprotection( CopyProtection::Status status )
    {
        switch ( status )
        {
            case CopyProtection::CHECKING:
                stream << "copyprotection checking" << std::endl;
                break;
            case CopyProtection::OK:
                stream << "copyprotection ok" << std::endl;
                break;
            case CopyProtection::ERROR:
                stream << "copyprotection error" << std::endl;
                break;
            default:
                break;
        }
    }
};

