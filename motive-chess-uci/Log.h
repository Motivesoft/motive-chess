#pragma once

#include <cstdarg>
#include <cstdio>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>

extern std::mutex consoleLogMutex;
extern std::mutex fileLogMutex;
extern thread_local std::stringstream perThreadBuffer;

class Log
{
public:
    enum class Level
    {
        TRACE, DEBUG, INFO, WARN, ERROR, NONE
    };

    class Destination
    {
    protected:
        static const char* levelName( Log::Level level );
        static std::string timestamp();

        Log::Level level;

    public:
        Destination( Log::Level level ) :
            level( level )
        {
            // Nothing to do here
        }
        
        virtual ~Destination()
        {
            // Nothing to do here
        }

        virtual void setLevel( Log::Level level )
        {
            this->level = level;
        }

        inline Log::Level getLevel() const
        {
            return level;
        }

        inline bool isIncluded( Log::Level level ) const 
        {
            return level >= this->level;
        }

        inline bool isExcluded( Log::Level level ) const
        {
            return level < this->level;
        }

        virtual void write( Log::Level level, const char* message ) = 0;
    };

    class Logger
    {
    private:
        Log::Level level;

        inline void write( const char* message ) const
        {
            Log::getDestination()->write( level, message );
        }

        Logger( Log::Level level ) :
            level( level )
        {
            // Do nothing
        }

    public:
        virtual ~Logger()
        {
            // Do nothing
        }
             
        void operator()( const char* format, ... ) const;

        void operator()( std::string& message ) const
        {
            if ( Log::isIncluded( level ) )
            {
                write( message.c_str() );
            }
        }

        typedef const std::function<void( const Log::Logger& logger )> LogCallback;

        // Calling syntax:
        //     Log::Error( [&] ( const Log::Logger& logger )
        //     {
        //         logger << "Benchmarking: " << ( benchmarking ? "on" : "off" ) << std::endl;
        //     } );
        void operator()( LogCallback& logCallback ) const
        {
            if ( Log::isIncluded( level ) )
            {
                logCallback( *this );
            }
        }

        template<typename T>
        const Logger& operator <<( T value ) const
        {
            // Save all the time we can
            if ( Log::isIncluded( level ) )
            {
                perThreadBuffer << value;
            }

            return *this;
        }

        const Logger& operator <<( decltype( std::endl<char, std::char_traits<char>> ) ) const
        {
            static const std::string blank;

            if ( Log::isIncluded( level ) )
            {
                write( perThreadBuffer.str().c_str() );

                // This should probably be done outside of this if-block because there is a chance of
                // some stale content in this case:
                // 
                // Log::setLevel( Debug );
                // Log::Debug << "Hello";
                // Log::setLevel( Error );
                // Log::Debug << std::endl;
                // Log::setLevel( Debug );
                //
                // If this becomes problematic, comment this line of code and uncomment the lines below
                perThreadBuffer.str( blank );
            }

            // Uncomment to clear the buffer more diligently - which, of course, will take extra time
            //if ( !perThreadBuffer.str().empty() )
            //{
            //    perThreadBuffer.str( blank );
            //}

            return *this;
        }

        const Logger& operator <<( decltype( std::hex ) manip ) const
        {
            if ( Log::isIncluded( level ) )
            {
                perThreadBuffer << manip;
            }

            return *this;
        }

        const Logger& operator <<( decltype( std::setw ) manip ) const
        {
            if ( Log::isIncluded( level ) )
            {
                perThreadBuffer << manip;
            }

            return *this;
        }

        friend class Log;
    };

    inline static const Log::Logger Trace = Logger( Log::Level::TRACE );
    inline static const Log::Logger Debug = Logger( Log::Level::DEBUG );
    inline static const Log::Logger Info  = Logger( Log::Level::INFO );
    inline static const Log::Logger Warn  = Logger( Log::Level::WARN );
    inline static const Log::Logger Error = Logger( Log::Level::ERROR );

    inline static const Log::Logger& logger( Log::Level level )
    {
        switch ( level )
        {
            case Log::Level::TRACE:
                return Log::Trace;

            case Log::Level::DEBUG:
                return Log::Debug;

            case Log::Level::INFO:
                return Log::Info;

            case Log::Level::WARN:
                return Log::Warn;

            case Log::Level::ERROR:
                return Log::Error;

            case Log::Level::NONE:
            default:
                Log::Error( "Logger does not exist. Returning alternative." );
                return Log::Error;
        }
    }

    inline static void setDestination( Log::Destination* destination )
    {
        if ( Log::destination != nullptr )
        {
            delete Log::destination;
        }

        Log::destination = destination;
    }

    inline static Log::Destination* getDestination() 
    {
        return Log::destination;
    }

    inline static bool hasDestination()
    {
        return Log::destination != nullptr;
    }

    inline static void shutdown()
    {
        setDestination( nullptr );
    }

    // Helper methods
    inline static bool isIncluded( Log::Level level )
    {
        if ( Log::destination == nullptr )
        {
            return false;
        }

        return Log::destination->isIncluded( level );
    }

    inline static bool isExcluded( Log::Level level )
    {
        if ( Log::destination == nullptr )
        {
            return true;
        }

        return Log::destination->isExcluded( level );
    }

private:
    inline static Log::Destination* destination = nullptr;
};

class ConsoleLogDestination : public Log::Destination
{
public:
    ConsoleLogDestination( Log::Level level = Log::Level::INFO ) :
        Log::Destination( level )
    {
        // Nothing to do here
    }

    inline void write( Log::Level level, const char* message ) override
    {
        std::lock_guard<std::mutex> guard( consoleLogMutex );
        std::cout << timestamp() << " " << levelName( level ) << " " << message << std::endl;
    }
};

class FileLogDestination : public Log::Destination
{
private:
    std::ofstream stream;

public:
    FileLogDestination( const std::string& filename, Log::Level level = Log::Level::INFO ) :
        FileLogDestination( filename.c_str(), level )
    {
        // Do nothing. All the action deferred to the other constructor
    }

    FileLogDestination( const char* filename, Log::Level level = Log::Level::INFO ) :
        Log::Destination( level )
    {
        stream.open( filename, std::ios::out );
    }

    virtual ~FileLogDestination()
    {
        stream.flush();
        stream.close();
    }

    inline void write( Log::Level level, const char* message ) override
    {
        std::lock_guard<std::mutex> guard( fileLogMutex );

        stream << timestamp() << " " << levelName( level ) << " " << message << std::endl;

        // TODO decide if we want this - might be costly
        stream.flush();
    }
};

class NullLogDestination : public Log::Destination
{
public:
    NullLogDestination() :
        Log::Destination( Log::Level::NONE )
    {
        // Nothing to do here
    }

    inline void write( Log::Level level, const char* message ) override
    {
        // Nothing to do here
    }
};

class TeeLogDestination : public Log::Destination
{
private:
    FileLogDestination* fileLogDestination;
    ConsoleLogDestination* consoleLogDestination;

public:
    TeeLogDestination( const std::string& filename, Log::Level level = Log::Level::INFO ) :
        TeeLogDestination( filename.c_str(), level )
    {
        // Do nothing. All the action deferred to the other constructor
    }

    TeeLogDestination( const char* filename, Log::Level level = Log::Level::INFO ) :
        Log::Destination( level ),
        fileLogDestination( new FileLogDestination( filename, level ) ),
        consoleLogDestination( new ConsoleLogDestination() )
    {
        // Nothing to do here
    }

    virtual ~TeeLogDestination()
    {
        delete fileLogDestination;
        delete consoleLogDestination;
    }

    inline void write( Log::Level level, const char* message ) override
    {
        fileLogDestination->write( level, message );
        consoleLogDestination->write( level, message );
    }

    void setLevel( Log::Level level ) override
    {
        this->level = level;
        fileLogDestination->setLevel( level );
        consoleLogDestination->setLevel( level );
    }
};
