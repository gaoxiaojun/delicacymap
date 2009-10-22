#include "MapDataSource.h"
#include <QtDebug>

#define PORT_NUM 24000

MapDataSource::MapDataSource()
{
	channel = new ::protorpc::TwoWayStream(QHostAddress::LocalHost, PORT_NUM, true);
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