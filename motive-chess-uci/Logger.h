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
#define LOG_TRACE Logger( Logger::Level::TRACE ).log()
#define LOG_DEBUG Logger( Logger::Level::DEBUG ).log()
#define LOG_INFO Logger( Logger::Level::INFO ).log()
#define LOG_WARN Logger( Logger::Level::WARN ).log()
#define LOG_ERROR Logger( Logger::Level::ERROR ).log()

class Logger
{
public:
    enum class Level
    {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        NONE
    };

private:
    inline static Logger::Level selectedLevel = Logger::Level::INFO;

    Logger::Level level;

    std::ostream& stream;

    std::ostringstream os;

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

    const char* getLevelName()
    {
        switch ( level )
        {
            case Logger::Level::TRACE:
                return "TRACE";

            case Logger::Level::DEBUG:
                return "DEBUG";

            case Logger::Level::INFO:
                return "INFO ";

            case Logger::Level::WARN:
                return "WARN ";

            case Logger::Level::ERROR:
                return "ERROR";

            default:
            case Logger::Level::NONE:
                return "     ";
        };
    }

public:
    static void configure( Logger::Level level )
    {
        Logger::selectedLevel = level;
    }

    Logger( Logger::Level level ) :
        stream( std::cerr ),
        level( level )
    {
        // Nothing to do
    }

    virtual ~Logger()
    {
        if ( level >= selectedLevel )
        {
            os << std::endl;
            stream << os.str() << std::flush;
        }
    }

    std::ostringstream& log( const std::source_location location = std::source_location::current() )
    {
        // only generate and insert the extra context if we're going to use it
        if ( level >= selectedLevel )
        {
            os << getTimestamp() << " " << getLevelName() << " " << std::setw( 20 ) << location.function_name() << " (" << location.line() << ") ";
        }

        return os;
    }
};

