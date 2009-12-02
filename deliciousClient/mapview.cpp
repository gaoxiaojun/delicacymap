#include "mapview.h"
#include "MapListener.h"
#include "mainwindow.h"
#include "Session.h"
#include "../protocol-buffer-src/MapProtocol.pb.h"

#include <QWebFrame>
#include <QKeyEvent>
#include <QFile>
#include <QTextStream>
#include <QtDebug>
#include <qgeopositioninfo.h>
#include <qgeopositioninfosource.h>
#include <boost/bind.hpp>

QTM_USE_NAMESPACE

#define OFFSET_PER_BUTTON_PUSH 100
static double LocationChangeThreshold = 0.0005f;

mapview::mapview(MainWindow *parent)
    : QWebView(parent)
{
    QString html("Load html error!!!");
    QFile f(":/webpages/index.htm");
    if (f.open(QIODevice::ReadOnly))
    {
    	QTextStream stream(&f);
        html = stream.readAll();
    }
    else
        html += QString("\nerror code: %1!!!").arg(f.error());
    setHtml(html/*, QUrl("file:///index.htm")*/);
    f.close();

    page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
    page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);

    QWebSettings::setMaximumPagesInCache(0);
    
	qDebug()<<"Done Webkit Init"<<endl;
    //===========================Done Webkit Init=====================================

    window = parent;

    commentlist = new ProtocolBuffer::CommentList();
    commentDataArrive = google::protobuf::NewPermanentCallback(this, &mapview::commentListArrive, commentlist);
	
	mapListener = new MapListener(this);
    changeSession(NULL);
    
	connect(this, SIGNAL(loadFinished(bool)), this, SLOT(setupmapconfiguration()), Qt::DirectConnection);
	connect(this, SIGNAL(loadFinished(bool)), this, SLOT(MapLoaded()), Qt::DirectConnection);
}

mapview::~mapview()
{
	delete mapListener;
    delete commentDataArrive;
    delete commentlist;
}

void mapview::changeSession(Session *s)
{
    session = s;
    mapListener->changeSession(s);
}

Session* mapview::getSession()
{
    return session;
}

void mapview::MapLoaded()
{
	page()->mainFrame()->addToJavaScriptWindowObject(QString("mapListener"),mapListener);//register mapListener

	markerCount = 0;

	loc_svc = QGeoPositionInfoSource::createDefaultSource(this);
	if  (loc_svc)
	{
        loc_svc->setUpdateInterval(3000);
        connect(loc_svc, SIGNAL(positionUpdated(QGeoPositionInfo)), this, SLOT(updateCurrentLocation(QGeoPositionInfo)));
        loc_svc->startUpdates();
	}

	//this->initLatLngs();	//init the bound data for BUPT and BNU
}

void mapview::keyPressEvent( QKeyEvent *event )
{
    //setHtml(QString("%1").arg(event->key()));
    //QWebView::keyPressEvent(event);
    switch (event->key())
    {
    case Qt::Key_Return:
        zoomin();
		break;
    case Qt::Key_Left:
        scroll(-OFFSET_PER_BUTTON_PUSH, 0);
        break;
    case Qt::Key_Right:
        scroll(OFFSET_PER_BUTTON_PUSH, 0);
        break;
    case Qt::Key_Up:
        scroll(0, -OFFSET_PER_BUTTON_PUSH);
        break;
    case Qt::Key_Down:
        scroll(0, +OFFSET_PER_BUTTON_PUSH);
        break;
	case Qt::Key_Space:
		qDebug()<<"Space"<<endl;
		makeMarkerByXY(size().width()/2,size().height()/2,QString("marker %1").arg(markerCount));
		placeMarker();
		
		break;
	case Qt::Key_P:
		qDebug()<<"P"<<endl;
// 		drawPolygon(bupt,"#ffffff",1,0.5,"#ff0000",0.5);
// 		drawPolygon(bnu,"#ffffff",1,0.5,"#0000ff",0.5);
		
		break;
    default:
        QWebView::keyPressEvent(event);
    }
}

void mapview::scroll( int xoffset, int yoffset )
{
    page()->mainFrame()->evaluateJavaScript(QString("map.panBy(%1, %2);").arg(xoffset).arg(yoffset));
}

void mapview::resizeEvent( QResizeEvent *e )
{
    QWebView::resizeEvent(e);
    resize(e->size().width(), e->size().height());
}

void mapview::getzoom()
{
    page()->mainFrame()->evaluateJavaScript("alert(map.getZoom());");
}

void mapview::zoomin()
{
    page()->mainFrame()->evaluateJavaScript("map.set_zoom(1 + map.get_zoom() );");
}

void mapview::zoomout()
{
    page()->mainFrame()->evaluateJavaScript("map.set_zoom( map.get_zoom() - 1 );");
}

void mapview::centerAt( double latitude, double longtitude )
{
    page()->mainFrame()->evaluateJavaScript(QString("map.set_center(new google.maps.LatLng(%1, %2));").arg(latitude).arg(longtitude));
}

void mapview::resize( int w, int h )
{
    QWebView::resize(w, h);

    page()->mainFrame()->evaluateJavaScript(QString("document.getElementById('map').style.width='%1px';"
                                                    "document.getElementById('map').style.height='%2px';"
                                                    "google.maps.event.trigger(map, 'resize');"
#ifdef _WIN32_WCE
                                                    ).arg(w/2).arg(h/2));
#else
                                                    ).arg(w).arg(h));
#endif
}

void mapview::placeMarker(int minZoom,int maxZoom)
{
	page()->mainFrame()->evaluateJavaScript(QString("markerManager.addMarker(tempMarker,%1,%2);").arg(minZoom).arg(maxZoom));
	//page()->mainFrame()->evaluateJavaScript(QString("GEvent.addListener(tempMarker,'click',function(){markerClicked();})"));
	page()->mainFrame()->evaluateJavaScript(QString("updateRoute();"));
	++markerCount;
}

void mapview::makeMarkerByLatLng(double lat, double lng, const QString& markerTitle)
{
	page()->mainFrame()->evaluateJavaScript(QString("makeMarkerByLatLng(%1,%2,'%3')").arg(lat).arg(lng).arg(markerTitle));
}

void mapview::makeMarkerByXY(int x, int y, const QString& markerTitle)
{
	page()->mainFrame()->evaluateJavaScript(QString("createMarkerByXY(%1,%2,'%3');").arg(x).arg(y).arg(markerTitle));
}

void mapview::addRestaurant( const ProtocolBuffer::Restaurant& r )
{
	page()->mainFrame()->evaluateJavaScript(
		QString("addRestaurant(%1, %2, %3, '%4');")
			.arg(r.rid())
			.arg(r.location().latitude())
			.arg(r.location().longitude())
			.arg(QString::fromUtf8(r.name().c_str(), r.name().length())));
}

void mapview::newRestaurants( ProtocolBuffer::RestaurantList* list )
{
	QString script;
	for (int i=0;i<list->restaurants_size();++i)
		if (!restaurants.contains(list->restaurants( i ).rid()))
		{
			const ProtocolBuffer::Restaurant& r = list->restaurants( i );
			restaurants.insert(r.rid());
			script.append(QString("addRestaurant(%1, %2, %3, '%4');")
				.arg(r.rid())
				.arg(r.location().latitude())
				.arg(r.location().longitude())
				.arg(QString::fromUtf8(r.name().c_str(), r.name().length())));
		}
	if (!script.isEmpty())
		page()->mainFrame()->evaluateJavaScript(script);
}

// void mapview::drawPolygon(const QVector<LatLng> &vertex, const QString& strokeColor,int strokeWeight, double strokeOpacity, const QString& fillColor,double fillOpacity)
// {
// 	QString cmd("map.addOverlay(new GPolygon([");
// 	for (int i=0;i<vertex.size();i++)
// 	{
// 		cmd += QString("new GLatLng(%1,%2),").arg(vertex[i].lat).arg(vertex[i].lng);
// 	}
// 	cmd += (QString("new GLatLng(%1,%2)").arg(vertex[0].lat).arg(vertex[0].lng));	//make sure the polygon is closed
// 	cmd += QString("], '%1',%2,%3,'%4',%5));").arg(strokeColor).arg(strokeWeight).arg(strokeOpacity).arg(fillColor).arg(fillOpacity);
// 	//qDebug()<<cmd<<endl;
// 	page()->mainFrame()->evaluateJavaScript(cmd);
// }

// void mapview::initLatLngs()
// {
// 	bupt.push_back(LatLng(39.9652,116.3610));
// 	bupt.push_back(LatLng(39.9582,116.3613));
// 	bupt.push_back(LatLng(39.9581,116.3551));
// 	bupt.push_back(LatLng(39.9650,116.3548));
// 
// 	bnu.push_back(LatLng(39.9678,116.3609));
// 	bnu.push_back(LatLng(39.9677,116.3701));
// 	bnu.push_back(LatLng(39.9581,116.3710));
// 	bnu.push_back(LatLng(39.9581,116.3613));
// }

void mapview::updateCurrentLocation( const QGeoPositionInfo &info )
{
    if ( _lastposition != info )
    {
        _lastposition = info;
        page()->mainFrame()->evaluateJavaScript(QString("updateMyLocation(%1, %2);")
            .arg(info.coordinate().latitude())
            .arg(info.coordinate().longitude()));
    }
}

void mapview::showCommentsForRestaurant( int rid )
{
    window->interfaceTransit_comment();
    getSession()->getDataSource().GetLastestCommentsOfRestaurant(rid, 20, commentlist, commentDataArrive);
    //getSession()->getDataSource().getr
}

void mapview::commentListArrive(ProtocolBuffer::CommentList* list)
{
    emit NewCommentListArrived(list);
}

void mapview::newcurrentLocation( const QString& s )
{
    emit LocationUpdate(s);
}
