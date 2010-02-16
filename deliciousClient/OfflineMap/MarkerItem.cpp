#include "MarkerItem.h"
#include "CoordsHelper.h"
#include "MapProtocol.pb.h"
#include <QPainter>
#include <QPixmap>
#include <boost/foreach.hpp>

void RestaurantMarkerItem::paint( QPainter *p, const QStyleOptionGraphicsItem *, QWidget * )
{
    p->drawEllipse(0, 0, 3, 3);
    p->drawPixmap(-markerImage().width()/2, -markerImage().height(), markerImage());
    p->drawText(-10, 10, QString::fromUtf8(restaurantInfo()->name().c_str()));
}

QRectF RestaurantMarkerItem::boundingRect() const
{
    return QRectF(-markerImage().width()/2, -markerImage().height(), markerImage().width(), markerImage().height());
}

QPixmap& RestaurantMarkerItem::markerImage()
{
    static QPixmap image(":/Icons/marker.png");
    return image;
}

void RouteItem::paint( QPainter *, const QStyleOptionGraphicsItem *, QWidget */* = 0 */ )
{

}

QRectF RouteItem::boundingRect() const
{
    QPoint sw = CoordsHelper::InternalGeoCoordToCoord(boundRect.SW.lat, boundRect.SW.lng, currentZoomlevel);
    QPoint ne = CoordsHelper::InternalGeoCoordToCoord(boundRect.NE.lat, boundRect.NE.lng, currentZoomlevel);
    return QRectF(
        sw.x(),
        ne.y(),
        ne.x() - sw.x(),
        ne.y() - sw.y());
}

void RouteItem::changeZoom( int zoom )
{
    if (zoom != currentZoomlevel)
    {
        currentZoomlevel = zoom;
        BOOST_FOREACH( const GeoPoint &c, points )
        {

        }
        prepareGeometryChange();
    }
}

RouteItem::RouteItem( const QList<GeoPoint>& r, bool editable /*= false*/ )
: points(r), isEditable(editable)
{
    boundRect.SW = points.first();
    boundRect.NE = points.first();
    BOOST_FOREACH( const GeoPoint &c, points )
    {
        if (c.lat < boundRect.SW.lat)
            boundRect.SW.lat = c.lat;
        if (c.lng < boundRect.SW.lng)
            boundRect.SW.lng = c.lng;
        if (boundRect.NE.lat < c.lat)
            boundRect.NE.lat = c.lat;
        if (boundRect.NE.lng < c.lng)
            boundRect.NE.lng = c.lng;
    }
}
