#include "Logger.h"

std::ostringstream& Logger::log( const std::source_location location )
{
    // only generate and insert the extra context if we're going to use it
    if ( level >= selectedLevel )
    {
        os << getTimestamp() << " " << getLevelName();

        os << " " << std::setw( 30 ) << std::left << location.function_name() << " : " << std::setw( 5 ) << std::right << location.line() << " : ";
    }

    return os;
}
