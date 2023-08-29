#include "Log.h"

#include <chrono>
#include <ctime>
#include <mutex>

std::mutex consoleLogMutex;
thread_local std::stringstream perThreadBuffer;

// Log::Destination

const char* Log::Destination::levelName( Log::Level level )
{
    switch ( level )
    {
        case Log::Level::TRACE:
            return "TRACE";

        case Log::Level::DEBUG:
            return "DEBUG";

        case Log::Level::INFO:
            return "INFO ";

        case Log::Level::WARN:
            return "WARN ";

        case Log::Level::ERROR:
            return "ERROR";

        default:
        case Log::Level::NONE:
            return "     ";
    }
}

std::string Log::Destination::timestamp()
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

// Log::Logger

void Log::Logger::operator()(const char* format, ...) const
{
    if ( level >= Log::getDestination()->getLevel() )
    {
        // Hazard a guess for a buffer size
        const size_t initialBuffer = 256;
        char* buffer = new char[initialBuffer];

        // Attach the varargs
        va_list args;
        va_start( args, format );

        // Print the string, but don't overflow the buffer and let it tell us if we need 
        // a larger buffer
        size_t size = vsnprintf(buffer, initialBuffer - 1, format, args);
        if ( size > initialBuffer )
        {
            // Larger buffer required
            delete[] buffer;
            buffer = new char[ size + 1 ];

            // Try again
            vsnprintf( buffer, size + 1, format, args );
        }

        // Output the log
        Log::getDestination()->write( level, buffer );

        // Housekeeping
        va_end( args );
        delete[] buffer;
    }
}

