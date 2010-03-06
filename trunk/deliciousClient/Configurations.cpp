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

    try{
        doc.LoadFile("configss.xml");

        deliciousClient = doc.FirstChildElement("deliciousClient");
        ticpp::Element* server = deliciousClient->FirstChildElement("server");
        ticpp::Element* address = server->FirstChildElement("address");
        ticpp::Element* port = server->FirstChildElement("port");
        this->address = address->GetText();
        port->FirstChild()->GetValue(&this->port);
    }catch (ticpp::Exception &e)
    {
        return;
    }
}
