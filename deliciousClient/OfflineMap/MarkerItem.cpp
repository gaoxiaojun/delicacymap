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
    if (getZoom() != -1)
        setPos(CoordsHelper::InternalGeoCoordToCoord(location.lat, location.lng, getZoom()));
}

void RestaurantMarkerItem::paint( QPainter *p, const QStyleOptionGraphicsItem *, QWidget * )
{
    p->drawPixmap(-markerImage().width()/2, -markerImage().height(), markerImage());
    p->drawText(-10, 10, QString::fromUtf8(restaurantInfo()->name().c_str()));
}

QRectF RestaurantMarkerItem::boundingRect() const
{
    return QRectF(-markerImage().width()/2, -markerImage().height(), markerImage().width(), markerImage().height() + 5);
}

QPixmap& RestaurantMarkerItem::markerImage()
{
    static QPixmap image(":/Icons/marker.png");
    return image;
}

void RouteItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *, QWidget * /* = 0 */ )
{
    QPen pen(QColor(25, 255, 50));
    pen.setWidth(3);
    painter->setPen(pen);
    painter->drawPolyline(sceneCoords);
}

QRectF RouteItem::boundingRect() const
{
    QPoint SW = CoordsHelper::InternalGeoCoordToCoord(boundRect.SW.lat, boundRect.SW.lng, getZoom());
    QPoint NE = CoordsHelper::InternalGeoCoordToCoord(boundRect.NE.lat, boundRect.NE.lng, getZoom());
    QPoint Center( (SW.x() + NE.x())/2, (SW.y() + NE.y())/2 );
    return QRectF(
        SW.x() - Center.x(),
        NE.y() - Center.y(),
        NE.x() - SW.x(),
        SW.y() - NE.y());
}

void RouteItem::setZoom( int zoom )
{
    if (zoom != getZoom())
    {
        ZoomSensitiveItem::setZoom(zoom);
        sceneCoords.clear();
        QPoint SW = CoordsHelper::InternalGeoCoordToCoord(boundRect.SW.lat, boundRect.SW.lng, getZoom());
        QPoint NE = CoordsHelper::InternalGeoCoordToCoord(boundRect.NE.lat, boundRect.NE.lng, getZoom());
        QPoint Center( (SW.x() + NE.x())/2, (SW.y() + NE.y())/2 );
        BOOST_FOREACH( const GeoPoint &c, points )
        {
            sceneCoords.push_back(CoordsHelper::InternalGeoCoordToCoord(c.lat, c.lng, getZoom()) - Center);
        }
        setPos(Center);
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
    const QPixmap& image = UserIcon();
    painter->drawPixmap(-image.width()/2, -image.height(), image);
}

QRectF UserMarkerItem::boundingRect() const
{
    const QPixmap& image = UserIcon();
    return QRectF(-image.width()/2, -image.height(), image.width(), image.height());
}

const QPixmap& UserMarkerItem::defaultUserIcon()
{
    static QPixmap image(":/Icons/arrow.png");
    return image;
}

const QPixmap& UserMarkerItem::UserIcon() const
{
    return defaultUserIcon();
}

void SelfMarkerItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */ )
{

}
