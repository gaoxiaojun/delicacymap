#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QWidget>
#include <QWebView>
#include <QObject>
#include <QList>
#include <QVector>
#include <QSet>
#include "../protocol-buffer-src/MapProtocol.pb.h"
#include "LocationSvc.h"
#include "MapListener.h"

struct Marker
{
	QString title;
	LatLng latlng;
};

class mapview : public QWebView
{
    Q_OBJECT

public:
    mapview(QWidget *parent = 0);
    ~mapview();

	bool isRestaurantInView( int rid ) { return restaurants.contains(rid); };

    void scroll(int xoffset, int yoffset);
    void centerAt(double latitude, double longtitude);
    void resize(int w, int h);

	void addRestaurant(const ProtocolBuffer::Restaurant& r);
	void placeMarker(int minZoom = 0, int maxZoom = 19);	//place "tempMarker"
	void makeMarkerByLatLng(double lat, double lng, const QString& markerTitle = "marker");
	void makeMarkerByXY(int x, int y, const QString& markerTitle = "marker");	//marker generated and saved in "tempMarker"
	//Marker getMarkerInfo();	//get "tempMarker"'s info

	void drawPolygon(const QVector<LatLng> &vertex,const QString& strokeColor = "#ffffff",int strokeWeight = 1, double strokeOpacity = 0.5, const QString& fillColor = "#ffffff",double fillOpacity = 0.5);

public slots:
	void newRestaurants(ProtocolBuffer::RestaurantList*);

protected:
    void keyPressEvent( QKeyEvent *event );
    void resizeEvent(QResizeEvent *e);
	void initLatLngs();

private slots:
	void MapLoaded();
    void setupmapconfiguration();
    void whereami();
    static void GPSCallback(void* context, LocationSvc*);

private:
	QSet<int> restaurants;
	QVector<LatLng> bupt,bnu;
	int markerCount;
	MapListener* mapListener;
    LocationSvc *loc_svc;

public slots:
    void getzoom();
    void zoomin();
    void zoomout();
};

#endif // MAPVIEW_H
