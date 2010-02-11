#ifndef MAP_VIEW_H
#define MAP_VIEW_H
#include <QWidget>
#include <QPoint>
#include "Decorator.h"
#include "GeoCoord.h"

class QImage;
class ImageCache;
class MarkerCache;

class MapViewBase: public QWidget{
    Q_OBJECT
public:
    MapViewBase(QWidget *parent = 0);
    ~MapViewBase();
    QSize sizeHint() const;
    void setDecorator(Decorator* decorator = 0);
    void insertDecorator(Decorator* decorator);
    void appendDecorator(Decorator* decorator);
    void setMarkerCache(MarkerCache* cache) { markers = cache; }
public slots:
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

protected:
    enum {MaxZoomLevel = 17, TilePower2 = 8, TileSize=256};
    int xCenter;
    int yCenter;
    int zoomLevel;
    int last_xcenter, last_ycenter;

    QPoint InternalGeoCoordToCoord(const GeoCoord& latitude, const GeoCoord& longitude) const;
    void InternalCoordToGeoCoord(QPoint coord, GeoCoord &latitude, GeoCoord &longitude) const;

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
    void paintEvent(QPaintEvent *event);
private:
    ImageCache *images;
    MarkerCache *markers;
    Decorator decorator;
    GeoBound currentBound;
};

#endif
