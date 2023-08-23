#pragma once

#include <string>

class VersionInfo
{
private:
    bool available;

    std::string productName;
    std::string productVersion;
    std::string companyName;

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

    std::string& getCompanyName()
    {
        return companyName;
    }

    std::string getProductNameAndVersion();
};

