#ifndef __MAPLISTENER__H__INCLUDED__
#define __MAPLISTENER__H__INCLUDED__


#include <QObject>

class MapListener : public QObject
{
	Q_OBJECT

public:
	MapListener(QObject *parent):QObject(parent)
	{}
	public slots:
		void markerClicked();
		void mapClicked(QString s);
};


#endif
