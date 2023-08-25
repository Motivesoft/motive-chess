#include "LogManager.h"

#include <iostream>

// ConsoleLogger

void ConsoleLogger::logImpl( LogManager::Level level, const char* message )
{
    std::cout << message << std::endl;
}

// FileLogger

void FileLogger::logImpl( LogManager::Level level, const char* message )
{
    stream << message << std::endl;
}
