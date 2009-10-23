#pragma once

#include <QtCore>
#include "../protocol-buffer-src/MapProtocol.pb.h"
#undef abort
#include "QTProbufController.h"

class QTProtobufChannel;

class MapDataSource : public QObject
{
	Q_OBJECT
public:
	MapDataSource();
	~MapDataSource();
	::ProtocolBuffer::DMService::Stub* getStub();

	void GetRestaurants(ProtocolBuffer::Query *, ProtocolBuffer::RestaurantList *, google::protobuf::Closure *);
	void GetLastestCommentsOfRestaurant(ProtocolBuffer::Query *, ProtocolBuffer::CommentList *, google::protobuf::Closure *);
	void GetLastestCommentsByUser(ProtocolBuffer::Query *, ProtocolBuffer::CommentList *, google::protobuf::Closure *);
	void GetCommentsOfUserSince(ProtocolBuffer::Query *, ProtocolBuffer::CommentList *, google::protobuf::Closure *);
	void GetCommentsOfRestaurantSince(ProtocolBuffer::Query *, ProtocolBuffer::CommentList *, google::protobuf::Closure *);


protected:
	::ProtocolBuffer::DMService::Stub *stub;
	QTProtobufChannel *channel;
	QTProbufController	controller;
};