#include "Log.h"

#include <mutex>

std::mutex consoleLogMutex;
thread_local std::stringstream perThreadBuffer;

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
