#pragma once

#include <QGraphicsItem>

namespace ProtocolBuffer{
    class Restaurant;
}

class QPixmap;

class RestaurantMarkerItem : public QGraphicsItem
{
public:
    RestaurantMarkerItem(const ProtocolBuffer::Restaurant* restaurant) : r(restaurant){}
    const ProtocolBuffer::Restaurant* restaurantInfo() const { return r; }
protected:
    void paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *);
    QRectF boundingRect() const;
    int type() const { return UserType + 1000; }

    static QPixmap& markerImage();

private:
    const ProtocolBuffer::Restaurant* r;
};
