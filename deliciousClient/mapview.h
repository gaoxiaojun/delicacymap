#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QWidget>
#include <QWebView>
#include <QObject>
#include <QList>
#include <QVector>
#include <QSet>
#include "LocationSvc.h"


class MainWindow;
class MapListener;
class Session;
namespace ProtocolBuffer{
	class Restaurant;
	class RestaurantList;
    class CommentList;
	class User;
}

namespace google{
    namespace protobuf{
        class Closure;
    }
}

class mapview : public QWebView
{
    Q_OBJECT

public:
    mapview(MainWindow *parent = 0);
    ~mapview();
    void changeSession(Session *);
    Session* getSession();

	bool isRestaurantInView( int rid ) { return restaurants.contains(rid); };

    void scroll(int xoffset, int yoffset);
    void centerAt(double latitude, double longtitude);
    void resize(int w, int h);
	void getzoom();

	void addRestaurant(const ProtocolBuffer::Restaurant& r);
	void placeMarker(int minZoom = 0, int maxZoom = 19);	//place "tempMarker"
	void makeMarkerByLatLng(double lat, double lng, const QString& markerTitle = "marker");
	void makeMarkerByXY(int x, int y, const QString& markerTitle = "marker");	//marker generated and saved in "tempMarker"
	//Marker getMarkerInfo();	//get "tempMarker"'s info

	//void drawPolygon(const QVector<LatLng> &vertex,const QString& strokeColor = "#ffffff",int strokeWeight = 1, double strokeOpacity = 0.5, const QString& fillColor = "#ffffff",double fillOpacity = 0.5);

//////////////////////////////Methods used internally////////////////////////////////////////////

    void showCommentsForRestaurant(int rid);

    void commentListArrive(ProtocolBuffer::CommentList*);

    void newcurrentLocation(const QString&);

signals:
    void _LocationUpdate(double latitude, double longitude);

/////////////////////////////////////////////////////////////////////////////////////////////////

signals:
    void NewCommentListArrived(ProtocolBuffer::CommentList*);
    void LocationUpdate(QString);

protected:
    void keyPressEvent( QKeyEvent *event );
    void resizeEvent(QResizeEvent *e);
	//void initLatLngs();

private slots:
	void MapLoaded();
    void setupmapconfiguration();
    void whereami();
    static void MyLocationUpdateCallback(void* context, LocationSvc*);

	void updateCurrentLocation(double latitude, double longitude);

private:
	QSet<int> restaurants;
	//QVector<LatLng> bupt,bnu;
	int markerCount;
    google::protobuf::Closure* commentDataArrive;
    ProtocolBuffer::CommentList* commentlist;
    MainWindow *window;
	MapListener *mapListener;
    Session* session;
    LocationSvc *loc_svc;

public slots:
	void newRestaurants(ProtocolBuffer::RestaurantList*);
    void zoomin();
    void zoomout();
};

#endif // MAPVIEW_H
