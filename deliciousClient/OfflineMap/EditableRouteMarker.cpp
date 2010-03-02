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

static inline
bool ValueInBetween(int val, int target1, int target2)
{
    if (abs(target1 - target2) < 5) // special case straight lines
    {
        if (target1 < target2)
        {
            target1 -= 3;
            target2 += 3;
        }
        else
        {
            target1 += 3;
            target2 -= 3;
        }
    }
    return (target1 <= val && val <= target2) || (target2 <= val && val <= target1);
}

static inline
bool PointOnLine(const QPoint& lineStart, const QPoint& lineEnd, const QPoint& p)
{
    if (ValueInBetween(p.x(), lineStart.x(), lineEnd.x()) && ValueInBetween(p.y(), lineStart.y(), lineEnd.y()))
    {
        int a1 = (p.y() - lineStart.y()) * (lineEnd.x() - lineStart.x());
        int a2 = (lineEnd.y() - lineStart.y()) * (p.x() - lineStart.x());
        int y = (p.x() - lineStart.x()) * (lineEnd.y() - lineStart.y()) / (lineEnd.x() - lineStart.x()) + lineStart.y();
        return abs( y - p.y() ) < abs(y * 0.08);
    }
    return false;
}

void RouteItem::mouseDoubleClickEvent( QGraphicsSceneMouseEvent *event )
{
    if (isEditable)
    {
        QPoint mousePoint = event->scenePos().toPoint() - Center;
        bool endEditing = true;
        if (isEditing && sceneCoords.size() > 2)
        {
            int pointdeleting = ContainsPoint(sceneCoords, mousePoint);
            if (pointdeleting != -1)
            {
                sceneCoords.remove(pointdeleting);
                points.removeAt(pointdeleting);
                endEditing = false;
            }
        }
        if (isEditing && endEditing)
        {
            int pointCnt = sceneCoords.size();
            for (int i=0;i<pointCnt-1;++i)
            {
                const QPoint lineStart = sceneCoords[i];
                const QPoint lineEnd = sceneCoords[i+1];
                if (PointOnLine(lineStart, lineEnd, mousePoint))
                {
                    // This would invalidate all iterators, be careful!!!
                    sceneCoords.insert(i+1, mousePoint);
                    GeoPoint newPoint;
                    CoordsHelper::InternalCoordToGeoCoord(mousePoint + Center, getZoom(), newPoint.lat, newPoint.lng);
                    points.insert(i+1, newPoint);
                    endEditing = false;
                    break;
                }
            }
        }
        if (endEditing)
        {
            isEditing = !isEditing;
            pointEditing = -1;
        }
        update();
    }
}
