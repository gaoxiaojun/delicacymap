#pragma once
#include <QObject>
#include "MapListener.h"
#include "MapDataSource.h"

// all js script calls are single threaded, so it is efficient to store the RPC object here and not 
// worry about synchronized access

class MapListenerPrivate : public QObject
{
	Q_OBJECT
signals:
	void RestaurantListDataArrive(ProtocolBuffer::RestaurantList* );
	void CommentListDataArrive(ProtocolBuffer::CommentList*);

public:

	MapListenerPrivate(MapListener* parent);

	void newRestaurantDataArrive();
	void newCommentDataArrive();

	MapDataSource connman;
	ProtocolBuffer::RestaurantList restaurantList;
	ProtocolBuffer::CommentList commentList;
	Bound LastBound;
	google::protobuf::Closure *restaurantClosure;
	google::protobuf::Closure *commentClosure;
	bool isfirstbound;	
};
