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
    subscriptionTimer.start(60000, this);
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
    else if(subscriptionTimer.timerId() == ev->timerId())
    {
        ProtocolBuffer::DMessage msg;
        msg.set_fromuser(getUser()->uid());
        msg.set_touser(0);
        msg.set_msgid(-1);
        msg.set_issystemmessage(true);
        msg.set_systemmessagetype(ProtocolBuffer::RequestSubscriptionUpdate);
        getDataSource().SendMessage(&msg);
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
    //check if we actually need to update
    if (infotoupdate->has_lastlocation() && 
        abs(infotoupdate->lastlocation().latitude() - p.lat.getDouble()) < 0.0003 &&
        abs(infotoupdate->lastlocation().longitude() - p.lng.getDouble()) < 0.0003)
    {
        return;
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
    ProtocolBuffer::AreaEx twoLocations;
    twoLocations.mutable_northeast()->set_location_st(from.toUtf8().constData());
    twoLocations.mutable_southwest()->set_location_st(to.toUtf8().constData());
    msg.set_buffer( twoLocations.SerializeAsString() );
    getDataSource().SendMessage(&msg);
}

void Session::SendRoutingRequest(const GeoPoint &from, const GeoPoint &to, int user)
{
    ProtocolBuffer::DMessage msg;
    msg.set_fromuser(getUser()->uid());
    msg.set_touser(user);
    msg.set_issystemmessage(true);
    msg.set_msgid(-1); // msgid is not set by user code. this is only to satisfy protocol buffer
    msg.set_systemmessagetype(ProtocolBuffer::RequestRouting);
    ProtocolBuffer::AreaEx twoLocations;
    twoLocations.mutable_northeast()->mutable_location_geo()->set_latitude( from.lat.getDouble() );
    twoLocations.mutable_northeast()->mutable_location_geo()->set_longitude( from.lng.getDouble() );
    twoLocations.mutable_southwest()->mutable_location_geo()->set_latitude( to.lat.getDouble() );
    twoLocations.mutable_southwest()->mutable_location_geo()->set_longitude( to.lng.getDouble() );
    msg.set_buffer( twoLocations.SerializeAsString() );
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

void Session::SubscribeToUser( int otherUser )
{
    ProtocolBuffer::DMessage msg;
    msg.set_fromuser(getUser()->uid());
    msg.set_touser(otherUser);
    msg.set_issystemmessage(true);
    msg.set_msgid(-1); // msgid is not set by user code. this is only to satisfy protocol buffer 
    msg.set_systemmessagetype(ProtocolBuffer::SubscribTo);
    getDataSource().SendMessage(&msg);
}

void Session::UnSubscribeFromUser( int otherUser )
{
    ProtocolBuffer::DMessage msg;
    msg.set_fromuser(getUser()->uid());
    msg.set_touser(otherUser);
    msg.set_issystemmessage(true);
    msg.set_msgid(-1); // msgid is not set by user code. this is only to satisfy protocol buffer 
    msg.set_systemmessagetype(ProtocolBuffer::UnSubscribeFrom);
    getDataSource().SendMessage(&msg);
}

void Session::SubscribeToRestaurant( int RID )
{
    ProtocolBuffer::Query queryRID;
    queryRID.set_rid(RID);
    ProtocolBuffer::DMessage msg;
    msg.set_fromuser(getUser()->uid());
    msg.set_touser(0);
    msg.set_issystemmessage(true);
    msg.set_msgid(-1); // msgid is not set by user code. this is only to satisfy protocol buffer 
    msg.set_systemmessagetype(ProtocolBuffer::SubscribTo);
    msg.set_buffer(queryRID.SerializeAsString());
    getDataSource().SendMessage(&msg);
}

void Session::UnSubscribeFromRestaurant( int RID )
{
    ProtocolBuffer::Query queryRID;
    queryRID.set_rid(RID);
    ProtocolBuffer::DMessage msg;
    msg.set_fromuser(getUser()->uid());
    msg.set_touser(0);
    msg.set_issystemmessage(true);
    msg.set_msgid(-1); // msgid is not set by user code. this is only to satisfy protocol buffer 
    msg.set_systemmessagetype(ProtocolBuffer::UnSubscribeFrom);
    msg.set_buffer(queryRID.SerializeAsString());
    getDataSource().SendMessage(&msg);
}

//通过姓名查找uid.......................未完成
int Session::findByName(string & nickName)
{
    QMap<int, ProtocolBuffer::User*>::iterator iter;
    for(iter = myfriends.begin();iter!=myfriends.end();iter++)
    {
    }
    return 0;
}

void Session::setRelationWith(int uid,UserRelation relation)
{
    google::protobuf::Closure* closure = google::protobuf::NewCallback(this, &Session::RelationChangeResponse, uid,relation);
    getDataSource().SetUserRelation(getUser()->uid(),uid,relation,closure);
}

void Session::RelationChangeResponse(int uid,UserRelation relation)
{
    QMap<int, ProtocolBuffer::User*>::iterator iter;
    iter=myfriends.find(uid);
    google::protobuf::Closure* closure = google::protobuf::NewCallback(this, &Session::GetUserResponse); 
    ProtocolBuffer::User* u=new ProtocolBuffer::User;
    getDataSource().GetUser(uid,u,closure);
    switch(relation)
    {
    case Friend:
        if(iter==myfriends.end())
            myfriends.insert(uid, u); //加为好友
        //否则，已经是好友，什么都不做
        break;
    case BlackList:
        break;
    case Unspecified:
        break;
    }
}

void Session::GetUserResponse()
{

}
