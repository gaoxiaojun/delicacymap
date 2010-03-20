#include "MarkerItem.h"
#include "CoordsHelper.h"
#include "MapProtocol.pb.h"
#include "OfflineMap/MapViewBase.h"
#include <QGraphicsSceneResizeEvent>
#include <QPainter>
#include <QPixmap>
#include <QWidget>
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

void UserMarkerItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *, QWidget * /* = 0 */ )
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

const QPixmap& SelfMarkerItem::UserIcon() const
{
    static QPixmap image(":/Icons/selfMarker.png");
    return image;
}

PanelWidget::PanelWidget(MapViewBase *map, QGraphicsItem* parent, Qt::WindowFlags wFlags)
    : QGraphicsProxyWidget(parent, wFlags)
{
    target = map;
    balloonTarget = NULL;
}

void PanelWidget::setWidget(QWidget *widget, ZoomSensitiveItem *balloonOn)
{
    connect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(handleWidgetDestroyed(QObject*)));
    if (balloonOn)
    {
        balloonTarget = balloonOn;
    }
    QGraphicsProxyWidget::setWidget(widget);
}

void PanelWidget::handleWidgetDestroyed(QObject*)
{
    target->unlockMap();
    this->deleteLater();
}

QRectF PanelWidget::boundingRect() const
{
    QRectF rect = QGraphicsProxyWidget::boundingRect();
    if (balloonTarget)
        rect.adjust(0., 0., 0., 10.);
    return rect;
}

void PanelWidget::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    QPointF p = target->mapToScene(target->width(), target->height());
    if (this->y() + event->newSize().height() > p.y())
    {
        this->setPos(this->pos().x(), this->pos().y() - (this->y() + event->newSize().height() - p.y()) - 10);
    }
    QGraphicsProxyWidget::resizeEvent(event);
}

void PanelWidget::paintWindowFrame(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (balloonTarget)
    {
        painter->drawEllipse(this->widget()->width()/2, this->widget()->height()+5,10, 10);
    }
    QGraphicsProxyWidget::paintWindowFrame(painter, option, widget);
}
