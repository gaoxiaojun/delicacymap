#pragma once

#include <string>

class Configurations
{
public:
    static const Configurations& Instance(){ return _config; }

    const std::string& Server_Address() const { return address; }
    int Server_Port() const { return port; }

private:
    Configurations(void);

    void ReloadConfigs();

    std::string address;
    int port;

    static Configurations _config;
};
