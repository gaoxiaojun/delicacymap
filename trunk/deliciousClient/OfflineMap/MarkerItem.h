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
    virtual ~ZoomSensitiveItem() {}
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
    enum { Type = UserType + 2000 };
    PanelWidget(MapViewBase*, QGraphicsItem* parent=NULL, Qt::WindowFlags wFlags=0);
    void setWidget(QWidget *widget, ZoomSensitiveItem* balloonOn=NULL);
    void paintWindowFrame(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    ZoomSensitiveItem* tiedTo() const { return balloonTarget; }
private slots:
    void handleWidgetDestroyed(QObject*);
    void handleCloseButtonClick();
protected:
    int type() const { return Type; }
    void resizeEvent(QGraphicsSceneResizeEvent *event);
    void retieToTarget();
    QGraphicsProxyWidget* setupCloseButton();
private:
    MapViewBase* target;
    ZoomSensitiveItem *balloonTarget;
    QGraphicsProxyWidget *closeButton;
};

class RestaurantMarkerItem : public ZoomSensitiveItem
{
public:
    enum { Type = UserType + 1000 };
    // when a NULL pointer is passed for restaurant, the marker is a local marker that can be edited
    RestaurantMarkerItem(const ProtocolBuffer::Restaurant* restaurant = NULL) : r(restaurant), isLocalMarker(!r){}
    ~RestaurantMarkerItem();
    bool isFakeMarker() const { return isLocalMarker; }
    const ProtocolBuffer::Restaurant* restaurantInfo() const { return r; }
    ProtocolBuffer::Restaurant* mutableRestaurantInfo(); // if the item is a local marker, this will return a correctly initailized internal object, otherwise, always return NULL
    QRectF boundingRect() const;
    void PromoteToRealMarker(const ProtocolBuffer::Restaurant* restaurant);
protected:
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *);
    int type() const { return Type; }

    static QPixmap& markerImage();
    static QPixmap& fakeMarkerImage();

private:
    const ProtocolBuffer::Restaurant* r;
    bool isLocalMarker;
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
