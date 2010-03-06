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
    return UserMarkerItem::UserIcon();
}
