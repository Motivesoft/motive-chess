#include "LogManager.h"

#include <iostream>

// ConsoleLogger

void ConsoleLogger::write( LogManager::Level level, const char* message )
{
    std::cout << LevelName( level ) << " " << message << std::endl;
}

// FileLogger

void FileLogger::write( LogManager::Level level, const char* message )
{
    stream << LevelName( level ) << " " << message << std::endl;
}
