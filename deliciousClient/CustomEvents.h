#include <QEvent>
#include "MapProtocol.pb.h"

class RestaurantListEvent : public QEvent
{
public:
	RestaurantListEvent():QEvent(QEvent::Type(User+1)){}
	ProtocolBuffer::RestaurantList* rlist;
};