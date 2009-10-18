#include "mapview.h"
#include "LocationSvc.h"

#include <QWebFrame>
#include <QKeyEvent>
#include <QFile>
#include <QTextStream>
#include <QtDebug>
#include <boost/bind.hpp>

#define OFFSET_PER_BUTTON_PUSH 100

mapview::mapview(QWidget *parent, int width, int height)
    : QWebView(parent)
{
    ui.setupUi(this);

    QString html("Load html error!!!");
    //QFile f(":/webpages/index.htm");
	QFile f("E:/Development/Qt/project/DMgoogle/trunk/deliciousClient/webpages/index.htm");
    if (f.open(QIODevice::ReadOnly))
    {
    	QTextStream stream(&f);
#ifdef _WIN32_WCE
        html = stream.readAll().arg(width/2).arg(height/2);
        setZoomFactor(2);
#else
        html = stream.readAll().arg(width).arg(height+40);
#endif
    }
    else
        html += QString("\nerror code: %1!!!").arg(f.error());
    setHtml(html/*, QUrl("file:///index.htm")*/);
    f.close();

    page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
    page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);

    QWebSettings::setMaximumPagesInCache(0);
    QWebSettings::setObjectCacheCapacities(0.5*1024*1024, 1*1024*1024, 0.5*1024*1024);
    
	qDebug()<<"Done Webkit Init"<<endl;
    //===========================Done Webkit Init=====================================
	
	mapListener = new MapListener(this);
	page()->mainFrame()->addToJavaScriptWindowObject(QString("mapListener"),mapListener);//register mapListener
	
	markerCount = 0;    

	this->initLatLngs();	//init the bound data for BUPT and BNU
    
    connect(this, SIGNAL(loadFinished(bool)), this, SLOT(setupmapconfiguration()));

    loc_svc = LocationSvc::create();

    //loc_svc->registerCallbackForNewPosition(&mapview::GPSCallback, this);
}

mapview::~mapview()
{
	delete mapListener;
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
//     case Qt::Key_Left:
//         scroll(OFFSET_PER_BUTTON_PUSH, 0);
//         break;
//     case Qt::Key_Right:
//         scroll(-OFFSET_PER_BUTTON_PUSH, 0);
//         break;
//     case Qt::Key_Up:
//         scroll(0, OFFSET_PER_BUTTON_PUSH);
//         break;
//     case Qt::Key_Down:
//         scroll(0, -OFFSET_PER_BUTTON_PUSH);
//         break;
	case Qt::Key_Space:
		qDebug()<<"Space"<<endl;
		makeMarkerByXY(size().width()/2,size().height()/2,QString("marker %1").arg(markerCount));
		placeMarker();
		
		break;
	case Qt::Key_P:
		qDebug()<<"P"<<endl;
		drawPolygon(bupt,"#ffffff",1,0.5,"#ff0000",0.5);
		drawPolygon(bnu,"#ffffff",1,0.5,"#0000ff",0.5);
		
		break;
    default:
        QWebView::keyPressEvent(event);
    }
}

void mapview::scroll( int xoffset, int yoffset )
{
    page()->mainFrame()->evaluateJavaScript(QString("map.panBy(new GSize(%1,%2));").arg(xoffset).arg(yoffset));
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
                                                    "map.checkResize();"
#ifdef _WIN32_WCE
                                                    ).arg(w/2).arg(h/2));
#else
                                                    ).arg(w).arg(h));
#endif
}

void mapview::whereami()
{
    if (loc_svc->canGetLocalLocation())
    {
        Location* loc = loc_svc->getLocalLocation();
        if (loc)
        {
            qDebug()<<"latitude: "<<loc->Latitude()<<"; longtitude: "<<loc->Longtitude();
            centerAt(loc->Latitude(), loc->Longtitude());
        }
    }
}

void mapview::placeMarker(int minZoom,int maxZoom)
{
	page()->mainFrame()->evaluateJavaScript(QString("markerManager.addMarker(tempMarker,%1,%2);").arg(minZoom).arg(maxZoom));
	//page()->mainFrame()->evaluateJavaScript(QString("GEvent.addListener(tempMarker,'click',function(){markerClicked();})"));
	page()->mainFrame()->evaluateJavaScript(QString("updateRoute();"));
	++markerCount;
}

void mapview::makeMarkerByLatLng(double lat, double lng, QString markerTitle)
{
	page()->mainFrame()->evaluateJavaScript(QString("makeMarkerByLatLng(%1,%2,'%3')").arg(lat).arg(lng).arg(markerTitle));
}

void mapview::makeMarkerByXY(int x, int y, QString markerTitle)
{
	page()->mainFrame()->evaluateJavaScript(QString("createMarkerByXY(%1,%2,'%3');").arg(x).arg(y).arg(markerTitle));
}

void mapview::drawPolygon(const QVector<LatLng> &vertex,QString strokeColor,int strokeWeight, double strokeOpacity, QString fillColor,double fillOpacity)
{
	QString cmd("map.addOverlay(new GPolygon([");
	for (int i=0;i<vertex.size();i++)
	{
		cmd += QString("new GLatLng(%1,%2),").arg(vertex[i].lat).arg(vertex[i].lng);
	}
	cmd += (QString("new GLatLng(%1,%2)").arg(vertex[0].lat).arg(vertex[0].lng));	//make sure the polygon is closed
	cmd += QString("], '%1',%2,%3,'%4',%5));").arg(strokeColor).arg(strokeWeight).arg(strokeOpacity).arg(fillColor).arg(fillOpacity);
	//qDebug()<<cmd<<endl;
	page()->mainFrame()->evaluateJavaScript(cmd);
}

void mapview::initLatLngs()
{
	bupt.push_back(LatLng(39.9652,116.3610));
	bupt.push_back(LatLng(39.9582,116.3613));
	bupt.push_back(LatLng(39.9581,116.3551));
	bupt.push_back(LatLng(39.9650,116.3548));

	bnu.push_back(LatLng(39.9678,116.3609));
	bnu.push_back(LatLng(39.9677,116.3701));
	bnu.push_back(LatLng(39.9581,116.3710));
	bnu.push_back(LatLng(39.9581,116.3613));
}

void mapview::GPSCallback( void* context, LocationSvc* svc )
{
    mapview* This = reinterpret_cast<mapview*>(context);
    qDebug()<<"New Position";
    This->whereami();
}

//---------------------------MapListener----------------------------------------

void MapListener::markerClicked()
{
	qDebug()<<"marker clicked";
}

void MapListener::mapClicked(QString s)
{
	((mapview*)parent())->page()->mainFrame()->evaluateJavaScript(QString("map.panTo(tempMarker.getLatLng());")); 
	qDebug()<<"map clicked:"<<s;
}
