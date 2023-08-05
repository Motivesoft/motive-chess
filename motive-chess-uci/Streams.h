#pragma once

#include <iostream>
#include <fstream>
#include <sstream>

class Streams
{
private:
    std::istream* inputStream;
    std::ostream* outputStream;
    std::ostream* logStream;

    std::ifstream* inputFile;
    std::ofstream* outputFile;
    std::ofstream* logFile;

    void close( std::ifstream** stream )
    {
        if ( *stream )
        {
            ( *stream )->close();

            delete* stream;

            ( *stream ) = nullptr;
        }
    }

    void close( std::ofstream** stream )
    {
        if ( *stream )
        {
            ( *stream )->flush();
            ( *stream )->close();

            delete* stream;

            ( *stream ) = nullptr;
        }
    }

public:
    Streams() :
        inputStream( &std::cin ),
        outputStream( &std::cout ),
        logStream( &std::cerr ),
        inputFile( nullptr ),
        outputFile( nullptr ),
        logFile( nullptr )
    {

    }

    virtual ~Streams()
    {
        close( &inputFile );
        close( &outputFile );
        close( &logFile );
    }

    void setInputFile( std::string filename )
    {
        close( &inputFile );

        inputFile = new std::ifstream();
        inputFile->open( filename );
        inputStream = inputFile;
    }

    void setOutputFile( std::string filename )
    {
        close( &outputFile );

        outputFile = new std::ofstream();
        outputFile->open( filename );
        outputStream = outputFile;
    }

    void setLogFile( std::string filename )
    {
        close( &logFile );

        logFile = new std::ofstream();
        logFile->open( filename );
        logStream = logFile;
    }

    std::ostream& getOuputStream() const
    {
        return *outputStream;
    }

    std::ostream& getLogStream() const
    {
        return *logStream;
    }

    std::istream& getLine( std::string& line )
    {
        return std::getline( *inputStream, line );
    }
};

