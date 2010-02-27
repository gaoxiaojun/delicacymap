#include "MarkerItem.h"
#include "CoordsHelper.h"
#include <QPainter>
#include <QRect>
#include <QGraphicsSceneMouseEvent>
#include <boost/foreach.hpp>

RouteItem::RouteItem( const QList<GeoPoint>& r, bool editable /*= false*/ )
: points(r), isEditable(editable), isEditing(false)
{
    pointEditing = -1;
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
    if (isEditing)
    {
        pen.setWidth(10);
        painter->setPen(pen);
        painter->drawPoints(sceneCoords);
    }
}

QRectF RouteItem::boundingRect() const
{
    QPoint SW = CoordsHelper::InternalGeoCoordToCoord(boundRect.SW.lat, boundRect.SW.lng, getZoom());
    QPoint NE = CoordsHelper::InternalGeoCoordToCoord(boundRect.NE.lat, boundRect.NE.lng, getZoom());
    return QRectF(
        SW.x() - Center.x() - 20,
        NE.y() - Center.y() - 20,
        NE.x() - SW.x() + 40,
        SW.y() - NE.y() + 40);
}

void RouteItem::setZoom( int zoom )
{
    if (zoom != getZoom())
    {
        ZoomSensitiveItem::setZoom(zoom);
        sceneCoords.clear();
        QPoint SW = CoordsHelper::InternalGeoCoordToCoord(boundRect.SW.lat, boundRect.SW.lng, getZoom());
        QPoint NE = CoordsHelper::InternalGeoCoordToCoord(boundRect.NE.lat, boundRect.NE.lng, getZoom());
        Center = QPoint( (SW.x() + NE.x())/2, (SW.y() + NE.y())/2 );
        BOOST_FOREACH( const GeoPoint &c, points )
        {
            sceneCoords.push_back(CoordsHelper::InternalGeoCoordToCoord(c.lat, c.lng, getZoom()) - Center);
        }
        setPos(Center);
        prepareGeometryChange();
    }
}

static inline
int ContainsPoint(const QPolygon& polygon, const QPoint& p)
{
    QPoint delta(5, 5);
    QRect rect(p - delta, p + delta);
    for (int i=0;i<polygon.size();i++)
        if (rect.contains(polygon[i]))
        {
            return i;
        }
    return -1;
}

void RouteItem::mousePressEvent( QGraphicsSceneMouseEvent *event )
{
    if (isEditing)
    {
        QPoint point = event->scenePos().toPoint() - Center;
        pointEditing = ContainsPoint(sceneCoords, point);
    }
}

void RouteItem::mouseReleaseEvent( QGraphicsSceneMouseEvent * )
{
    pointEditing = -1;
}

void RouteItem::mouseMoveEvent( QGraphicsSceneMouseEvent *event )
{
    if (isEditing && pointEditing != -1)
    {
        QPoint modifiedPoint = event->scenePos().toPoint();
        bool boundChanged = false;
        sceneCoords[pointEditing] = modifiedPoint - Center;
        CoordsHelper::InternalCoordToGeoCoord(modifiedPoint, getZoom(), points[pointEditing].lat, points[pointEditing].lng);

        // Adjusting bounds
        GeoPoint geop;
        CoordsHelper::InternalCoordToGeoCoord(modifiedPoint, getZoom(), geop.lat, geop.lng);
        if (geop.lat < boundRect.SW.lat)
            boundRect.SW.lat = geop.lat, boundChanged = true;
        if (geop.lng < boundRect.SW.lng)
            boundRect.SW.lng = geop.lng, boundChanged = true;
        if (boundRect.NE.lat < geop.lat)
            boundRect.NE.lat = geop.lat, boundChanged = true;
        if (boundRect.NE.lng < geop.lng)
            boundRect.NE.lng = geop.lng, boundChanged = true;
        if (boundChanged)
            this->prepareGeometryChange();
        update();
    }
}

void RouteItem::mouseDoubleClickEvent( QGraphicsSceneMouseEvent *event )
{
    bool isdeletingpoint = false;
    if (isEditing && sceneCoords.size() > 2)
    {
        QPoint point = event->scenePos().toPoint() - Center;
        int pointdeleting = ContainsPoint(sceneCoords, point);
        if (pointdeleting != -1)
        {
            sceneCoords.remove(pointdeleting);
            points.removeAt(pointdeleting);
            isdeletingpoint = true;
        }
    }
    if (!isdeletingpoint)
    {
        isEditing = !isEditing;
        pointEditing = -1;
    }
    update();
}
