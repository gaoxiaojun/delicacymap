#include "MapListenerPrivate.h"

MapListenerPrivate::MapListenerPrivate(MapListener* parent)
{
	isfirstbound = true;
	closure = google::protobuf::NewPermanentCallback(this, &MapListenerPrivate::newRestaurantDataArrive);
}

void MapListenerPrivate::newRestaurantDataArrive( )
{
	emit RestaurantListDataArrive(&list);
}
