#ifndef __MAPLISTENER__H__INCLUDED__
#define __MAPLISTENER__H__INCLUDED__


#include <QObject>

class mapview;

struct LatLng
{
	LatLng() {lat=0;lng=0;}
	LatLng(double Lat,double Lng) {lat = Lat; lng = Lng;}
	double lat;
	double lng;
};

struct Bound
{
	LatLng SW,NE;
};

class MapListener : public QObject
{
	Q_OBJECT

public:
	MapListener(QObject *parent);
public slots:
	void markerClicked();
	void mapClicked(QString s);
	void mapBoundChanged();

private:
	Bound getCurrentBoundFromMap();
private:
	Bound LastBound;
	mapview *mview;
	bool isfirstbound;
};


#endif
