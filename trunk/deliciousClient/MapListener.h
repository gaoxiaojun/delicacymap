#ifndef __MAPLISTENER__H__INCLUDED__
#define __MAPLISTENER__H__INCLUDED__


#include <QObject>

class mapview;
class MapListenerPrivate;

namespace ProtocolBuffer
{
	class RestaurantList;
}

struct LatLng
{
	LatLng() {lat=0;lng=0;}
	LatLng(double Lat,double Lng) {lat = Lat; lng = Lng;}

	bool operator==(const LatLng& other) const {return lat==other.lat && lng==other.lng;}

	double lat;
	double lng;
};

struct Bound
{
	LatLng SW,NE;

	bool operator==(const Bound& other) const {return SW==other.SW && NE==other.NE;}
};

class MapListener : public QObject
{
	Q_OBJECT

public:
	MapListener(QObject *parent);
public slots:
	void markerClicked();
	void mapClicked(QString s);
	void mapBoundChanged(const QString& boundstr);
	void restaurantMarkerClicked(int rid);

private:
	Bound googleboundToMyBound(const QString& boundstr);

private:
	mapview *mview;
	MapListenerPrivate *_private;
};


#endif
