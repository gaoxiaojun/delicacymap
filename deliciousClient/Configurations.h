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

    double GPSCorrection_Latitude() const { return gps_lat; };
    double GPSCorrection_Longitude() const { return gps_long; };

    bool UI_UseLargeIcon() const { return ui_largeicon; }

    bool SmoothPan() const { return smooth_pan; }

private:
    Configurations(void);

    void ReloadConfigs();

    double gps_lat, gps_long;
    std::string address, login_usr, login_pwd, mappath;
    int port;
    bool autologin;
    bool smooth_pan;
    bool ui_largeicon;

    static Configurations _config;
};
