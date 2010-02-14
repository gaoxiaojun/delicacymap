#include "MarkerItem.h"
#include <QPainter>
#include <QPixmap>

void RestaurantMarkerItem::paint( QPainter *p, const QStyleOptionGraphicsItem *, QWidget * )
{
    p->drawEllipse(0, 0, 3, 3);
    p->drawPixmap(-markerImage().width()/2, -markerImage().height(), markerImage());
}

QRectF RestaurantMarkerItem::boundingRect() const
{
    return QRectF(-markerImage().width()/2, -markerImage().height(), markerImage().width(), markerImage().height()+3);
}

QPixmap& RestaurantMarkerItem::markerImage()
{
    static QPixmap image(":/Icons/marker.png");
    return image;
}
