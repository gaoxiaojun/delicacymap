#include "MapDecorators.h"
#include <QMouseEvent> 
#include <QTimerEvent>
#include <QPainter>
#include "MapViewBase.h"
#include "GeoCoord.h"
#include <stdio.h>

MapDecorator::MapDecorator(MapViewBase* mapView)
:target(mapView){
}

MoveDecorator::MoveDecorator(MapViewBase* mapView, bool smooth)
:MapDecorator(mapView), dragging(false)
{
    timerId = -1;
    smooth_enabled = false;
    EnableSmoothMovement(smooth);
}

void MoveDecorator::EnableSmoothMovement(bool enable)
{
    if (!enable && timerId != -1)
        this->killTimer(timerId);

    smooth_enabled = enable;
}

void MoveDecorator::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        if (timerId != -1)
        {
            this->killTimer(timerId);
            timerId = -1;
        }
        m_mouse_pos = event->pos();
        dragging = true;
        m_speed = QPointF();
    }
    MapDecorator::mousePressEvent(event);
}

void MoveDecorator::mouseReleaseEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) == 0)
    {
        dragging = false;
        timerId = this->startTimer(1000/35);
    }
    MapDecorator::mouseReleaseEvent(event);
}

void MoveDecorator::mouseMoveEvent(QMouseEvent *event)
{
    if (dragging)
    {
        QPoint delta = m_mouse_pos - event->pos();
        m_mouse_pos = event->pos();
        target->moveBy(delta.x(), delta.y());
        m_speed = delta;
    }
    else
        MapDecorator::mouseMoveEvent(event);
}

void MoveDecorator::timerEvent( QTimerEvent *e )
{
    if (!dragging && m_speed.manhattanLength() > 3.0)
    {
        m_speed *= 0.85;
        target->moveBy(m_speed.x(), m_speed.y());
    }
    else
    {
        this->killTimer(timerId);
        timerId = -1;
    }
}

ZoomDecorator::ZoomDecorator(MapViewBase *mapView)
:MapDecorator(mapView)
{
}

void ZoomDecorator::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton)
        target->zoomInAt(event->x(), event->y());
    if (event->buttons() & Qt::RightButton)
        target->zoomOutAt(event->x(), event->y());
}

void DownloadDecorator::mousePressEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton)&&(event->buttons() & Qt::RightButton))
        target->downloadMissingImages();
    MapDecorator::mousePressEvent(event);
}

DownloadDecorator::DownloadDecorator(MapViewBase *mapView)
:MapDecorator(mapView)
{
}

void CrossDecorator::paintEvent(QPainter &painter)
{
    if (isEnabled)
    {
        QRect window = painter.window();
        painter.setPen(QColor(255, 0, 0));
        int halfWidth = (window.left()+window.right())/2;
        int halfHeight = (window.top() + window.bottom())/2;
        painter.drawLine(window.left(), halfHeight, window.right(), halfHeight);
        painter.drawLine(halfWidth, window.top(), halfWidth, window.bottom());
    }
    MapDecorator::paintEvent(painter);
}

CrossDecorator::CrossDecorator(MapViewBase* mapView)
:MapDecorator(mapView), isEnabled(true)
{
}

bool CrossDecorator::enabled()
{
    return isEnabled;
}

void CrossDecorator::enable(bool newValue)
{
    if (isEnabled != newValue)
    {
        isEnabled = newValue;
        emit stateChanged();
    }
}

CoordsDecorator::CoordsDecorator(MapViewBase* mapView)
:MapDecorator(mapView), isEnabled(true)
{
}

bool CoordsDecorator::enabled()
{
    return isEnabled;
}

void CoordsDecorator::enable(bool value)
{
    if (isEnabled != value)
    {
        isEnabled = value;
        emit stateChanged();
    }
}

void CoordsDecorator::paintEvent(QPainter &painter)
{
    if (isEnabled)
    {
        QRect window = painter.window();
        painter.setPen(QColor(255, 0, 0));
        QString str;
        QPoint coords = target->getCoords();
        GeoPoint center = target->getGeoCenter();
        str += QString("coordinates:\n    x: %1\n    y: %2\ngeo coordinates:\n   lat: %4\n   lng: %5\nzoom level: %3\n")
            .arg(coords.x()).arg(coords.y())
            .arg(target->getZoomLevel())
            .arg(center.lat.getDouble())
            .arg(center.lng.getDouble());

        QRect rect(
            window.left() + 10,
            window.top() + 10,
            window.width() - 20,
            window.height() - 20
        );
		
        painter.drawText(rect, str);
    }

    MapDecorator::paintEvent(painter);
}
