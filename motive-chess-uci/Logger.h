#pragma once

#include <iostream>
#include <source_location>
#include <string>

class Logger
{
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
};

