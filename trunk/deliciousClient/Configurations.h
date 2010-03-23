#pragma once

#include <string>

class Configurations
{
public:
    static Configurations& Instance(){ return _config; }
    void SaveConfigs();

    const std::string& Server_Address() const { return address; }
    int Server_Port() const { return port; }

    bool EnableAutoLogin() const { return autologin; }
    const std::string& AutoLogin_Username() const { return login_usr; }
    const std::string& AutoLogin_Password() const { return login_pwd; }
    const std::string& MapPath() const { return mappath; };

private:
    Configurations(void);

    void ReloadConfigs();

    std::string address, login_usr, login_pwd, mappath;
    int port;
    bool autologin;

    static Configurations _config;
};
