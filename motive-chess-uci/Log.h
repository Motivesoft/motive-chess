#pragma once

#include <cstdarg>
#include <cstdio>
#include <iomanip>
#include <iostream>
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
             
        void operator()( const char* format,... ) const
        {
            if ( level >= Log::getDestination()->getLevel() )
            {
                // Hazard a guess for a buffer size
                const size_t initialBuffer = 256;
                char* buffer = new char[ initialBuffer ];

                // Attach the varargs
                va_list args;
                va_start( args, format );

                // Print the string, but don't overflow the buffer and let it tell us if we need 
                // a larger buffer
                size_t size = vsnprintf( buffer, initialBuffer - 1, format, args );
                if ( size > initialBuffer )
                {
                    // Larger buffer required
                    delete[] buffer;
                    buffer = new char[ size + 1 ];

                    // Try again
                    vsnprintf( buffer, size + 1, format, args );
                }

                // Output the log
                Log::getDestination()->write( level, buffer );
                
                // Housekeeping
                va_end( args );
                delete[] buffer;
            }
        }

        inline void operator()( std::string& message ) const
        {
            if ( level >= Log::getDestination()->getLevel() )
            {
                Log::getDestination()->write( level, message.c_str() );
            }
        }

        template<typename T>
        Logger& operator <<( T value )
        {
            // Save all the time we can
            if ( level >= Log::getDestination()->getLevel() )
            {
                perThreadBuffer << value;
            }

            return *this;
        }

        Logger& operator <<( decltype( std::endl<char, std::char_traits<char>> ) )
        {
            if ( level >= Log::getDestination()->getLevel() )
            {
                Log::getDestination()->write( level, perThreadBuffer.str().c_str() );
            }

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