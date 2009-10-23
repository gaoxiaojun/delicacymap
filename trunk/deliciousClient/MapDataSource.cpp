#include "MapDataSource.h"
#include "QTProtobufChannel.h"
#include <QtDebug>

#define PORT_NUM 24000

MapDataSource::MapDataSource()
{
	channel = new QTProtobufChannel(QHostAddress::LocalHost, PORT_NUM);
	stub = new ::ProtocolBuffer::DMService::Stub(channel);
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

void MapDataSource::GetRestaurants( ProtocolBuffer::Query *query, ProtocolBuffer::RestaurantList *rlist, google::protobuf::Closure *done )
{
	getStub()->GetRestaurants(&controller, query, rlist, done);
}

void MapDataSource::GetLastestCommentsOfRestaurant( ProtocolBuffer::Query *query, ProtocolBuffer::CommentList *clist, google::protobuf::Closure *done )
{
	getStub()->GetLastestCommentsOfRestaurant(&controller, query, clist, done);
}

void MapDataSource::GetLastestCommentsByUser( ProtocolBuffer::Query *query, ProtocolBuffer::CommentList *clist, google::protobuf::Closure *done )
{
	getStub()->GetLastestCommentsByUser(&controller, query, clist, done);
}

void MapDataSource::GetCommentsOfUserSince( ProtocolBuffer::Query *query, ProtocolBuffer::CommentList *clist, google::protobuf::Closure *done )
{
	getStub()->GetCommentsOfUserSince(&controller, query, clist, done);
}

void MapDataSource::GetCommentsOfRestaurantSince( ProtocolBuffer::Query *query, ProtocolBuffer::CommentList *clist, google::protobuf::Closure *done )
{
	getStub()->GetCommentsOfRestaurantSince(&controller, query, clist, done);
}
