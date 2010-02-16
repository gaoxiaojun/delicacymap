#pragma once

#include "GeoCoord.h"
#include <QGraphicsItem>
#include <QList>
#include <QPoint>

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

class RouteItem : public QGraphicsItem
{
public:
    RouteItem(const QList<GeoPoint>& r, bool editable = false);
    void changeZoom(int zoom);
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */);
    QRectF boundingRect() const;
    int type() const { return UserType + 1001; }

    QList<GeoPoint> points;
    QList<QPoint> sceneCoords;
    GeoBound boundRect;
    int currentZoomlevel;
    bool isEditable;
};