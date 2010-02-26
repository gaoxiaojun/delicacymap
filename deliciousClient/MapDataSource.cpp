#include "MapDataSource.h"
#include "QTProtobufChannel.h"
#include <QDebug>
#include <QtDebug>

#define PORT_NUM 24000

MapDataSource::MapDataSource()
{
    channel = new QTProtobufChannel(QHostAddress("192.168.123.194"), PORT_NUM);
    QObject::connect(channel, SIGNAL(connected()), this, SLOT(channel_connected()));
    QObject::connect(channel, SIGNAL(error()), this, SLOT(channel_error()));
    QObject::connect(channel, SIGNAL(disconnected()), this, SLOT(channel_disconnected()));
    QObject::connect(channel, SIGNAL(messageReceived(const google::protobuf::MessageLite*)), this, SLOT(emitDMessage(const google::protobuf::MessageLite*)));
}

MapDataSource::~MapDataSource()
{
    delete channel;
}

void MapDataSource::emitDMessage( const google::protobuf::MessageLite*  msg )
{
    emit messageReceived( ::google::protobuf::down_cast<const ProtocolBuffer::DMessage*>(msg) );
}

void MapDataSource::channel_disconnected()
{
    qDebug()<<"Channel disconnected. reconnecting...";
    connect();
}

void MapDataSource::channel_error()
{
    emit ready(false);
}
void MapDataSource::channel_connected()
{
    emit ready(true);
}

void MapDataSource::connect()
{
    if (channel->started())
        emit ready(true);
    else
        channel->start();
}

QString MapDataSource::error()
{
    return channel->errorString();
}


void MapDataSource::GetRestaurants( ProtocolBuffer::Query *query, ProtocolBuffer::RestaurantList *rlist, google::protobuf::Closure *done )
{
    channel->CallMethod(protorpc::GetRestaurants, query, rlist, done);
}

void MapDataSource::GetRestaurants( double lattitude_from, double longitude_from, double latitude_to, double longitude_to, int level, ProtocolBuffer::RestaurantList *rlist, google::protobuf::Closure *done )
{
    query.Clear();
    query.mutable_area()->mutable_southwest()->set_latitude(lattitude_from);
    query.mutable_area()->mutable_southwest()->set_longitude(longitude_from);
    query.mutable_area()->mutable_northeast()->set_latitude(latitude_to);
    query.mutable_area()->mutable_northeast()->set_longitude(longitude_to);
    query.set_level(level);
    GetRestaurants(&query, rlist, done);
}

void MapDataSource::GetLastestCommentsOfRestaurant( ProtocolBuffer::Query *query, ProtocolBuffer::CommentList *clist, google::protobuf::Closure *done )
{
    channel->CallMethod(protorpc::GetLastestCommentsOfRestaurant, query, clist, done);
}

void MapDataSource::GetLastestCommentsOfRestaurant( int rid, int max_entry, ProtocolBuffer::CommentList *clist, google::protobuf::Closure *done )
{
    query.Clear();
    query.set_rid(rid);
    query.set_n(max_entry);
    GetLastestCommentsOfRestaurant(&query, clist, done);
}

void MapDataSource::GetLastestCommentsByUser( ProtocolBuffer::Query *query, ProtocolBuffer::CommentList *clist, google::protobuf::Closure *done )
{
    channel->CallMethod(protorpc::GetLastestCommentsByUser, query, clist, done);
}

void MapDataSource::GetLastestCommentsByUser( int uid, int max_entry, ProtocolBuffer::CommentList *clist, google::protobuf::Closure *done )
{
    query.Clear();
    query.set_uid(uid);
    query.set_n(max_entry);
    GetLastestCommentsByUser(&query, clist, done);
}

void MapDataSource::GetCommentsOfUserSince( ProtocolBuffer::Query *query, ProtocolBuffer::CommentList *clist, google::protobuf::Closure *done )
{
    channel->CallMethod(protorpc::GetCommentsOfUserSince, query, clist, done);
}

void MapDataSource::GetCommentsOfUserSince( int uid, const std::string& datetime, ProtocolBuffer::CommentList *clist, google::protobuf::Closure *done )
{
    query.Clear();
    query.set_uid(uid);
    query.mutable_time()->set_timestamp(datetime);
    GetCommentsOfUserSince(&query, clist, done);
}

void MapDataSource::GetCommentsOfRestaurantSince( ProtocolBuffer::Query *query, ProtocolBuffer::CommentList *clist, google::protobuf::Closure *done )
{
    channel->CallMethod(protorpc::GetCommentsOfRestaurantSince, query, clist, done);
}

void MapDataSource::GetCommentsOfRestaurantSince( int rid, const std::string& datetime, ProtocolBuffer::CommentList *clist, google::protobuf::Closure *done )
{
    query.Clear();
    query.set_rid(rid);
    query.mutable_time()->set_timestamp(datetime);
    GetCommentsOfUserSince(&query, clist, done);
}

void MapDataSource::AddCommentForRestaurant( ProtocolBuffer::Query *query, ProtocolBuffer::Comment *comment, google::protobuf::Closure *done )
{
    channel->CallMethod(protorpc::AddCommentForRestaurant, query, comment, done);
}

void MapDataSource::UserLogin( const std::string& useremailaddr, const std::string& password, ProtocolBuffer::User *usr, google::protobuf::Closure *done )
{
    query.Clear();
    query.set_emailaddress(useremailaddr);
    query.set_password(password);
    UserLogin(&query, usr, done);
}

void MapDataSource::UserLogin( ProtocolBuffer::Query *query, ProtocolBuffer::User *usr, google::protobuf::Closure *done )
{
    channel->CallMethod(protorpc::UserLogin, query, usr, done);
}

void MapDataSource::GetUser( ProtocolBuffer::Query *query, ProtocolBuffer::User *usr, google::protobuf::Closure *done )
{
    channel->CallMethod(protorpc::GetUserInfo, query, usr, done);
}

void MapDataSource::GetUser( int uid, ProtocolBuffer::User *usr, google::protobuf::Closure *done )
{
    query.Clear();
    query.set_uid(uid);
    GetUser(&query, usr, done);
}

void MapDataSource::UpdateUserInfo( int uid, const std::string& password, ProtocolBuffer::User* info, google::protobuf::Closure *done )
{
    query.Clear();
    query.set_uid(uid);
    query.set_password(password);
    query.mutable_userinfo()->CopyFrom(*info);
    UpdateUserInfo(&query, info, done);
}

void MapDataSource::UpdateUserInfo( ProtocolBuffer::Query *query, ProtocolBuffer::User *usr, google::protobuf::Closure *done )
{
    channel->CallMethod(protorpc::UpdateUserInfo, query, usr, done);
}

void MapDataSource::SendMessage( ProtocolBuffer::DMessage* m )
{
    channel->SendMessage(m);
}

void MapDataSource::GetRelatedUsers( ProtocolBuffer::Query *query, ProtocolBuffer::UserList *users, google::protobuf::Closure *done )
{
    channel->CallMethod( protorpc::GetRelatedUsers, query, users, done);
}

void MapDataSource::GetRelatedUsers( int uid, int relation, ProtocolBuffer::UserList *users, google::protobuf::Closure *done )
{
    // User relation is specified in DBManager/RelationConstants.cs, as follows:
    // UserRelationship { Friend = 0, BlackList, Unspecified = -1 };
    query.Clear();
    query.set_uid(uid);
    query.set_relation(relation);
    GetRelatedUsers(&query, users, done);
}
