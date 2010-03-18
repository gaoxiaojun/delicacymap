#ifndef MAP_VIEW_H
#define MAP_VIEW_H

#include <QPoint>
#include <QGraphicsView>
#include <QPixmap>
#include "Decorator.h"
#include "GeoCoord.h"

class ImageCache;
struct MarkerInfo;
class QGraphicsScene;
class SelfMarkerItem;
class RouteItem;
class ZoomSensitiveItem;

namespace ProtocolBuffer{
    class Restaurant;
}

class MapViewBase: public QGraphicsView{
    Q_OBJECT
public:
    MapViewBase(QWidget *parent = 0);
    ~MapViewBase();
    QSize sizeHint() const;
    void setDecorator(Decorator* decorator = 0);
    void insertDecorator(Decorator* decorator);
    void appendDecorator(Decorator* decorator);

    void lockMap();
    void unlockMap();
    bool isLocked() { return mapIsLocked; }
    void addBlockingPanel(QWidget*);
    void removeItem(ZoomSensitiveItem*);

    void setCache(ImageCache* cache);
    int getZoomLevel();
    QPoint getCoords();
    void setCoords(const QPoint& coords);
    void setGeoCoords(const GeoCoord &latitude, const GeoCoord &longitude);
    void getGeoCoords(GeoCoord& latitude, GeoCoord& longitude) const;
public slots:
    void addRestaurantMarker(const ProtocolBuffer::Restaurant*);
    RouteItem* addRoute(const QList<GeoPoint>&);
    RouteItem* addRoute(const QList<GeoPoint>&, int);
    void setSelfLocation(const GeoPoint& coord);
    void updateUserLocation(int, const GeoPoint& coord);
    void resetCoords();
    void zoomIn();
    void zoomOut();
    void zoomInAt(int x, int y);
    void zoomOutAt(int x, int y);
    void setZoomLevel(int level);
    void setZoomLevelAt(int level, int x, int y);
    void moveBy(int x, int y);
    void downloadMissingImages();
    void scheduleRepaint();
signals:
    void canZoomIn(bool status);
    void canZoomOut(bool status);
    void zoomLevelChanged(int level);
    void boundsChange(const GeoBound& newbound);
    void restaurantMarkerClicked(const ProtocolBuffer::Restaurant*);

protected:
    int xCenter;
    int yCenter;
    int zoomLevel;
    int last_xcenter, last_ycenter;

    void drawBackground(QPainter *painter, const QRectF &rect);
    int getSide() const;
    int getSideMask() const;
    int getSidePow() const;
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void leaveEvent(QEvent *event);
    void adjustCenter();
    void updateBound();
    void remapMarkers(int oldzoomlevel, int newzoomlevel);

private:
    ImageCache *images;
    QGraphicsScene *scene;
    SelfMarkerItem *self;
    Decorator decorator;
    GeoBound currentBound;
    bool handleReleaseEvent, handlePressEvent, handleDblClickEvent, handleMoveEvent;
    bool mapIsLocked;
};

#endif
