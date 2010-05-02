#ifndef __SESSION__H__INCLUDED__
#define __SESSION__H__INCLUDED__

#include "MapDataSource.h"
#include "qmobilityglobal.h"
#include "OfflineMap/GeoCoord.h"

#include <string>
#include <QObject>
#include <QMap>
#include <QSet>
#include <QList>
#include <QBasicTimer>

namespace ProtocolBuffer
{
    class User;
    class UserList;
}

#if QT_VERSION >= 0x040700
class QNetworkSession;
#define QTM_PREPEND_NAMESPACE(x) x
#else
QTM_BEGIN_NAMESPACE
class QNetworkSession;
QTM_END_NAMESPACE
#endif

class Session : public QObject
{
    Q_OBJECT
public:
    Session();
    ~Session();

    void start();

    MapDataSource& getDataSource();
    ProtocolBuffer::User* getUser(); 

    QList<ProtocolBuffer::User*> friends();
    ProtocolBuffer::User* getUser(int uid);

    bool isSharingLocationWith(int uid) const;
    bool isSubscribedToUser(int uid) const;
    bool isSubscribedToRestaurant(int rid) const;
    
    bool isfriend(int uid);
    void SendRoutingReply( const QList<GeoPoint>& route, int user );
    void SendRoutingRequest( const QString&, const QString&, int user);
    void SendRoutingRequest( const GeoPoint&, const GeoPoint&, int user);
    void ShareMyLocationWith( int otherUser );
    void SubscribeToUser( int otherUser );
    void UnSubscribeFromUser( int otherUser );
    void SubscribeToRestaurant( int RID );
    void UnSubscribeFromRestaurant( int RID );

    void setRelationWith(int uid,UserRelation relation);
    void RelationChangeResponse(int uid,UserRelation relation);
    int findByName(string & nickName);
public slots:
    void UserLocationUpdate(const GeoPoint&);
    void loginMessenger();

signals:
    void ready(bool);
    void userChanged(bool,int);  //true代表添加好友，int为uid

protected:
    void timerEvent(QTimerEvent *);

private slots:
    void handleLocationSharing(const ProtocolBuffer::DMessage*);

private:
    void UpdatedUserInfo();
    void FriendsResponse(ProtocolBuffer::UserList*);
    void GetUserResponse(ProtocolBuffer::User *,UserRelation relation);
    void GetSubscribeUserResponse(ProtocolBuffer::SearchResult*);
    QBasicTimer timer, subscriptionTimer;
    MapDataSource datasource;
    QMap<int, ProtocolBuffer::User*> myfriends;
    QSet<int> userSharingLocationWithMe, userSubscribeTo, restaurantSubscribeTo;
    ProtocolBuffer::User *user, *infotoupdate;
    google::protobuf::Closure *updatedone;
    QTM_PREPEND_NAMESPACE(QNetworkSession)* network;
    bool info_isdirty;
};

#endif
