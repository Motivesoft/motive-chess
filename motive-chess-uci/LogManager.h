#pragma once

#include <fstream>
#include <functional>
#include <ostream>
#include <sstream>
#include <string>

class LogManager
{
public:
    enum class Level
    {
        TRACE, DEBUG, INFO, WARN, ERROR, NONE
    };

    inline const static LogManager::Level DefaultLevel = LogManager::Level::INFO;

    /// <summary>
    /// Abstract base class for all loggers
    /// </summary>
    class LoggerBase
    {
    protected:
        LoggerBase()
        {
            // Do nothing
        }

        const char* LevelName( LogManager::Level level ) const
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

        std::string getTimestamp();

    public:
        // TODO consider renaming this to 'log'
        virtual void write( LogManager::Level level, const char* message ) = 0;
    };

    class LevelLogger
    {
    private:
        LogManager::Level level;
        LogManager::LoggerBase& logger;

        std::ostringstream* currentStream = nullptr;

        void closeStream()
        {
            if ( currentStream != nullptr )
            {
                logger.write( level, currentStream->str().c_str() );
            }
        }

    public:
        LevelLogger( LogManager::Level level, LogManager::LoggerBase& logger ) :
            level( level ),
            logger( logger )
        {

        }

        virtual ~LevelLogger()
        {
            closeStream();
        }

        inline std::ostringstream& stream()
        {
            if ( currentStream == nullptr )
            {
                currentStream = new std::ostringstream();
            }

            return *currentStream;
        }

        inline void write( const char* message )
        {
            logger.write( level, message );
        }
    };

    class Logger : public LogManager::LoggerBase
    {
    private:
        LogManager::Level level;

    protected:
        Logger( LogManager::Level level ) :
            level( level )
        {
            // Do nothing
        }

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

        void log( LogManager::Level level, std::string& message )
        {
            if ( level >= getLevel() )
            {
                write( level, message.c_str() );
            }
        }

        void log( LogManager::Level level, const char* message )
        {
            if ( level >= getLevel() )
            {
                write( level, message );
            }
        }

        inline void log( LogManager::Level level, const std::function <void( LogManager::LevelLogger& logger )>& log_callback )
        {
            if ( getLevel() <= level )
            {
                LevelLogger logger( level, *this );
                log_callback( logger );
            }
        }
    };

    static void setLogger( Logger* logger )
    {
        if ( LogManager::logger != nullptr )
        {
            delete LogManager::logger;
        }

        LogManager::logger = logger;
    }

    static Logger* getLogger()
    {
        return LogManager::logger;
    }

    static LogManager::Level getLevel()
    {
        if ( logger == nullptr )
        {
            return DefaultLevel;
        }

        return logger->getLevel();
    }

private:
    inline static LogManager::Logger* logger = nullptr;
};

/// <summary>
/// A logger that writes to the console
/// </summary>
class ConsoleLogger : public LogManager::Logger
{
public:
    ConsoleLogger( LogManager::Level level = LogManager::DefaultLevel ) :
        Logger( level )
    {
        // Do nothing
    }

    void write( LogManager::Level level, const char* message );
};

/// <summary>
/// A logger that writes to a file
/// </summary>
class FileLogger : public LogManager::Logger
{
private:
    std::ofstream stream;

public:
    FileLogger( const char* filename, LogManager::Level level = LogManager::DefaultLevel ) :
        Logger( level )
    {
        stream.open( filename, std::ios::out );
    }

    FileLogger( std::string& filename, LogManager::Level level = LogManager::DefaultLevel ) :
        FileLogger( filename.c_str(), level )
    {
        // Nothing more to do
    }

    virtual ~FileLogger()
    {
        stream.flush();
        stream.close();
    }

    void write( LogManager::Level level, const char* message );
};

/// <summary>
/// A logger that does nothing
/// </summary>
class NullLogger : public LogManager::Logger
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

    inline void write( LogManager::Level level, const char* message ) override
    {
        // Do nothing
    }
};
