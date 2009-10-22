#include <QEvent>

class ProtobufDataEvent : public QEvent
{
public:
	enum EventType { RestaurantListRecv=QEvent::User+1, CommentListRecvs };
	ProtobufDataEvent(EventType t):QEvent((QEvent::Type)t){}
	void* data;
};
