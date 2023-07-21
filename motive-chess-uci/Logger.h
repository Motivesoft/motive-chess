#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <source_location>
#include <sstream>
#include <string>
#include <time.h>

// Helper macros
#define LOG_DEBUG Logger().debug()

class Logger
{
private:
    std::ostringstream os;

    bool enabled;

private:
    std::ostringstream& log( const std::source_location location, std::string level )
    {
        // only generate and insert the extra context if we're going to use it
        if ( enabled )
        {
            os << getTimestamp() << " " << level << " ";
        }
        return os;
    }

    std::string getTimestamp()
    {
        std::ostringstream timestamp;

        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        auto ms = duration_cast<std::chrono::milliseconds>( now.time_since_epoch() ) % 1000;


        // convert to std::time_t in order to convert to std::tm (broken time)
        time_t timer = std::chrono::system_clock::to_time_t( now );

        // convert to broken time
        struct tm newtime;
        localtime_s( &newtime, &timer );

        std::ostringstream oss;

        timestamp << std::put_time( &newtime, "%H:%M:%S" ); // HH:MM:SS
        timestamp << '.' << std::setfill( '0' ) << std::setw( 3 ) << ms.count();

        return timestamp.str();
    }

public:
    Logger() : enabled( true )
    {
        // Nothing to do
    }

    virtual ~Logger()
    {
        if ( enabled )
        {
            os << std::endl;
            std::cerr << os.str() << std::flush;
        }
    }

    std::ostringstream& debug( const std::source_location location = std::source_location::current() )
    {
        return log( location, "DEBUG" );
    }




/*
private:
    inline static bool enabled = true;

    inline static std::ostream& stream = std::cerr;

public:
    static void trace( std::string message, const std::source_location location = std::source_location::current() )
    {
        if ( enabled )
        {
            stream << location.file_name() << " " << location.function_name() << " " << location.line() << " " << message << std::endl << std::flush;
        }
    }
    */
};

