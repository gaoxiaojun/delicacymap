#ifndef __MAPDATASOURCE__H__INCLUDED__
#define __MAPDATASOURCE__H__INCLUDED__

#include <QtCore/qobject>
#include <string>
#include "../protocol-buffer-src/MapProtocol.pb.h"
#undef abort
#include "QTProbufController.h"

class QTProtobufChannel;

class MapDataSource : public QObject
{
    Q_OBJECT
signals:
    void ready(bool success);

public slots:
    void connect();

public:
    MapDataSource();
    ~MapDataSource();
    ::ProtocolBuffer::DMService::Stub* getStub();
    QString error();

    // **Caution** : following methods are not reentrent!!!
    void GetRestaurants(double lattitude_from, double longitude_from, double latitude_to, double longitude_to, int level, ProtocolBuffer::RestaurantList *, google::protobuf::Closure *);
    void GetLastestCommentsOfRestaurant(int rid, int max_entry, ProtocolBuffer::CommentList *, google::protobuf::Closure *);
    void GetLastestCommentsByUser(int uid, int max_entry, ProtocolBuffer::CommentList *, google::protobuf::Closure *);
    void GetCommentsOfUserSince(int uid, const std::string& datetime, ProtocolBuffer::CommentList *, google::protobuf::Closure *);
    void GetCommentsOfRestaurantSince(int rid, const std::string& datetime, ProtocolBuffer::CommentList *, google::protobuf::Closure *);
    void UserLogin(const std::string& useremailaddr, const std::string& password, ProtocolBuffer::User *, google::protobuf::Closure *);
    void GetUser(int uid, ProtocolBuffer::User *, google::protobuf::Closure *);

    void GetRestaurants(ProtocolBuffer::Query *, ProtocolBuffer::RestaurantList *, google::protobuf::Closure *);
    void GetLastestCommentsOfRestaurant(ProtocolBuffer::Query *, ProtocolBuffer::CommentList *, google::protobuf::Closure *);
    void GetLastestCommentsByUser(ProtocolBuffer::Query *, ProtocolBuffer::CommentList *, google::protobuf::Closure *);
    void GetCommentsOfUserSince(ProtocolBuffer::Query *, ProtocolBuffer::CommentList *, google::protobuf::Closure *);
    void GetCommentsOfRestaurantSince(ProtocolBuffer::Query *, ProtocolBuffer::CommentList *, google::protobuf::Closure *);
    void AddCommentForRestaurant(ProtocolBuffer::Query *, ProtocolBuffer::Comment *, google::protobuf::Closure *);
    void UserLogin(ProtocolBuffer::Query *, ProtocolBuffer::User *, google::protobuf::Closure *);
    void GetUser(ProtocolBuffer::Query *, ProtocolBuffer::User *, google::protobuf::Closure *);


protected:
    ProtocolBuffer::Query query;
    ::ProtocolBuffer::DMService::Stub *stub;
    QTProtobufChannel *channel;
    QTProbufController	controller;

private slots:
    void channel_disconnected();
    void channel_connected();
    void channel_error();
};

#endif