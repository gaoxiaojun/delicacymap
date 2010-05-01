#include "Configurations.h"
#include "TinyXMLPP/ticpp.h"

Configurations Configurations::_config;

Configurations::Configurations(void)
{
    ReloadConfigs();
}

void Configurations::ReloadConfigs()
{
    ticpp::Document doc;
    ticpp::Element* deliciousClient;

    // init to default values
    port = 24000;
    address = "127.0.0.1";
    autologin = false;
    mappath = "tiles.map";
    gps_lat = 0.;
    gps_long = 0.;
    smooth_pan = true;
    ui_largeicon = false;

    try{
        doc.LoadFile("configs.xml");

        deliciousClient = doc.FirstChildElement("deliciousClient");
        ticpp::Element* server = deliciousClient->FirstChildElement("server");
        ticpp::Element* address = server->FirstChildElement("address");
        ticpp::Element* port = server->FirstChildElement("port");
        this->address = address->GetTextOrDefault(this->address);
        port->FirstChild()->GetValue(&this->port);
        ticpp::Element* autologin = deliciousClient->FirstChildElement("autologin");
        this->autologin = autologin->GetAttribute<bool>("enabled", false);
        if (this->autologin)
        {
            ticpp::Element* username = autologin->FirstChildElement("username");
            this->login_usr = username->GetTextOrDefault(this->login_usr);
            ticpp::Element* password = autologin->FirstChildElement("password");
            this->login_pwd = password->GetTextOrDefault(this->login_pwd);
        }

        ticpp::Element* location = deliciousClient->FirstChildElement("location");
        if (location)
        {
            ticpp::Element* gps = location->FirstChildElement("gps");
            if (gps)
            {
                ticpp::Element* correction = gps->FirstChildElement("correction");
                correction->GetAttribute("latitude", &gps_lat, false);
                correction->GetAttribute("longitude", &gps_long, false);
            }
        }

        ticpp::Element* mapview = deliciousClient->FirstChildElement("mapview");
        if (mapview)
        {
            ticpp::Element* smoothpan = mapview->FirstChildElement("smoothscroll");
            if (smoothpan)
            {
                smoothpan->GetTextOrDefault(&smooth_pan, smooth_pan);
            }
            ticpp::Element* datapath = mapview->FirstChildElement("datapath");
            if (datapath)
            {
                mappath = datapath->GetTextOrDefault(mappath);
            }
        }

        ticpp::Element* ui = deliciousClient->FirstChildElement("UI");
        if (ui)
        {
            ticpp::Element* toolbar = ui->FirstChildElement("ToolBar");
            if (toolbar)
            {
                toolbar->GetAttributeOrDefault("UserLargeIcon", &ui_largeicon, ui_largeicon);
            }
        }

    }catch (ticpp::Exception &e)
    {
        return;
    }
}

void Configurations::SaveConfigs()
{

}
