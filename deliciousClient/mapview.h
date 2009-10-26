#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QWidget>
#include <QWebView>
#include <QObject>
#include <QList>
#include <QVector>
#include "../protocol-buffer-src/MapProtocol.pb.h"
#include "LocationSvc.h"


struct LatLng
{
	LatLng() {lat=0;lng=0;}
	LatLng(double Lat,double Lng) {lat = Lat; lng = Lng;}
	double lat;
	double lng;
};

struct Marker
{
	QString title;
	LatLng latlng;
};



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

class mapview : public QWebView
{
    Q_OBJECT

public:
    mapview(QWidget *parent = 0);
    ~mapview();
    void scroll(int xoffset, int yoffset);
    void centerAt(double latitude, double longtitude);
    void resize(int w, int h);
	void placeMarker(int minZoom = 0, int maxZoom = 19);	//place "tempMarker"
	void makeMarkerByLatLng(double lat, double lng, QString markerTitle = "marker");
	void makeMarkerByXY(int x, int y, QString markerTitle = "marker");	//marker generated and saved in "tempMarker"
	//Marker getMarkerInfo();	//get "tempMarker"'s info

	void drawPolygon(const QVector<LatLng> &vertex,QString strokeColor = "#ffffff",int strokeWeight = 1, double strokeOpacity = 0.5, QString fillColor = "#ffffff",double fillOpacity = 0.5);

protected:
    void keyPressEvent( QKeyEvent *event );
    void resizeEvent(QResizeEvent *e);
	void initLatLngs();

private slots:
    void setupmapconfiguration();
    void whereami();
    static void GPSCallback(void* context, LocationSvc*);

private:
	MapListener* mapListener;
	QVector<LatLng> bupt,bnu;
	int markerCount;
    LocationSvc *loc_svc;

public slots:
    void getzoom();
    void zoomin();
    void zoomout();
};

#endif // MAPVIEW_H
