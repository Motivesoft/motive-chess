#pragma once

#include <cstdarg>
#include <cstdio>
#include <fstream>
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
        const Logger& operator <<( T value ) const
        {
            // Save all the time we can
            if ( level >= Log::getDestination()->getLevel() )
            {
                perThreadBuffer << value;
            }

            return *this;
        }

        const Logger& operator <<( decltype( std::endl<char, std::char_traits<char>> ) ) const
        {
            if ( level >= Log::getDestination()->getLevel() )
            {
                Log::getDestination()->write( level, perThreadBuffer.str().c_str() );
            }

            perThreadBuffer.str( std::string() );
            return *this;
        }

        const Logger& operator <<( decltype( std::hex ) manip ) const
        {
            perThreadBuffer << manip;
            return *this;
        }

        const Logger& operator <<( decltype( std::setw ) manip ) const
        {
            perThreadBuffer << manip; 
            return *this;
        }
    };

    inline static const Log::Logger Trace = Logger( Log::Level::TRACE );
    inline static const Log::Logger Debug = Logger( Log::Level::DEBUG );
    inline static const Log::Logger Info  = Logger( Log::Level::INFO );
    inline static const Log::Logger Warn  = Logger( Log::Level::WARN );
    inline static const Log::Logger Error = Logger( Log::Level::ERROR );

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

    // Helper methods
    inline static bool isIncluded( Log::Level level )
    {
        if ( Log::destination != nullptr )
        {
            return level >= Log::destination->getLevel();
        }

        return false;
    }

    inline static bool isExcluded( Log::Level level )
    {
        if ( Log::destination != nullptr )
        {
            return level < Log::destination->getLevel();
        }

        return true;
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
        stream.open( filename, std::ios::out );
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
        stream << timestamp() << " " << levelName( level ) << " " << message << std::endl;
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