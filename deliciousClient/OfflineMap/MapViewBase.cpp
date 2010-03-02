#include <QPainter>
#include <QGraphicsScene>
#include <QTimer>
#include <QDebug>
#include <QMouseEvent>
#include <math.h>
#include "CoordsHelper.h"
#include "MapViewBase.h"
#include "ImageCache.h"
#include "GeoCoord.h"
#include "MarkerItem.h"
#include "MapProtocol.pb.h"

#include <boost/foreach.hpp>

const GeoCoord CoordsHelper::maxLatitude(90, 0, 0, 0);//(85.0511287798066);//(90, 0, 0, 0);
const GeoCoord CoordsHelper::maxLongitude(180, 0, 0, 0);
const double CoordsHelper::pi = 3.1415926535897932384626433832795028841971693993751;

MapViewBase::MapViewBase(QWidget *parent)
:QGraphicsView(parent), xCenter(128), yCenter(128), zoomLevel(0), images(0), self(NULL)
{
    handleDblClickEvent = handleMoveEvent = handlePressEvent = handleReleaseEvent = true;
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    last_xcenter = xCenter;
    last_ycenter = yCenter;
    scene = new QGraphicsScene;
    setScene(scene);
    centerOn(xCenter, yCenter);
    //setCacheMode(QGraphicsView::CacheBackground);
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setAttribute( Qt::WA_OpaquePaintEvent );// a small optimization
}

MapViewBase::~MapViewBase(){
    delete scene;
    delete self;
}

void MapViewBase::setCache(ImageCache* imageCache){
    images = imageCache;
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

void MapViewBase::adjustCenter(){
    xCenter = CoordsHelper::fitToPow2(xCenter, zoomLevel+CoordsHelper::TilePower2);
    yCenter = CoordsHelper::fitToPow2(yCenter, zoomLevel+CoordsHelper::TilePower2);
}

void MapViewBase::setZoomLevel(int level){
    if ((level >= 0) && (level <= CoordsHelper::MaxZoomLevel)){
        if (zoomLevel != level){
            adjustCenter();
            xCenter = CoordsHelper::remapToPow2(xCenter, zoomLevel, level);
            yCenter = CoordsHelper::remapToPow2(yCenter, zoomLevel, level);
            remapMarkers(zoomLevel, level);
            zoomLevel = level;
            emit zoomLevelChanged(zoomLevel);
            if (images)
                images->clear();
            updateBound();
            scene->setSceneRect(0, 0, 1<<(zoomLevel+CoordsHelper::TilePower2), 1<<(zoomLevel+CoordsHelper::TilePower2));
            centerOn(xCenter, yCenter);
            //repaint();
        }
        emit canZoomIn(level < 16);
        emit canZoomOut(level > 0);
    }
}

void MapViewBase::setZoomLevelAt(int level, int x, int y){
    if ((level >= 0) && (level <= CoordsHelper::MaxZoomLevel)){
        if (zoomLevel != level){
            int deltaX = x - width()/2;
            int deltaY = y - height()/2;
            xCenter += deltaX;
            yCenter += deltaY;
            adjustCenter();

            xCenter = CoordsHelper::remapToPow2(xCenter, zoomLevel, level);
            yCenter = CoordsHelper::remapToPow2(yCenter, zoomLevel, level);

            xCenter -= deltaX;
            yCenter -= deltaY;

            remapMarkers(zoomLevel, level);
            zoomLevel = level;
            emit zoomLevelChanged(zoomLevel);
            if (images)
                images->clear();
            updateBound();
            scene->setSceneRect(0, 0, 1<<(zoomLevel+CoordsHelper::TilePower2), 1<<(zoomLevel+CoordsHelper::TilePower2));
            centerOn(xCenter, yCenter);
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
    return zoomLevel + CoordsHelper::TilePower2;
}

QSize MapViewBase::sizeHint() const{
    return QSize(CoordsHelper::TileSize, CoordsHelper::TileSize);
}

struct TileCoord{
    int x;
    int y;
};

void MapViewBase::moveBy(int x, int y){
    xCenter -= x;
    yCenter -= y;
    adjustCenter();
    updateBound();
    centerOn(xCenter, yCenter);
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
    //handleReleaseEvent = false;
    if (handleMoveEvent)
    {
        QGraphicsItem *item = itemAt(event->pos());
        RouteItem *r;
        if (item && (r = qgraphicsitem_cast<RouteItem*>(item)))
        {
            QGraphicsView::mouseMoveEvent(event);
            return;
        }
        decorator.mouseMoveEvent(event);
    }
    QGraphicsView::mouseMoveEvent(event);
}

void MapViewBase::mousePressEvent(QMouseEvent *event){
    handleReleaseEvent = true;
    if (handlePressEvent)
        decorator.mousePressEvent(event);
    QGraphicsView::mousePressEvent(event);
}

void MapViewBase::mouseReleaseEvent(QMouseEvent *event){
    if (handleReleaseEvent)
    {
        if (event->button() == Qt::LeftButton)
        {
            QGraphicsItem *item = itemAt(event->pos());
            RestaurantMarkerItem *r;
            if (item && (r = qgraphicsitem_cast<RestaurantMarkerItem*>(item)))
            {
                emit restaurantMarkerClicked(r->restaurantInfo());
            }
        }
        decorator.mouseReleaseEvent(event);
    }
    QGraphicsView::mouseReleaseEvent(event);
    handleMoveEvent = true;
}

void MapViewBase::mouseDoubleClickEvent(QMouseEvent *event){
    if (handleDblClickEvent)
    {
        if (event->button() == Qt::LeftButton)
        {
            // specify here which marks we want to make interactive
            QGraphicsItem *item = itemAt(event->pos());
            RouteItem *r;
            if (item && (r = qgraphicsitem_cast<RouteItem*>(item)))
            {
                QGraphicsView::mouseDoubleClickEvent(event);
                return;
            }
        }
        decorator.mouseDoubleClickEvent(event);
    }
    QGraphicsView::mouseDoubleClickEvent(event);
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
    if (!images)
        return;

    int xOffset = xCenter - width()/2;
    int yOffset = yCenter - height()/2;

    int mask= (1 << zoomLevel) - 1;

    xOffset = CoordsHelper::fitToPow2(xOffset, getSidePow());
    yOffset = CoordsHelper::fitToPow2(yOffset, getSidePow());

    int firstTileX = - (xOffset & 0xFF);
    int firstTileY = - (yOffset & 0xFF);

    for (int y = firstTileY; y < height(); y+= 256){
        int row = ((y + yOffset) >> CoordsHelper::TilePower2) & mask;
        for (int x = firstTileX; x < width(); x += 256){
            int col = ((x + xOffset)  >> CoordsHelper::TilePower2) & mask;
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
        CoordsHelper::remapToPow2(xCenter, zoomLevel, CoordsHelper::MaxZoomLevel),
        CoordsHelper::remapToPow2(yCenter, zoomLevel, CoordsHelper::MaxZoomLevel)
        );
}

void MapViewBase::setCoords(const QPoint& coords){
    xCenter = CoordsHelper::remapToPow2(coords.x(), CoordsHelper::MaxZoomLevel, zoomLevel);
    yCenter = CoordsHelper::remapToPow2(coords.y(), CoordsHelper::MaxZoomLevel, zoomLevel);
    updateBound();
    centerOn(xCenter, yCenter);
}

void MapViewBase::resetCoords(){
    int ofs = 1 << (CoordsHelper::TilePower2 + CoordsHelper::MaxZoomLevel - 1);
    setCoords(QPoint(ofs, ofs));
    setZoomLevel(0);
}

void MapViewBase::setGeoCoords(const GeoCoord &latitude, const GeoCoord &longitude)
{
    setCoords(CoordsHelper::InternalGeoCoordToCoord(latitude, longitude));
}

void MapViewBase::getGeoCoords(GeoCoord& latitude, GeoCoord& longitude) const
{
    CoordsHelper::InternalCoordToGeoCoord(QPoint(xCenter, yCenter), zoomLevel, latitude, longitude);
}

void MapViewBase::updateBound()
{
    // avoid emitting too much signals
    if ( abs(xCenter - last_xcenter) > 50 || abs(yCenter - last_ycenter) > 50 )
    {
        int xLeft, xRight, yTop, yBottom;
        int halfWidth, halfHeight;
        halfWidth = width() / 2;
        halfHeight = height() / 2;
        xLeft = xCenter - halfWidth;
        xRight = xCenter + halfWidth;
        yTop = yCenter - halfHeight;
        yBottom = yCenter + halfHeight;

        GeoPoint geoSW;
        CoordsHelper::InternalCoordToGeoCoord(QPoint(xLeft, yBottom), zoomLevel, geoSW.lat, geoSW.lng);
        //if ( abs(currentBound.SW.lat.getDouble() - geoSW.lat.getDouble()) > 0.005 || abs(currentBound.SW.lng.getDouble() - geoSW.lng.getDouble()) > 0.005 )
        {
            last_xcenter = xCenter;
            last_ycenter = yCenter;
            currentBound.SW = geoSW;
            CoordsHelper::InternalCoordToGeoCoord(QPoint(xRight, yTop), zoomLevel, currentBound.NE.lat, currentBound.NE.lng);

            emit boundsChange(currentBound);
        }
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
            int row = (y >> CoordsHelper::TilePower2) & mask;
            for (int x = firstTileX; x < dirtywidth; x += 256){
                int col = (x >> CoordsHelper::TilePower2) & mask;
                QPixmap* img = images->getImage(col, row, zoomLevel);
                if (img){
                    painter->drawPixmap(QPoint(x, y), *img, ImageRect);
                }
                else{
                    painter->setPen(white);
                    painter->setBrush(black);
                    painter->drawRect(x, y, CoordsHelper::TileSize, CoordsHelper::TileSize);
                    painter->drawText(x, y, CoordsHelper::TileSize, CoordsHelper::TileSize, Qt::AlignHCenter|Qt::AlignVCenter, 
                        QString(
                        "col:%1, row:%2, zoom:%3\nNo image\n" 
                        "click left+right mouse buttons\nto download missing images."
                        ).arg(col).arg(row).arg(zoomLevel)
                        );
                }		
            }
        }		
        images->tick();
    }
}

void MapViewBase::addRestaurantMarker(const ProtocolBuffer::Restaurant* r)
{
    RestaurantMarkerItem *item = new RestaurantMarkerItem(r);
    item->setPos(GeoPoint(r->location().latitude(), r->location().longitude()));
    item->setZoom(zoomLevel);
    scene->addItem(item);
}

void MapViewBase::remapMarkers( int /*oldzoomlevel*/, int newzoomlevel )
{
    QList<QGraphicsItem*> items = scene->items();
    BOOST_FOREACH(QGraphicsItem* item, items)
    {
        ZoomSensitiveItem* zoomitem = (ZoomSensitiveItem*)item;  // !!! Not Safe!!!!
        zoomitem->setZoom(newzoomlevel);
    }
}

void MapViewBase::addRoute( const QList<GeoPoint>& p )
{
    RouteItem * item = new RouteItem(p);
    item->setZoom(zoomLevel);
    scene->addItem(item);
}

void MapViewBase::setSelfLocation( const GeoPoint& coord )
{
    if (!self)
    {
        self = new SelfMarkerItem;
        self->setZoom(zoomLevel);
    }
    self->setPos(coord);
}
