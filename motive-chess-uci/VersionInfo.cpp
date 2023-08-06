#include "VersionInfo.h"

#include <sstream>
#include <windows.h>
#include <winver.h>

VersionInfo* VersionInfo::getVersionInfo()
{
    char buffer[ MAX_PATH ];
    ::GetModuleFileNameA( nullptr, buffer, MAX_PATH );

    return new VersionInfo( buffer );
}

VersionInfo::VersionInfo( const char* filename )
{
    DWORD sizeHandle;
    DWORD size = ::GetFileVersionInfoSizeA( filename, &sizeHandle );

    available = size > 0;

    if ( size > 0 )
    {
        char* buffer = new char[ size ];

        available = ::GetFileVersionInfoA( filename, 0, size, buffer );

        if ( available )
        {
            LPVOID valuePointer;
            UINT valueLength;

            if ( ::VerQueryValueA( buffer, "\\StringFileInfo\\080904b0\\ProductName", &valuePointer, &valueLength ) )
            {
                productName = std::string( (char*) valuePointer );
            }

            if ( ::VerQueryValueA( buffer, "\\StringFileInfo\\080904b0\\ProductVersion", &valuePointer, &valueLength ) )
            {
                productVersion = std::string( (char*) valuePointer );
            }
        }

        delete[] buffer;
    }
}