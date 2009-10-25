#pragma once

#include <QtCore>
#include <string>
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

	// **Caution** : following methods are not reentrent!!!
	void GetRestaurants(double lattitude_from, double longitude_from, double latitude_to, double longitude_to, int level, ProtocolBuffer::RestaurantList *, google::protobuf::Closure *);
	void GetLastestCommentsOfRestaurant(int rid, int max_entry, ProtocolBuffer::CommentList *, google::protobuf::Closure *);
	void GetLastestCommentsByUser(int uid, int max_entry, ProtocolBuffer::CommentList *, google::protobuf::Closure *);
	void GetCommentsOfUserSince(int uid, const std::string& datetime, ProtocolBuffer::CommentList *, google::protobuf::Closure *);
	void GetCommentsOfRestaurantSince(int rid, const std::string& datetime, ProtocolBuffer::CommentList *, google::protobuf::Closure *);

	void GetRestaurants(ProtocolBuffer::Query *, ProtocolBuffer::RestaurantList *, google::protobuf::Closure *);
	void GetLastestCommentsOfRestaurant(ProtocolBuffer::Query *, ProtocolBuffer::CommentList *, google::protobuf::Closure *);
	void GetLastestCommentsByUser(ProtocolBuffer::Query *, ProtocolBuffer::CommentList *, google::protobuf::Closure *);
	void GetCommentsOfUserSince(ProtocolBuffer::Query *, ProtocolBuffer::CommentList *, google::protobuf::Closure *);
	void GetCommentsOfRestaurantSince(ProtocolBuffer::Query *, ProtocolBuffer::CommentList *, google::protobuf::Closure *);
	void AddCommentForRestaurant(ProtocolBuffer::Query *, ProtocolBuffer::Comment *, google::protobuf::Closure *);


protected:
	ProtocolBuffer::Query query;
	::ProtocolBuffer::DMService::Stub *stub;
	QTProtobufChannel *channel;
	QTProbufController	controller;
};