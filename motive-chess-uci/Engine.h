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

    bool ucinewgameExpected;
    bool ucinewgameReceived;

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
        debugging( DebugSwitch::OFF ),
        ucinewgameExpected( true ),
        ucinewgameReceived( false )
    {
        broadcaster.info( "MotiveChess" );
    }

    virtual ~Engine()
    {
        stopCommand();
        initialized = false;
    }

    void uciCommand();
    void debugCommand( std::vector<std::string>& arguments );
    void isreadyCommand();
    void setoptionCommand( std::vector<std::string>& arguments );
    void registerCommand( std::vector<std::string>& arguments );
    void ucinewgameCommand();
    void positionCommand( std::vector<std::string>& arguments );
    void goCommand( std::vector<std::string>& arguments );
    void stopCommand();
    void ponderhitCommand();
    bool quitCommand();
};

