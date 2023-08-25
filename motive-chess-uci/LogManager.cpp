#include "LogManager.h"

#include <iostream>

// ConsoleLogger

void ConsoleLogger::write( LogManager::Level level, const char* message )
{
    std::cout << message << std::endl;
}

// FileLogger

void FileLogger::write( LogManager::Level level, const char* message )
{
    stream << message << std::endl;
}
