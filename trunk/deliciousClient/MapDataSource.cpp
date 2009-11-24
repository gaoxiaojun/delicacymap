#include "MapDataSource.h"
#include "QTProtobufChannel.h"
#include <QtDebug>

#define PORT_NUM 24000

MapDataSource::MapDataSource()
{
    channel = new QTProtobufChannel(QHostAddress::LocalHost, PORT_NUM);
    stub = new ::ProtocolBuffer::DMService::Stub(channel);
    QObject::connect(channel, SIGNAL(connected()), this, SLOT(channel_connected()));
    QObject::connect(channel, SIGNAL(error()), this, SLOT(channel_error()));
}

MapDataSource::~MapDataSource()
{
    delete stub;
    delete channel;
}

::ProtocolBuffer::DMService::Stub* MapDataSource::getStub()
{
    return stub;
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
    getStub()->GetRestaurants(&controller, query, rlist, done);
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
    getStub()->GetLastestCommentsOfRestaurant(&controller, query, clist, done);
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
    getStub()->GetLastestCommentsByUser(&controller, query, clist, done);
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
    getStub()->GetCommentsOfUserSince(&controller, query, clist, done);
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
    getStub()->GetCommentsOfRestaurantSince(&controller, query, clist, done);
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
    getStub()->AddCommentForRestaurant(&controller, query, comment, done);
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
    getStub()->UserLogin(&controller, query, usr, done);
}

void MapDataSource::GetUser( ProtocolBuffer::Query *, ProtocolBuffer::User *, google::protobuf::Closure * )
{

}

void MapDataSource::GetUser( int uid, ProtocolBuffer::User *, google::protobuf::Closure * )
{

}
