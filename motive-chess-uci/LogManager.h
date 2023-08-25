#pragma once

#include <cstdio>
#include <fstream>
#include <iostream>

class LogManager
{
public:
    enum class Level
    {
        TRACE, DEBUG, INFO, WARN, ERROR, NONE
    };

    class Logger
    {
    private:
        LogManager::Level level;

    protected:
        Logger( LogManager::Level level ) :
            level( level )
        {
            // Do nothing
        }

        virtual void logImpl( LogManager::Level level, const char* message ) = 0;

    public:
        static LogManager::Logger* getLogger()
        {
            return logger;
        }

        static void setLogger( LogManager::Logger* logger )
        {
            if ( LogManager::logger != nullptr )
            {
                // (temporarily) close the logger to allow the previous default to be destroyed
                close();
            }

            // TODO mutex protect access to logger
            LogManager::logger = logger;
        }

        static void close()
        {
            if ( LogManager::logger != nullptr )
            {
                delete LogManager::logger;
                LogManager::logger = nullptr;
            }
        }

        inline void setLevel( LogManager::Level level )
        {
            this->level = level;
        }

        inline LogManager::Level getLevel()
        {
            return this->level;
        }

        void log( LogManager::Level level, const char* message )
        {
            if ( level >= getLevel() )
            {
                logImpl( level, message );
            }
        }
    };

private:
    inline static LogManager::Logger* logger = nullptr;
};

/// <summary>
/// A logger that writes to the console
/// </summary>
class ConsoleLogger : public LogManager::Logger
{
public:
    ConsoleLogger( LogManager::Level level = LogManager::Level::INFO ) :
        Logger( level )
    {
        // Do nothing
    }

    inline void logImpl( LogManager::Level level, const char* message );
};

/// <summary>
/// A logger that writes to a file
/// </summary>
class FileLogger : public LogManager::Logger
{
private:
    std::ofstream stream;

public:
    FileLogger( const char* filename, LogManager::Level level = LogManager::Level::INFO ) :
        Logger( level )
    {
        stream.open( filename, std::ios::out );
    }

    virtual ~FileLogger()
    {
        stream.flush();
        stream.close();
    }

    inline void logImpl( LogManager::Level level, const char* message );
};

/// <summary>
/// A logger that does nothing
/// </summary>
class NullLogger : LogManager::Logger
{
public:
    NullLogger() :
        Logger( LogManager::Level::NONE )
    {
        // Do nothing
    }

    inline void setLevel( LogManager::Level level )
    {
        // Do nothing
    }

    inline void logImpl( LogManager::Level level, const char* message )
    {
        // Do nothing
    }
};
