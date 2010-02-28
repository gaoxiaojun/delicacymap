#pragma once

#include "GeoCoord.h"
#include <QGraphicsItem>
#include <QList>
#include <QPolygon>
#include <QPoint>

namespace ProtocolBuffer{
    class Restaurant;
}

class QPixmap;

class ZoomSensitiveItem : public QGraphicsItem
{
public:
    ZoomSensitiveItem() : location(0., 0.), currentZoom(-1){}
    void setPos(const GeoPoint& center);
    using QGraphicsItem::setPos;
    const GeoPoint& getPos() {return location;};
    virtual void setZoom(int zoom);
    int getZoom() const {return currentZoom;}
private:
    GeoPoint location;
    int currentZoom;
};

class RestaurantMarkerItem : public ZoomSensitiveItem
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

class RouteItem : public ZoomSensitiveItem
{
public:
    RouteItem(const QList<GeoPoint>& r, bool editable = false);
    void setZoom(int zoom);
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */);
    QRectF boundingRect() const;
    int type() const { return UserType + 1001; }
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

    QList<GeoPoint> points;
    QPolygon sceneCoords;
    GeoBound boundRect;
    QPoint Center;
    int pointEditing;
    bool isEditable, isEditing;
};

class UserMarkerItem : public ZoomSensitiveItem
{
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */);
    QRectF boundingRect() const;
    int type() const { return UserType + 1002; }
    virtual const QPixmap& UserIcon() const;
    static const QPixmap& defaultUserIcon();
};

class SelfMarkerItem : public UserMarkerItem
{
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */);
    int type() const { return UserType + 1003; }
};