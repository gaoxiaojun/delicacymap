#ifndef MAP_VIEW_H
#define MAP_VIEW_H

#include <QPoint>
#include <QGraphicsView>
#include <QPixmap>
#include <QMap>
#include <QTimeLine>
#include "Decorator.h"
#include "GeoCoord.h"

class ImageCache;
struct MarkerInfo;
class QGraphicsScene;
class SelfMarkerItem;
class RouteItem;
class ZoomSensitiveItem;
class RestaurantMarkerItem;
class UserMarkerItem;
class PanelWidget;
class TipPanel;

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
    QRect exposedView() const;
    void ensureVisible(const QGraphicsItem *item, int xmargin=30, int ymargin=30);// shadow base class methods
    void panBy(QPoint);

    void lockMap();
    void unlockMap();
    bool isLocked() { return mapIsLocked; }
    // if balloonOn is NULL, the panel is placed in the center
    PanelWidget* addBlockingPanel(QWidget*, ZoomSensitiveItem* balloonOn = NULL);
    void showTip(QWidget*);
    void removeItem(ZoomSensitiveItem*);
    void addLocalMarker(ZoomSensitiveItem*);
    void removeFromLocal(ZoomSensitiveItem*);
    bool hasLocalMarker() const;
    QList<ZoomSensitiveItem*> localMarkers() const;

    void setCache(ImageCache* cache);
    int getZoomLevel();
    QPoint getCoords();
    void centerOn(int x, int y);
    void centerOn(QPoint p);
    void centerOn(QGraphicsItem* item, bool ignoreLockState=false);
    void setCoords(const QPoint& coords);
    void setGeoCoords(const GeoCoord &latitude, const GeoCoord &longitude);
    GeoPoint getGeoCenter() const;

    RestaurantMarkerItem* getRestaurantMarker(int rid);
public slots:
    void addRestaurantMarker(const ProtocolBuffer::Restaurant*);
    RouteItem* addRoute(const QList<GeoPoint>&);
    RouteItem* addRoute(const QList<GeoPoint>&, int);
    void setSelfLocation(const InaccurateGeoPoint& coord);
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
    void restaurantMarkerClicked(RestaurantMarkerItem*);

protected:
    int xCenter;
    int yCenter;
    int zoomLevel;
    int last_xcenter, last_ycenter;

    void addRoute(RouteItem*);
    void drawBackground(QPainter *painter, const QRectF &rect);
    void invalidateBackground();
    int getSide() const;
    int getSideMask() const;
    int getSidePow() const;
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
#ifndef QT_NO_WHEELEVENT
    void wheelEvent(QWheelEvent *event);
#endif
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void leaveEvent(QEvent *event);
    void resizeEvent(QResizeEvent *event);
    void adjustCenter();
    void updateBound();
    void remapMarkers(int oldzoomlevel, int newzoomlevel);

private slots:
    void removeCurrentTip();
    void showNextTip();
    void panMapXhandler(qreal);
    void panMapStateChange(QTimeLine::State);

private:
    QPixmap backgroundCache;
    QRect backgroundRect;
    ImageCache *images;
    QGraphicsScene *scene;
    SelfMarkerItem *self;
    RouteItem *singleRouteItem;
    QTimeLine panMapTimeline/*, bounceItemTimeline*/;
    QPoint _panOldCenter, _panDelta;
    Decorator decorator;
    GeoBound currentBound;
    QList<ZoomSensitiveItem*> localItems;
    QMap<int, UserMarkerItem*> users;
    QList<TipPanel*> tipPanels;
    bool mapIsLocked;
};

#endif
