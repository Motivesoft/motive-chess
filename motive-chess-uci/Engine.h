#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "Broadcaster.h"
#include "CopyProtection.h"
#include "Registration.h"

class Engine
{
public:
    enum class DebugSwitch
    {
        OFF,
        ON
    };

private:
    Broadcaster& broadcaster;

    CopyProtection copyProtection;
    Registration registration;

    bool initialized;

    volatile bool quitting;
    volatile DebugSwitch debugging;

    void debug( std::string message )
    {
        if ( debugging == DebugSwitch::ON )
        {
            broadcaster.info( message );
        }
    }

    void debug( std::string message, std::string value )
    {
        if ( debugging == DebugSwitch::ON )
        {
            debug( message + " " + value );
        }
    }

    void error( std::string message )
    {
        std::cerr << "Error: " << message << std::endl;
    }

    void error( std::string message, std::string value )
    {
        error( message + " " + value );
    }

    // Implementation methods that do not broadcast notifications 
    void stopImpl();
    void isreadyImpl();
    void debugImpl( DebugSwitch flag );
    void registerImpl();
    void registerImpl( std::string& name, std::string& code );
    void setoptionImpl( std::string& name, std::string& value );

public:
    Engine( Broadcaster& broadcaster ) : 
        broadcaster( broadcaster ), 
        initialized( false ),
        quitting( false ),
        debugging( DebugSwitch::OFF )
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
    void setoption( std::vector<std::string>& arguments );
    void registerX( std::vector<std::string>& arguments ); // Can't call a method 'register'
    void ucinewgame();
    void position( std::vector<std::string>& arguments );
    void go( std::vector<std::string>& arguments );
    void stop();
    void ponderhit();
    bool quit();
};

