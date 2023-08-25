#include "LogManager.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>

// LoggerBase
std::string LogManager::LoggerBase::getTimestamp()
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

const char* LogManager::LoggerBase::LevelName( LogManager::Level level ) const
{
    switch ( level )
    {
        case LogManager::Level::TRACE:
            return "TRACE";

        case LogManager::Level::DEBUG:
            return "DEBUG";

        case LogManager::Level::INFO:
            return "INFO ";

        case LogManager::Level::WARN:
            return "WARN ";

        case LogManager::Level::ERROR:
            return "ERROR";

        default:
        case LogManager::Level::NONE:
            return "     ";

    }
}

// ConsoleLogger

void ConsoleLogger::write( LogManager::Level level, const char* message )
{
    std::cout << getTimestamp() << " " << LevelName(level) << " " << message << std::endl;
}

// FileLogger

void FileLogger::write( LogManager::Level level, const char* message )
{
    stream << getTimestamp() << " " << LevelName( level ) << " " << message << std::endl;
}
