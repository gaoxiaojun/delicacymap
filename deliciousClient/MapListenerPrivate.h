#pragma once
#include <QObject>
#include <QStack>
#include "MapListener.h"
#include "MapDataSource.h"

// all js script calls are single threaded, so it is efficient to store the RPC object here and not 
// worry about synchronized access

// struct CommentCallEntry
// {
// 	ProtocolBuffer::CommentList list;
// 	google::protobuf::Closure* callback;
// };

class Session;

class MapListenerPrivate : public QObject
{
	Q_OBJECT
signals:
	void RestaurantListDataArrive(ProtocolBuffer::RestaurantList* );
	//void CommentListDataArrive(CommentCallEntry*);

public:

	MapListenerPrivate(MapListener* parent);
	~MapListenerPrivate();

    void changeSession(Session *);
    Session* getSession();

	void newRestaurantDataArrive();
// 	void newCommentDataArrive(CommentCallEntry*);
// 
// 	CommentCallEntry* getCommentList();
// 	void returnCommentList(CommentCallEntry*);

	ProtocolBuffer::RestaurantList restaurantList;
	Bound LastBound;
	google::protobuf::Closure *restaurantClosure;
    Session *session;
	bool isfirstbound;	
private:
// 	void moreCommentList();
// 	QStack<CommentCallEntry*> commentLists;
};
