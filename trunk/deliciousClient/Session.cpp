#include "Session.h"
#include "MapProtocol.pb.h"
#include "QNetworksession"
#include "QNetworkConfiguration"
#include "QNetworkConfigurationManager"

#include <QTimerEvent>
#include <boost/foreach.hpp>

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
            timer.stop();
        }
    }
    else
    {
        QObject::timerEvent(ev);
    }
}

void Session::UserLocationUpdate( const GeoPoint& p )
{
    if (getUser()->password().empty())
        return;
    if (!infotoupdate)
    {
        infotoupdate = new ProtocolBuffer::User;
        infotoupdate->CopyFrom(*user);
        updatedone = google::protobuf::NewPermanentCallback(this, &Session::UpdatedUserInfo);
    }
    if (!timer.isActive())
    {
        timer.start(30000, this);
    }
    infotoupdate->mutable_lastlocation()->set_latitude(p.lat.getDouble());
    infotoupdate->mutable_lastlocation()->set_longitude(p.lng.getDouble());
    info_isdirty = true;
    
    ProtocolBuffer::DMessage msg;
    ProtocolBuffer::Location loc;
    loc.set_latitude(p.lat.getDouble());
    loc.set_longitude(p.lng.getDouble());
    msg.set_fromuser(getUser()->uid());
    msg.set_touser(0);
    msg.set_issystemmessage(true);
    msg.set_msgid(-1); // msgid is not set by user code. this is only to satisfy protocol buffer 
    msg.set_systemmessagetype(ProtocolBuffer::UserLocationUpdate);
    msg.set_buffer(loc.SerializeAsString());
    getDataSource().SendMessage(&msg);
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
        getDataSource().GetRelatedUsers(getUser()->uid(), Friend, users, closure);
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

void Session::SendRoutingReply( const QList<GeoPoint>& route, int user )
{
    ProtocolBuffer::Route routereply;
    BOOST_FOREACH(const GeoPoint& p, route)
    {
        ProtocolBuffer::Location* point = routereply.add_waypoints();
        point->set_latitude(p.lat.getDouble());
        point->set_longitude(p.lng.getDouble());
    }
    ProtocolBuffer::DMessage msg;
    msg.set_fromuser(getUser()->uid());
    msg.set_touser(user);
    msg.set_issystemmessage(true);
    msg.set_msgid(-1); // msgid is not set by user code. this is only to satisfy protocol buffer 
    msg.set_systemmessagetype(ProtocolBuffer::RoutingReply);
    msg.set_buffer(routereply.SerializeAsString());
    getDataSource().SendMessage(&msg);
}

void Session::SendRoutingRequest( const QString& from, const QString& to, int user )
{
    ProtocolBuffer::DMessage msg;
    msg.set_fromuser(getUser()->uid());
    msg.set_touser(user);
    msg.set_issystemmessage(true);
    msg.set_msgid(-1); // msgid is not set by user code. this is only to satisfy protocol buffer 
    msg.set_systemmessagetype(ProtocolBuffer::RequestRouting);
    QString text = from + "|" + to;
    QByteArray utf8encoded = text.toUtf8();
    msg.set_buffer( utf8encoded.constData(), utf8encoded.length() );
    getDataSource().SendMessage(&msg);
}

void Session::ShareMyLocationWith( int otherUser )
{
    ProtocolBuffer::DMessage msg;
    msg.set_fromuser(getUser()->uid());
    msg.set_touser(otherUser);
    msg.set_issystemmessage(true);
    msg.set_msgid(-1); // msgid is not set by user code. this is only to satisfy protocol buffer 
    msg.set_systemmessagetype(ProtocolBuffer::ShareLocationWith);
    getDataSource().SendMessage(&msg);
}
