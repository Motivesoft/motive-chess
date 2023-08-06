#pragma once

#include <string>

class VersionInfo
{
private:
    bool available;

    std::string productName;
    std::string productVersion;

    VersionInfo( const char* filename );

public:
    static VersionInfo* getVersionInfo();

    bool isAvailable()
    {
        return available;
    }

    std::string& getProductName()
    {
        return productName;
    }

    std::string& getProductVersion()
    {
        return productVersion;
    }
};

