/*
QGMView - Qt Google Map Viewer
Copyright (C) 2007  Victor Eremin

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

You can contact author using following e-mail addreses
erv255@googlemail.com 
ErV2005@rambler.ru
*/
#ifndef MAP_VIEW_H
#define MAP_VIEW_H
#include <QWidget>
#include <QPoint>
#include "Decorator.h"
#include "ImageCache.h"
#include "GeoCoord.h"
class QImage;

class MapViewBase: public QWidget{
    Q_OBJECT
public:
    MapViewBase(QWidget *parent = 0);
    ~MapViewBase();
    QSize sizeHint() const;
    void setDecorator(Decorator* decorator = 0);
    void insertDecorator(Decorator* decorator);
    void appendDecorator(Decorator* decorator);
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
protected:
    enum {MaxZoomLevel = 17, TilePower2 = 8, TileSize=256};
    int xCenter;
    int yCenter;
    int zoomLevel;
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
    void paintEvent(QPaintEvent *event);
private:
    ImageCache *images;
    Decorator decorator;
};

#endif
