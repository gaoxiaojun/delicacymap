#pragma once

#include "GeoCoord.h"
#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsProxyWidget>
#include <QList>
#include <QPolygon>
#include <QPoint>

namespace ProtocolBuffer{
    class Restaurant;
}

class QPixmap;
class MapViewBase;

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

class PanelWidget : public QGraphicsProxyWidget
{
    Q_OBJECT
public:
    //enum { Type = UserType + 2000 };
    PanelWidget(MapViewBase*, QGraphicsItem* parent=NULL, Qt::WindowFlags wFlags=0);
    void setWidget(QWidget *widget, ZoomSensitiveItem* balloonOn=NULL);
    void paintWindowFrame(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
private slots:
    void handleWidgetDestroyed(QObject*);
protected:
    //int type() const { return Type; }
    void resizeEvent(QGraphicsSceneResizeEvent *event);
   QRectF boundingRect() const;
private:
    MapViewBase* target;
    ZoomSensitiveItem *balloonTarget;
};

class RestaurantMarkerItem : public ZoomSensitiveItem
{
public:
    enum { Type = UserType + 1000 };
    RestaurantMarkerItem(const ProtocolBuffer::Restaurant* restaurant) : r(restaurant){}
    const ProtocolBuffer::Restaurant* restaurantInfo() const { return r; }
protected:
    void paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *);
    QRectF boundingRect() const;
    int type() const { return Type; }

    static QPixmap& markerImage();

private:
    const ProtocolBuffer::Restaurant* r;
};

class RouteItem : public QObject, public ZoomSensitiveItem
{
    Q_OBJECT
public:
    enum { Type = UserType + 1001 };
    RouteItem(const QList<GeoPoint>& r, bool editable = false);
    void setZoom(int zoom);
    QList<GeoPoint> getRoute() const {return points;}
    void setRouteReceiverWhenDoneEditing(int);
    int getRouteReceiverWhenDoneEditing() { return targetUser; }

signals:
    void EditFinished(RouteItem*);
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */);
    QRectF boundingRect() const;
    int type() const { return Type; }
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

    QList<GeoPoint> points;
    QPolygon sceneCoords;
    GeoBound boundRect;
    QPoint Center;
    int pointEditing, targetUser;
    bool isEditable, isEditing;
};

class UserMarkerItem : public ZoomSensitiveItem
{
public:
    enum { Type = UserType + 1002 };
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */);
    QRectF boundingRect() const;
    int type() const { return Type; }
    virtual const QPixmap& UserIcon() const;
    static const QPixmap& defaultUserIcon();
};

class SelfMarkerItem : public UserMarkerItem
{
public:
    enum { Type = UserType + 1003 };
protected:
    const QPixmap& UserIcon() const;
    int type() const { return Type; }
};
