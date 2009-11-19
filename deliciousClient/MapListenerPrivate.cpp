#include "MapListenerPrivate.h"

MapListenerPrivate::MapListenerPrivate(MapListener* parent)
{
	isfirstbound = true;
	restaurantClosure = google::protobuf::NewPermanentCallback(this, &MapListenerPrivate::newRestaurantDataArrive);
	commentClosure = google::protobuf::NewPermanentCallback(this, &MapListenerPrivate::newCommentDataArrive);
}

void MapListenerPrivate::newRestaurantDataArrive( )
{
	emit RestaurantListDataArrive(&restaurantList);
}

void MapListenerPrivate::newCommentDataArrive()
{
	emit CommentListDataArrive(&commentList);
}
