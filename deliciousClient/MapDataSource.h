#ifndef __MAPDATASOURCE__H__INCLUDED__
#define __MAPDATASOURCE__H__INCLUDED__

#include <QtCore/qobject>
#include <string>
#include "MapProtocol.pb.h"
#undef abort
using namespace std;

enum UserRelation
{
    Friend = 0,
    BlackList = 1,
    Unspecified = -1
};

class QTProtobufChannel;

class MapDataSource : public QObject
{
    Q_OBJECT
signals:
    void ready(bool success);
    void messageReceived(const ProtocolBuffer::DMessage*);

public slots:
    void connect();

public:
    MapDataSource();
    ~MapDataSource();
    bool isReady();
    QString error();
    QString lastRPCError();

    // **Caution** : following methods are not reentrent!!!
    void GetRestaurants(double lattitude_from, double longitude_from, double latitude_to, double longitude_to, int level, ProtocolBuffer::RestaurantList *, google::protobuf::Closure *);
    void GetLastestCommentsOfRestaurant(int rid, int max_entry, ProtocolBuffer::CommentList *, google::protobuf::Closure *);
    void GetLastestCommentsByUser(int uid, int max_entry, ProtocolBuffer::CommentList *, google::protobuf::Closure *);
    void GetCommentsOfUserSince(int uid, const std::string& datetime, ProtocolBuffer::CommentList *, google::protobuf::Closure *);
    void GetCommentsOfRestaurantSince(int rid, const std::string& datetime, ProtocolBuffer::CommentList *, google::protobuf::Closure *);
    void AddCommentForRestaurant(int rid,int uid,const std::string & content,ProtocolBuffer::Comment* comment,google::protobuf::Closure *done);   
    void UserLogin(const std::string& useremailaddr, const std::string& password, ProtocolBuffer::User *, google::protobuf::Closure *);
    void GetUser(int uid, ProtocolBuffer::User *, google::protobuf::Closure *);
    void GetRelatedUsers(int uid, UserRelation relation, ProtocolBuffer::UserList *, google::protobuf::Closure *);
    void UpdateUserInfo(int uid, const std::string& password, ProtocolBuffer::User* info, google::protobuf::Closure *);
    void SetUserRelation(int uid, int target_uid, UserRelation relation, google::protobuf::Closure *);
    void AddRestaurant(ProtocolBuffer::Restaurant* r, google::protobuf::Closure* done);
    void Search(const std::string& text, ProtocolBuffer::SearchResult* result, google::protobuf::Closure* done);
    void GetSubscribtionInfo(int uid, ProtocolBuffer::SearchResult* result, google::protobuf::Closure* done);
    void RegisterUser(const std::string& username, const std::string& md5PWD, const std::string& nickname, ProtocolBuffer::User* result, google::protobuf::Closure* done);

    void GetRestaurants(ProtocolBuffer::Query *, ProtocolBuffer::RestaurantList *, google::protobuf::Closure *);
    void GetLastestCommentsOfRestaurant(ProtocolBuffer::Query *, ProtocolBuffer::CommentList *, google::protobuf::Closure *);
    void GetLastestCommentsByUser(ProtocolBuffer::Query *, ProtocolBuffer::CommentList *, google::protobuf::Closure *);
    void GetCommentsOfUserSince(ProtocolBuffer::Query *, ProtocolBuffer::CommentList *, google::protobuf::Closure *);
    void GetCommentsOfRestaurantSince(ProtocolBuffer::Query *, ProtocolBuffer::CommentList *, google::protobuf::Closure *);
    void AddCommentForRestaurant(ProtocolBuffer::Query *, ProtocolBuffer::Comment *, google::protobuf::Closure *);
    void UserLogin(ProtocolBuffer::Query *, ProtocolBuffer::User *, google::protobuf::Closure *);
    void GetUser(ProtocolBuffer::Query *, ProtocolBuffer::User *, google::protobuf::Closure *);
    void GetRelatedUsers(ProtocolBuffer::Query *, ProtocolBuffer::UserList *, google::protobuf::Closure *);
    void UpdateUserInfo(ProtocolBuffer::Query *, ProtocolBuffer::User *, google::protobuf::Closure *);
    void SetUserRelation(ProtocolBuffer::Query *, google::protobuf::Closure*);
    void AddRestaurant(ProtocolBuffer::Query *, ProtocolBuffer::Restaurant*, google::protobuf::Closure*);
    void Search(ProtocolBuffer::Query *, ProtocolBuffer::SearchResult*, google::protobuf::Closure*);
    void GetSubscribtionInfo(ProtocolBuffer::Query *, ProtocolBuffer::SearchResult* result, google::protobuf::Closure* done);
    void RegisterUser(ProtocolBuffer::Query *, ProtocolBuffer::User* result, google::protobuf::Closure* done);

    void SendMessage(ProtocolBuffer::DMessage*);
    void SendMessage(int fromuid,int touid,string message);

protected:
    ProtocolBuffer::Query query;
    QTProtobufChannel *channel;

private slots:
    void emitDMessage(const google::protobuf::MessageLite*);

    void channel_disconnected();
    void channel_connected();
    void channel_error();
};

#endif
