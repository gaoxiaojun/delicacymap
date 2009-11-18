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
public:

	MapListenerPrivate(MapListener* parent);

	void newRestaurantDataArrive();

	MapDataSource connman;
	ProtocolBuffer::RestaurantList list;
	Bound LastBound;
	google::protobuf::Closure *closure;
	bool isfirstbound;	
};
