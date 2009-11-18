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
	connect(_private, SIGNAL(RestaurantListDataArrive(ProtocolBuffer::RestaurantList*)), this, SLOT(newRestaurants(ProtocolBuffer::RestaurantList*)));
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

void MapListener::mapBoundChanged()
{
	Bound bound = getCurrentBoundFromMap();
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

QString& trimHead(QString& str, char c)
{
	while (str.startsWith(c))
		str.remove(0,1);
	return str;
}

QString& trimTail(QString& str, char c)
{
	while (str.endsWith(c))
		str.chop(1);
	return str;
}

Bound MapListener::getCurrentBoundFromMap()
{
	QVariant bound = mview->page()->mainFrame()->evaluateJavaScript(QString("map.getBounds().toString()"));
	QString s = bound.value<QString>();
	assert(s.length() > 0);

	QStringList list = s.split(',');
	assert(list.count() == 4);

	Bound mapbound;
	mapbound.SW.lat = trimTail(trimHead(list[0].trimmed(), '('), ')').toDouble();
	mapbound.SW.lng = trimTail(trimHead(list[1].trimmed(), '('), ')').toDouble();
	mapbound.NE.lat = trimTail(trimHead(list[2].trimmed(), '('), ')').toDouble();
	mapbound.NE.lng = trimTail(trimHead(list[3].trimmed(), '('), ')').toDouble();
	return mapbound;
}

void MapListener::newRestaurants( ProtocolBuffer::RestaurantList* list )
{
	for (int i=0;i<list->restaurants_size();++i)
	{
		mview->addRestaurant(list->restaurants( i ));
	}
}