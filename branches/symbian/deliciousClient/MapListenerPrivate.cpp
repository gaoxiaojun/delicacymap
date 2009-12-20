#include "MapListenerPrivate.h"

MapListenerPrivate::MapListenerPrivate(MapListener* parent)
{
	isfirstbound = true;
	restaurantClosure = google::protobuf::NewPermanentCallback(this, &MapListenerPrivate::newRestaurantDataArrive);
}

MapListenerPrivate::~MapListenerPrivate()
{
	// this is not completely right. if some request are still in the rpc channel, this may leak
// 	while (!commentLists.empty())
// 		delete commentLists.pop();
}

void MapListenerPrivate::changeSession(Session *s)
{
    session = s;
}

Session* MapListenerPrivate::getSession()
{
    return session;
}

void MapListenerPrivate::newRestaurantDataArrive( )
{
	emit RestaurantListDataArrive(&restaurantList);
}
// 
// void MapListenerPrivate::newCommentDataArrive(CommentCallEntry *entry)
// {
// 	emit CommentListDataArrive(entry);
// }
// 
// void MapListenerPrivate::moreCommentList()
// {
// 	for (int i=0;i<4;++i)
// 	{
// 		CommentCallEntry* entry = new CommentCallEntry;
// 		entry->callback = google::protobuf::NewPermanentCallback(this, &MapListenerPrivate::newCommentDataArrive, entry);
// 		commentLists.push(entry);
// 	}
// }
// 
// CommentCallEntry* MapListenerPrivate::getCommentList()
// {
// 	if (commentLists.empty())
// 		moreCommentList();
// 	return commentLists.pop();
// }
// 
// void MapListenerPrivate::returnCommentList( CommentCallEntry* l )
// {
// 	commentLists.push(l);
// }
