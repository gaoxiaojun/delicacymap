#include "Session.h"
#include "MapProtocol.pb.h"
#include "QNetworksession"
#include "QNetworkConfiguration"
#include "QNetworkConfigurationManager"

#include <QTimerEvent>

QTM_USE_NAMESPACE

Session::Session()
{
    user = new ProtocolBuffer::User();
    network = NULL;
    connect(&getDataSource(), SIGNAL(ready(bool)), this, SIGNAL(ready(bool)), Qt::DirectConnection);
    info_isdirty = false;
    infotoupdate = NULL;
    updatedone = NULL;
}

Session::~Session()
{
    delete network;
    delete user;
    delete infotoupdate;
    delete updatedone;
}

MapDataSource& Session::getDataSource()
{
    return datasource;
}

ProtocolBuffer::User* Session::getUser()
{
    return user;
}

ProtocolBuffer::User* Session::getUser( int uid )
{
    // temporary solution
    return myfriends.value(uid);//avoid inserting NULL into the container
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

void Session::timerEvent( QTimerEvent *ev )
{
    if (timer.timerId() == ev->timerId())
    {
        if (info_isdirty)
        {
            getDataSource().UpdateUserInfo(getUser()->uid(), getUser()->password(), infotoupdate, updatedone);
            info_isdirty = false;
        }
    }
    else
    {
        QObject::timerEvent(ev);
    }
}

void Session::UserLocationUpdate( double latitude, double longitude )
{
    if (!timer.isActive())
    {
        infotoupdate = new ProtocolBuffer::User;
        infotoupdate->CopyFrom(*user);
        updatedone = google::protobuf::NewPermanentCallback(this, &Session::UpdatedUserInfo);
        timer.start(10000, this);
    }
    infotoupdate->mutable_lastlocation()->set_latitude(latitude);
    infotoupdate->mutable_lastlocation()->set_longitude(longitude);
    info_isdirty = true;
}

void Session::UpdatedUserInfo()
{
    std::swap(infotoupdate, user);
    infotoupdate->CopyFrom(*user);
}

QList<ProtocolBuffer::User*> Session::friends()
{
    return myfriends.values();
}

void Session::loginMessenger()
{
    if (getUser()->has_uid())
    {
        ProtocolBuffer::UserList* users = new ProtocolBuffer::UserList;
        google::protobuf::Closure* closure = google::protobuf::NewCallback(this, &Session::FriendsResponse, users);
        getDataSource().GetRelatedUsers(getUser()->uid(), 0, users, closure);
    }
}

void Session::FriendsResponse(ProtocolBuffer::UserList* users)
{
    for (int i=0;i<users->users_size();i++)
    {
        const ProtocolBuffer::User& u = users->users(i);
        myfriends.insert(u.uid(), new ProtocolBuffer::User(u));
    }
    delete users;
}