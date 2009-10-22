#pragma once

#include <QtCore>
#include "../protocol-buffer-src/MapProtocol.pb.h"
#include "twowaystream.h"
#include "simplerpccontroller.h"


class MapDataSource : public QObject
{
	Q_OBJECT
public:
	MapDataSource();
	~MapDataSource();
	::ProtocolBuffer::DMService::Stub* getStub();

	void GetRestaurants(ProtocolBuffer::Query *, ProtocolBuffer::RestaurantList *, google::protobuf::Closure *);
	void GetLastestCommentsOfRestaurant(ProtocolBuffer::Query *, ProtocolBuffer::CommentList *, google::protobuf::Closure *);

protected:
	::ProtocolBuffer::DMService::Stub *stub;
	::protorpc::TwoWayStream *channel;
	::protorpc::SimpleRpcController	controller;
};