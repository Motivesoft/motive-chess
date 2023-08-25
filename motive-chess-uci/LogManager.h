#pragma once

#include <cstdarg>
#include <fstream>
#include <functional>
#include <ostream>
#include <sstream>
#include <string>

#define PLOG_DEBUG( ... ) LogManager::getLogger()->log( LogManager::Level::DEBUG, __VA_ARGS__ )
#define SLOG_DEBUG( LOGGER, CODE ) LogManager::getLogger()->log( LogManager::Level::DEBUG, [&] ( LogManager::LevelLogger& LOGGER ) -> void CODE )
#define VLOG_DEBUG( ... ) LogManager::getLogger()->log( LogManager::Level::DEBUG, [&] ( LogManager::LevelLogger& logger ) -> void { logger.write( __VA_ARGS__ ); } )

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

        const char* LevelName( LogManager::Level level ) const;

        std::string getTimestamp();

    public:
        // TODO consider renaming this to 'log'
        virtual void write( LogManager::Level level, const char* message ) = 0;
    };

    /// <summary>
    /// Exists purely to service function-based logging, which is used either for complex or potentially
    /// time consuming stuff that should be called only if guaranteed to be logged
    /// Should look a bit like a logger but is actually a facade
    /// </summary>
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
            // Nothing to do
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

        inline void write( std::string& message )
        {
            logger.write( level, message.c_str() );
        }

        inline void write( const char* message, ... )
        {
            std::va_list args;
            va_start( args, message );
            logger.write( level, message );// , args ); // TODO TODO WAHHH REINSTATE ARGS
            va_end( args );
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

        void log( LogManager::Level level, const char* format, ... )
        {
            if ( level >= getLevel() )
            {
                std::va_list args;
                va_start( args, format );

                char buffer[ 200 ];
                size_t size = snprintf( buffer, sizeof( buffer ), format, args );

                if ( size <= sizeof( buffer ) )
                {
                    write( level, buffer );
                }
                else
                {
                    char* longBuffer = new char[ size ];
                    
                    write( level, longBuffer );
                    snprintf( longBuffer, size, format, args );

                    delete[] longBuffer;
                }

                va_end( args );
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

    /// <summary>
    /// Set the current logger
    /// Any previous logger is destroyed (deleted)
    /// </summary>
    /// <param name="logger">the logger</param>
    static void setLogger( Logger* logger )
    {
        if ( LogManager::logger != nullptr )
        {
            delete LogManager::logger;
        }

        LogManager::logger = logger;
    }

    /// <summary>
    /// Get the currently installed logger
    /// </summary>
    /// <returns>the logger, or nullptr if there is no current logger</returns>
    static Logger* getLogger()
    {
        return LogManager::logger;
    }

    /// <summary>
    /// Convenience method to set the level of the current logger
    /// Does nothing if there is no current logger
    /// </summary>
    /// <param name="level">the level</param>
    static void setLevel( LogManager::Level level )
    {
        if ( logger != nullptr )
        {
            logger->setLevel( level );
        }
    }

    /// <summary>
    /// Convenience method to get the level of the current logger
    /// Returns the system default value if there is no current logger
    /// </summary>
    /// <returns>the level</returns>
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

    void write( LogManager::Level level, const char* message ) override;
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

    void write( LogManager::Level level, const char* message ) override;
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
