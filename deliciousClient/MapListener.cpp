#include <QtDebug>
#include <QWebFrame>
#include <QVariant>
#include "MapListener.h"
#include "MapListenerPrivate.h"
#include "mapview.h"


MapListener::MapListener( QObject *parent )
:QObject(parent)
{
	this->mview = (mapview*)parent;
	_private = new MapListenerPrivate(this);
	connect(_private, SIGNAL(RestaurantListDataArrive(ProtocolBuffer::RestaurantList*)), mview, SLOT(newRestaurants(ProtocolBuffer::RestaurantList*)));
}

void MapListener::markerClicked()
{
	qDebug()<<"marker clicked";
}

void MapListener::mapClicked(QString s)
{
	mview->page()->mainFrame()->evaluateJavaScript(QString("map.panTo(tempMarker.getLatLng());")); 
	qDebug()<<"map clicked:"<<s;
}

void MapListener::mapBoundChanged(const QString& boundstr)
{
	Bound bound = googleboundToMyBound(boundstr);
	if (_private->isfirstbound)
	{
		_private->isfirstbound = false;
		_private->LastBound = bound;
	}
	_private->connman.GetRestaurants(
		bound.SW.lat, bound.SW.lng,
		bound.NE.lat, bound.NE.lng,
		10,
		&_private->list,
		_private->closure);
}

// QString& trimHead(QString& str, char c)
// {
// 	while (str.startsWith(c))
// 		str.remove(0,1);
// 	return str;
// }
// 
// QString& trimTail(QString& str, char c)
// {
// 	while (str.endsWith(c))
// 		str.chop(1);
// 	return str;
// }

Bound MapListener::googleboundToMyBound(const QString& boundstr)
{
	assert(boundstr.length() > 0);

	QStringList list = boundstr.split(',');
	assert(list.count() == 4);

	Bound mapbound;
	mapbound.SW.lat = list[0].toDouble();
	mapbound.SW.lng = list[1].toDouble();
	mapbound.NE.lat = list[2].toDouble();
	mapbound.NE.lng = list[3].toDouble();
	return mapbound;
}

void MapListener::restaurantMarkerClicked( int rid )
{

}
