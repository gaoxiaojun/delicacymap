#include <QPainter>
#include <QPaintEvent>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsItemGroup>
#include <QTimer>
#include <QDebug>
#include <QtGui>
#include <math.h>
#include "MapViewBase.h"
#include "ImageCache.h"
#include "GeoCoord.h"

#include <boost/foreach.hpp>

static const GeoCoord maxLatitude(90, 0, 0, 0);//(85.0511287798066);//(90, 0, 0, 0);
static const GeoCoord maxLongitude(180, 0, 0, 0);

MapViewBase::MapViewBase(QWidget *parent)
:QGraphicsView(parent), xCenter(128), yCenter(128), zoomLevel(0), images(0), markerImage(":/Icons/marker.png")
{
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    last_xcenter = xCenter;
    last_ycenter = yCenter;
    scene = new QGraphicsScene;
    setScene(scene);
    centerOn(xCenter, yCenter);
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setAttribute(Qt::WA_OpaquePaintEvent);// a small optimization
}

MapViewBase::~MapViewBase(){
}

void MapViewBase::setCache(ImageCache* imageCache){
    images = imageCache;
}

template <typename T> T min(const T arg1, const T arg2){
    return (arg1 < arg2)?arg1: arg2;
}

template <typename T> T max(const T arg1, const T arg2){
    return (arg1 > arg2)?arg1: arg2;
}

void MapViewBase::zoomIn(){
    setZoomLevel(zoomLevel + 1);
}

void MapViewBase::zoomOut(){
    setZoomLevel(zoomLevel - 1);
}

void MapViewBase::zoomInAt(int x, int y){
    setZoomLevelAt(zoomLevel+1, x, y);
}

void MapViewBase::zoomOutAt(int x, int y){
    setZoomLevelAt(zoomLevel-1, x, y);
}

inline int fitToPow2(int num, int power){
    int val = 1 << power;
    int mask = val - 1;
    if (num < 0)
        num = val - ((-num)&mask);		
    return num & mask;
}

inline int remapToPow2(int num, int oldPow, int newPow){
    return (oldPow > newPow) ? 
        num >> (oldPow - newPow):
    num << (newPow - oldPow); 
} 

void MapViewBase::adjustCenter(){
    xCenter = fitToPow2(xCenter, zoomLevel+TilePower2);
    yCenter = fitToPow2(yCenter, zoomLevel+TilePower2);
}

void MapViewBase::setZoomLevel(int level){
    if ((level >= 0) && (level <= MaxZoomLevel)){
        if (zoomLevel != level){
            adjustCenter();
            xCenter = remapToPow2(xCenter, zoomLevel, level);
            yCenter = remapToPow2(yCenter, zoomLevel, level);
            remapMarkers(zoomLevel, level);
            zoomLevel = level;
            emit zoomLevelChanged(zoomLevel);
            updateBound();
            scene->setSceneRect(0, 0, 1<<(zoomLevel+TilePower2), 1<<(zoomLevel+TilePower2));
            centerOn(xCenter, yCenter);
            repaint();
        }
        emit canZoomIn(level < 16);
        emit canZoomOut(level > 0);
    }
}

void MapViewBase::setZoomLevelAt(int level, int x, int y){
    if ((level >= 0) && (level <= MaxZoomLevel)){
        if (zoomLevel != level){
            int deltaX = x - width()/2;
            int deltaY = y - height()/2;
            xCenter += deltaX;
            yCenter += deltaY;
            adjustCenter();

            xCenter = remapToPow2(xCenter, zoomLevel, level);
            yCenter = remapToPow2(yCenter, zoomLevel, level);

            xCenter -= deltaX;
            yCenter -= deltaY;

            remapMarkers(zoomLevel, level);
            zoomLevel = level;
            emit zoomLevelChanged(zoomLevel);
            updateBound();
            scene->setSceneRect(0, 0, 1<<(zoomLevel+TilePower2), 1<<(zoomLevel+TilePower2));
            centerOn(xCenter, yCenter);
            repaint();
        }
        emit canZoomIn(level < 16);
        emit canZoomOut(level > 0);
    }
}


int MapViewBase::getSide() const{
    return 256 << zoomLevel;
}

int MapViewBase::getSideMask() const{
    return getSide() - 1;
}

int MapViewBase::getSidePow() const{
    return zoomLevel + TilePower2;
}

QSize MapViewBase::sizeHint() const{
    return QSize(TileSize, TileSize);
}

struct TileCoord{
    int x;
    int y;
};

void MapViewBase::update(QPaintEvent *event){
    event = 0;
    QPainter painter(this);
    QColor gridColor(0, 255, 0);
    QColor backgroundColor(0, 0, 0);
    QColor black(0, 0, 0);
    QColor gray(128, 128, 128);
    QColor white(255, 255, 255);
    painter.setBrush(backgroundColor);

    int xOffset = xCenter - width()/2;
    int yOffset = yCenter - height()/2;

    int mask= (1 << zoomLevel) - 1;

    xOffset = fitToPow2(xOffset, getSidePow());
    yOffset = fitToPow2(yOffset, getSidePow());

    int firstTileX = - (xOffset & 0xFF);
    int firstTileY = - (yOffset & 0xFF);

    if (images){
        for (int y = firstTileY; y < height(); y+= 256){
            int row = ((y + yOffset) >> TilePower2) & mask;
            for (int x = firstTileX; x < width(); x += 256){
                int col = ((x + xOffset)  >> TilePower2) & mask;
                QPixmap* img = images->getImage(col, row, zoomLevel);
                if (img){
                    painter.drawPixmap(QPoint(x, y), *img, QRect(0, 0, TileSize, TileSize));
                }
                else{
                    painter.setPen(white);
                    painter.setBrush(black);
                    painter.drawRect(x, y, TileSize, TileSize);
                    painter.drawText(x, y, TileSize, TileSize, Qt::AlignHCenter|Qt::AlignVCenter, 
                        QString(
                        tr(
                        "col:%1, row:%2, zoom:%3\nNo image\n" 
                        "click left+right mouse buttons\nto download missing images.")
                        ).arg(col).arg(row).arg(zoomLevel)
                        );
                }		
            }
        }		
        images->update();
        // now let's draw the markers, the code is in the path because it makes no much sense if we couldn't draw the images.
//         if (markers)
//         {
//             MarkerCache::RangeType ms = markers->MarkersInBound(currentBound); // may not be reliable in the future.
//             QPoint lowerleft = QPoint(xCenter, yCenter) - QPoint(width()/2, height()/2);
//             
//             BOOST_FOREACH(const MarkerInfo& m, ms)
//             {
//                 QPoint p = InternalGeoCoordToCoord(m.location.lat, m.location.lng);
//                 p.setX( remapToPow2(p.x(), MaxZoomLevel, zoomLevel) );
//                 p.setY( remapToPow2(p.y(), MaxZoomLevel, zoomLevel) );
//                 p -= lowerleft;
//                 painter.drawEllipse(p, 5, 5);
//                 painter.drawText(p.x() - 10, p.y() + 20, m.info);
//             }
//         }
    }
    else	
        for (int y = firstTileY; y < height(); y+= 256){
            int row = ((y + yOffset) >> TilePower2) & mask;
            for (int x = firstTileX; x < width(); x += 256){
                int col = ((x + xOffset)  >> TilePower2) & mask;
                if ((col + row)&1)
                    painter.setBrush(gray);
                else
                    painter.setBrush(black);
                painter.setPen(white);
                painter.drawRect(x, y, TileSize, TileSize);
                painter.drawText(
                    x, y, TileSize, TileSize, Qt::AlignHCenter|Qt::AlignVCenter,
                    QString("col:%1, row:%2, zoom:%3\ncache not set\nprogramming error, maybe?")
                    .arg(col).arg(row).arg(zoomLevel)
                    );			
            }
        }
        decorator.paintEvent(painter);
}

void MapViewBase::moveBy(int x, int y){
    xCenter -= x;
    yCenter -= y;
    adjustCenter();
    updateBound();
    centerOn(xCenter, yCenter);
    //repaint();
}

void MapViewBase::setDecorator(Decorator *newDecorator){
    decorator.setDecorator(newDecorator);
}

void MapViewBase::appendDecorator(Decorator *newDecorator){
    decorator.appendDecorator(newDecorator);
}

void MapViewBase::insertDecorator(Decorator *newDecorator){
    decorator.insertDecorator(newDecorator);
}

void MapViewBase::mouseMoveEvent(QMouseEvent *event){
    decorator.mouseMoveEvent(event);
}

void MapViewBase::mousePressEvent(QMouseEvent *event){
    decorator.mousePressEvent(event);
}

void MapViewBase::mouseReleaseEvent(QMouseEvent *event){
    decorator.mouseReleaseEvent(event);
}

void MapViewBase::mouseDoubleClickEvent(QMouseEvent *event){
    decorator.mouseDoubleClickEvent(event);
}

void MapViewBase::keyPressEvent(QKeyEvent *event){
    decorator.keyPressEvent(event);
}

void MapViewBase::keyReleaseEvent(QKeyEvent *event){
    decorator.keyReleaseEvent(event);
}

void MapViewBase::leaveEvent(QEvent *event){
    decorator.leaveEvent(event);
}

void MapViewBase::downloadMissingImages(){
    printf("download missing images\n");
    if (!images)
        return;

    int xOffset = xCenter - width()/2;
    int yOffset = yCenter - height()/2;

    int mask= (1 << zoomLevel) - 1;

    xOffset = fitToPow2(xOffset, getSidePow());
    yOffset = fitToPow2(yOffset, getSidePow());

    int firstTileX = - (xOffset & 0xFF);
    int firstTileY = - (yOffset & 0xFF);

    for (int y = firstTileY; y < height(); y+= 256){
        int row = ((y + yOffset) >> TilePower2) & mask;
        for (int x = firstTileX; x < width(); x += 256){
            int col = ((x + xOffset)  >> TilePower2) & mask;
            //FIXME it would be good to have a more elegant solution...
            images->tryDownload(col, row, zoomLevel);		
        }
    }
}

void MapViewBase::scheduleRepaint(){
    QTimer::singleShot(500, this, SLOT(repaint()));
}

int MapViewBase::getZoomLevel(){
    return zoomLevel;
}

QPoint MapViewBase::getCoords(){
    return QPoint(
        remapToPow2(xCenter, zoomLevel, MaxZoomLevel),
        remapToPow2(yCenter, zoomLevel, MaxZoomLevel)
        );
}

void MapViewBase::setCoords(const QPoint& coords){
    xCenter = remapToPow2(coords.x(), MaxZoomLevel, zoomLevel);
    yCenter = remapToPow2(coords.y(), MaxZoomLevel, zoomLevel);
    updateBound();
    centerOn(xCenter, yCenter);
    repaint();
}

void MapViewBase::resetCoords(){
    int ofs = 1 << (TilePower2 + MaxZoomLevel - 1);
    setCoords(QPoint(ofs, ofs));
    setZoomLevel(0);
}

static const double pi = 3.1415926535897932384626433832795028841971693993751;

void MapViewBase::setGeoCoords(const GeoCoord &latitude, const GeoCoord &longitude)
{
    setCoords(InternalGeoCoordToCoord(latitude, longitude));
}

void MapViewBase::getGeoCoords(GeoCoord& latitude, GeoCoord& longitude) const
{
    InternalCoordToGeoCoord(QPoint(xCenter, yCenter), latitude, longitude);
}

QPoint MapViewBase::InternalGeoCoordToCoord( const GeoCoord& latitude, const GeoCoord& longitude ) const
{
    QPoint coords;

    //Mercator projection (this is the one used by Google)
    qint32 halfSize = (1 << (MaxZoomLevel + TilePower2 - 1));
    double x = (double)longitude.getValue()/(double)maxLongitude.getValue();
    double y = (double)latitude.getValue()/(double)maxLatitude.getValue();

    x = x * halfSize + halfSize;
    y = y * pi/2.0;
    y = log(tan(y) + 1.0/cos(y));
    y = y/(pi);

    y = halfSize - y * halfSize;

    coords.setX((int)x);
    coords.setY((int)y);
    return coords;
}

void MapViewBase::InternalCoordToGeoCoord( QPoint coord, GeoCoord &latitude, GeoCoord &longitude ) const
{
    qint32 halfSize = (1 << (zoomLevel + TilePower2 - 1));
    double x = (double)coord.x() / (double)halfSize - 1;
    double y = 1- (double)coord.y() / (double)halfSize;
    longitude.setDouble(x * 180.0);	
    y *= pi;
    y = 2.0 * atan(exp(y)) - pi/2.0;
    latitude.setDouble(y * 180 / pi);
}

void MapViewBase::updateBound()
{
    // avoid emitting too much signals
    if ( abs(xCenter - last_xcenter) > 40 || abs(yCenter - last_ycenter) > 40 )
    {
        last_xcenter = xCenter;
        last_ycenter = yCenter;
        int xLeft, xRight, yTop, yBottom;
        int halfWidth, halfHeight;
        halfWidth = width() / 2;
        halfHeight = height() / 2;
        xLeft = xCenter - halfWidth;
        xRight = xCenter + halfWidth;
        yTop = yCenter - halfHeight;
        yBottom = yCenter + halfHeight;

        InternalCoordToGeoCoord(QPoint(xLeft, yBottom), currentBound.SW.lat, currentBound.SW.lng);
        InternalCoordToGeoCoord(QPoint(xRight, yTop), currentBound.NE.lat, currentBound.NE.lng);

        emit boundsChange(currentBound);
    }
}

QColor gridColor(0, 255, 0);
QColor backgroundColor(0, 0, 0);
QColor black(0, 0, 0);
QColor gray(128, 128, 128);
QColor white(255, 255, 255);
QRect ImageRect(0, 0, 256, 256);

void MapViewBase::drawBackground( QPainter *painter, const QRectF &rect )
{
    painter->setBrush(backgroundColor);

    int mask= (1 << zoomLevel) - 1;

    int firstTileX = (~0xFF) & (int)rect.x();
    int firstTileY = (~0xFF) & (int)rect.y();

    int dirtywidth = rect.x() + rect.width();
    int dirtyheight = rect.y() + rect.height();

    if (images){
#if 0
        qDebug()<<"Dirty Region: x: "<<(int)rect.x()<<" y: "<<(int)rect.y()
            <<"              w: "<<(int)rect.width()<<" h: "<<(int)rect.height();
#endif
        for (int y = firstTileY; y < dirtyheight; y+= 256){
            int row = (y >> TilePower2) & mask;
            for (int x = firstTileX; x < dirtywidth; x += 256){
                int col = (x >> TilePower2) & mask;
                QPixmap* img = images->getImage(col, row, zoomLevel);
                if (img){
                    painter->drawPixmap(QPoint(x, y), *img, ImageRect);
                }
                else{
                    painter->setPen(white);
                    painter->setBrush(black);
                    painter->drawRect(x, y, TileSize, TileSize);
                    painter->drawText(x, y, TileSize, TileSize, Qt::AlignHCenter|Qt::AlignVCenter, 
                        QString(
                        "col:%1, row:%2, zoom:%3\nNo image\n" 
                        "click left+right mouse buttons\nto download missing images."
                        ).arg(col).arg(row).arg(zoomLevel)
                        );
                }		
            }
        }		
        images->update();
    }
}

void MapViewBase::addRestaurantMarker(ProtocolBuffer::Restaurant* r)
{
//     QPoint p = InternalGeoCoordToCoord(info->location.lat, info->location.lng);
//     p.setX( remapToPow2(p.x(), MaxZoomLevel, zoomLevel) );
//     p.setY( remapToPow2(p.y(), MaxZoomLevel, zoomLevel) );
//     QPoint markerp;
//     markerp.setX( p.x() );
//     markerp.setY( p.y() - markerImage.height());
//     scene->addText(info->info)->setPos(p);
//     scene->addPixmap(markerImage)->setPos(markerp);
//     delete info; // this is bad, we might need to switch markerinfo to a QObject and use deleteLater()
}

void MapViewBase::remapMarkers( int oldzoomlevel, int newzoomlevel )
{
    QList<QGraphicsItem*> items = scene->items();
    BOOST_FOREACH(QGraphicsItem* item, items)
    {
        QGraphicsPixmapItem *imageitem = qgraphicsitem_cast<QGraphicsPixmapItem*>(item);
        QPointF p = item->pos();
        if (imageitem)
        {
            p.setY( p.y() + markerImage.height());
        }
        p.setX( remapToPow2(p.x(), oldzoomlevel, newzoomlevel) );
        p.setY( remapToPow2(p.y(), oldzoomlevel, newzoomlevel) );
        if (imageitem)
        {
            p.setY( p.y() - markerImage.height());
        }
        item->setPos(p);
    }
}
