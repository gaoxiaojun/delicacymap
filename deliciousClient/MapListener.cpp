#include <QtDebug>
#include <QWebFrame>
#include "MapListener.h"
#include "mapview.h"

void MapListener::markerClicked()
{
	qDebug()<<"marker clicked";
}

void MapListener::mapClicked(QString s)
{
	((mapview*)parent())->page()->mainFrame()->evaluateJavaScript(QString("map.panTo(tempMarker.getLatLng());")); 
	qDebug()<<"map clicked:"<<s;
}