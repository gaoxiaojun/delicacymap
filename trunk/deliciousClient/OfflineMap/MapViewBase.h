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
public slots:
    void addRestaurantMarker(const ProtocolBuffer::Restaurant*);
    void addRoute(const QList<GeoPoint>&);
    void resetCoords();
    void zoomIn();
    void zoomOut();
    void zoomInAt(int x, int y);
    void zoomOutAt(int x, int y);
    void setZoomLevel(int level);
    void setZoomLevelAt(int level, int x, int y);
    void moveBy(int x, int y);
    void setCache(ImageCache* cache);
    void downloadMissingImages();
    void scheduleRepaint();
    int getZoomLevel();
    QPoint getCoords();
    void setCoords(const QPoint& coords);
    void setGeoCoords(const GeoCoord &latitude, const GeoCoord &longitude);
    void getGeoCoords(GeoCoord& latitude, GeoCoord& longitude) const;
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
    Decorator decorator;
    GeoBound currentBound;
    bool shouldProcessReleaseEvent;
};

#endif
