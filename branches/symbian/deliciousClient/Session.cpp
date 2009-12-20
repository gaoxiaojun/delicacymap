#include "Session.h"
#include "../protocol-buffer-src/MapProtocol.pb.h"
#include "qnetworksession.h"
#include "qnetworkconfigmanager.h"
#include "qnetworkconfiguration.h"

QTM_USE_NAMESPACE

Session::Session()
{
    user = new ProtocolBuffer::User();
    network = NULL;
    connect(&getDataSource(), SIGNAL(ready(bool)), this, SIGNAL(ready(bool)), Qt::DirectConnection);
}

Session::~Session()
{
    delete network;
    delete user;
}

MapDataSource& Session::getDataSource()
{
    return datasource;
}

ProtocolBuffer::User* Session::getUser()
{
    return user;
}

void Session::start()
{
    if (!network)
    {
        QNetworkConfigurationManager manager;
        const bool canStartIAP = (manager.capabilities() & QNetworkConfigurationManager::CanStartAndStopInterfaces);
        QNetworkConfiguration cfg = manager.defaultConfiguration();
        if (!cfg.isValid() || !canStartIAP)
            throw;

        network = new QNetworkSession(cfg);
        network->open();
        connect(network, SIGNAL(opened()), &getDataSource(), SLOT(connect()));
    }
    else
        emit ready(true);
}
