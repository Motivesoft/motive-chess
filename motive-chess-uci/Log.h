#pragma once

#include <iomanip>
#include <iostream>
#include <map>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>

extern std::mutex consoleLogMutex;
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

        virtual void write( Log::Level level, const char* message ) = 0;

        inline Log::Level getLevel()
        {
            return level;
        }

        inline void setLevel( Log::Level level )
        {
            this->level = level;
        }
    };

    class Logger
    {
    private:
        Log::Level level;

        inline void log( const char* message ) const
        {
            if ( level >= Log::getDestination()->getLevel() )
            {
                Log::getDestination()->write( level, message );
            }
        }

    public:
        Logger( Log::Level level ) :
            level( level )
        {
            // Do nothing
        }

        virtual ~Logger()
        {
            // Do nothing
        }
             
        inline void operator()( const char* message ) const
        {
            log( message );
        }

        inline void operator()( std::string& message ) const
        {
            log( message.c_str() );
        }

        template<typename T>
        Logger& operator <<( T value )
        {
            perThreadBuffer << value;
            return *this;
        }

        Logger& operator <<( decltype( std::endl<char, std::char_traits<char>> ) )
        {
            Log::getDestination()->write( level, perThreadBuffer.str().c_str() );
            perThreadBuffer.str( std::string() );
            return *this;
        }

        Logger& operator <<( decltype( std::hex ) manip )
        {
            perThreadBuffer << manip;
            return *this;
        }

        Logger& operator <<( decltype( std::setw ) manip )
        {
            perThreadBuffer << manip; 
            return *this;
        }
    };

    inline static Log::Logger Trace = Logger( Log::Level::TRACE );
    inline static Log::Logger Debug = Logger( Log::Level::DEBUG );
    inline static Log::Logger Info  = Logger( Log::Level::INFO );
    inline static Log::Logger Warn  = Logger( Log::Level::WARN );
    inline static Log::Logger Error = Logger( Log::Level::ERROR );

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
        std::cout << levelName( level ) << " " << message << std::endl;
    }
};