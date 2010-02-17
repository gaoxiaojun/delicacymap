#include "MarkerItem.h"
#include "CoordsHelper.h"
#include "MapProtocol.pb.h"
#include <QPainter>
#include <QPixmap>
#include <boost/foreach.hpp>

void ZoomSensitiveItem::setZoom( int zoom )
{
    currentZoom = zoom;
    setPos(getPos());
}

void ZoomSensitiveItem::setPos( const GeoPoint& center )
{
    location = center;
    setPos(CoordsHelper::InternalGeoCoordToCoord(location.lat, location.lng, getZoom()));
}

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

void RouteItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *, QWidget * /* = 0 */ )
{
    painter->drawPolyline(sceneCoords);
}

QRectF RouteItem::boundingRect() const
{
    QPoint sw = CoordsHelper::InternalGeoCoordToCoord(boundRect.SW.lat, boundRect.SW.lng, getZoom());
    QPoint ne = CoordsHelper::InternalGeoCoordToCoord(boundRect.NE.lat, boundRect.NE.lng, getZoom());
    return QRectF(
        sw.x(),
        ne.y(),
        ne.x() - sw.x(),
        ne.y() - sw.y());
}

void RouteItem::setZoom( int zoom )
{
    if (zoom != getZoom())
    {
        ZoomSensitiveItem::setZoom(zoom);
        sceneCoords.clear();
        BOOST_FOREACH( const GeoPoint &c, points )
        {
            sceneCoords.push_back(CoordsHelper::InternalGeoCoordToCoord(c.lat, c.lng, getZoom()));
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

void UserMarkerItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */ )
{

}

QRectF UserMarkerItem::boundingRect() const
{
    return QRectF();
}

const QPixmap& UserMarkerItem::defaultUserIcon()
{
    static QPixmap image(":/Icons/arrow.png");
    return image;
}

const QPixmap& UserMarkerItem::UserIcon()
{
    return defaultUserIcon();
}

void SelfMarkerItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */ )
{

}
