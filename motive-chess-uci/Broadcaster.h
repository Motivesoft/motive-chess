#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include "CopyProtection.h"
#include "Option.h"
#include "Registration.h"

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
            case CopyProtection::Status::CHECKING:
                stream << "copyprotection checking" << std::endl;
                break;
            case CopyProtection::Status::OK:
                stream << "copyprotection ok" << std::endl;
                break;
            case CopyProtection::Status::ERROR:
                stream << "copyprotection error" << std::endl;
                break;
            default:
                break;
        }
    }

    void registration( Registration::Status status )
    {
        switch ( status )
        {
            case Registration::Status::CHECKING:
                stream << "registration checking" << std::endl;
                break;
            case Registration::Status::OK:
                stream << "registration ok" << std::endl;
                break;
            case Registration::Status::ERROR:
                stream << "registration error" << std::endl;
                break;
            default:
                break;
        }
    }

    void info( std::vector<std::pair<std::string,std::string>> values )
    {
        std::stringstream details;
        bool first = true;
        for ( std::pair<std::string, std::string> value : values )
        {
            if ( first )
            {
                first = false;
            }
            else
            {
                details << " ";
            }

            details << value.first << " " << value.second;
        }

        stream << "info " << details.str() << std::endl;
    }

    void option( std::string& name, Option::Type type, std::string def, std::string min, std::string max, std::string vars[] )
    {
        std::stringstream details;

        switch ( type )
        {
            case Option::Type::CHECK:
                details << "check default " << def;
                break;

            case Option::Type::SPIN:
                details << "spin default " << def << " min " << min << " max " << max;
                break;

            case Option::Type::COMBO:
                details << "combo default " << def;
                for ( int loop = 0; loop < sizeof( vars ); loop++ )
                {
                    details << " var " << vars[ loop ];
                }
                break;

            case Option::Type::BUTTON:
                details << "button";
                break;

            case Option::Type::STRING:
                details << "string default " << def;
                break;

            default:
                // TODO what's the best thing here?
                break;
        }

        stream << "option name " << name << " type " << details.str() << std::endl;
    }

    // Helper functions

    void info( std::string message )
    {
        info( std::pair<std::string, std::string>( "string", message ) );
    }

    void info( std::pair<std::string, std::string> value )
    {
        std::vector< std::pair<std::string, std::string>> values;
        values.push_back( value );
        info( values );
    }
};

