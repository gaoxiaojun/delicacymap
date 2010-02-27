#include "MarkerItem.h"
#include "CoordsHelper.h"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <boost/foreach.hpp>

RouteItem::RouteItem( const QList<GeoPoint>& r, bool editable /*= false*/ )
: points(r), isEditable(editable), isEditing(false)
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

void RouteItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *, QWidget * /* = 0 */ )
{
    QPen pen;
    if (isEditing)
        pen.setColor(QColor(255, 40, 40));
    else
        pen.setColor(QColor(25, 255, 50));
    pen.setWidth(3);
    painter->setPen(pen);
    painter->drawPolyline(sceneCoords);
    painter->drawRect(boundingRect());
}

QRectF RouteItem::boundingRect() const
{
    QPoint SW = CoordsHelper::InternalGeoCoordToCoord(boundRect.SW.lat, boundRect.SW.lng, getZoom());
    QPoint NE = CoordsHelper::InternalGeoCoordToCoord(boundRect.NE.lat, boundRect.NE.lng, getZoom());
    QPoint Center( (SW.x() + NE.x())/2, (SW.y() + NE.y())/2 );
    return QRectF(
        SW.x() - Center.x() - 10,
        NE.y() - Center.y() - 10,
        NE.x() - SW.x() + 20,
        SW.y() - NE.y() + 20);
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

void RouteItem::mousePressEvent( QGraphicsSceneMouseEvent *event )
{
    QPoint SW = CoordsHelper::InternalGeoCoordToCoord(boundRect.SW.lat, boundRect.SW.lng, getZoom());
    QPoint NE = CoordsHelper::InternalGeoCoordToCoord(boundRect.NE.lat, boundRect.NE.lng, getZoom());
    QPoint Center( (SW.x() + NE.x())/2, (SW.y() + NE.y())/2 );
    QPoint point = event->scenePos().toPoint() - Center;
    if (sceneCoords.contains(point))
    {

    }
}

void RouteItem::mouseReleaseEvent( QGraphicsSceneMouseEvent *event )
{

}

void RouteItem::mouseMoveEvent( QGraphicsSceneMouseEvent *event )
{
    if (isEditing)
    {

    }
}

void RouteItem::mouseDoubleClickEvent( QGraphicsSceneMouseEvent *event )
{
    isEditing = !isEditing;
}
