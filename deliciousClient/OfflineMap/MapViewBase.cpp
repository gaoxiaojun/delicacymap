#include <QPainter>
#include <QGraphicsScene>
#include <QTimer>
#include <QTimeLine>
#include <QDebug>
#include <QMouseEvent>
#include <QEasingCurve>
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
    this->setBackgroundBrush(QBrush(QColor(50, 50, 50, 150)));
    mapIsLocked = false;
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    last_xcenter = xCenter;
    last_ycenter = yCenter;
    scene = new QGraphicsScene;
    panMapTimeline.setDuration(300);
    connect(&panMapTimeline, SIGNAL(valueChanged(qreal)), SLOT(panMapXhandler(qreal)));
    connect(&panMapTimeline, SIGNAL(stateChanged(QTimeLine::State)), SLOT(panMapStateChange(QTimeLine::State)));
//    bounceItemTimeline.setDuration(500);
//    bounceItemTimeline.setEasingCurve(QEasingCurve(QEasingCurve::OutBounce));
    setScene(scene);
    centerOn(xCenter, yCenter);
    //setCacheMode(QGraphicsView::CacheBackground);
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setAttribute( Qt::WA_OpaquePaintEvent );// a small optimization
}

MapViewBase::~MapViewBase()
{
    if (self)
        scene->removeItem(self);
    delete self;
    delete scene;
}

void MapViewBase::lockMap()
{
    if (!isLocked())
    {
        mapIsLocked = true;
        this->update();
        emit canZoomIn(false);
        emit canZoomOut(false);
    }
}

void MapViewBase::unlockMap()
{
    if (isLocked())
    {
        mapIsLocked = false;
        this->update();
        emit canZoomIn(zoomLevel < CoordsHelper::MaxZoomLevel);
        emit canZoomOut(zoomLevel > 0);
    }
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
    if (!isLocked() && (level >= 0) && (level <= CoordsHelper::MaxZoomLevel)){
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
        emit canZoomIn(level < CoordsHelper::MaxZoomLevel);
        emit canZoomOut(level > 0);
    }
}

void MapViewBase::setZoomLevelAt(int level, int x, int y){
    if (!isLocked() && (level >= 0) && (level <= CoordsHelper::MaxZoomLevel)){
        if (zoomLevel != level){
            int deltaX = x - width()/2;
            int deltaY = y - height()/2;
            xCenter += deltaX;
            yCenter += deltaY;
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
        }
        emit canZoomIn(level < CoordsHelper::MaxZoomLevel);
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
    if (!isLocked())
    {
        centerOn(xCenter-x, yCenter-y);
        updateBound();
    }
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

void MapViewBase::mouseMoveEvent(QMouseEvent *event)
{
    if (isLocked())
        return QGraphicsView::mouseMoveEvent(event);
    QGraphicsItem *item = itemAt(event->pos());
    if (!item)
        decorator.mouseMoveEvent(event);
    QGraphicsView::mouseMoveEvent(event);
}

void MapViewBase::mousePressEvent(QMouseEvent *event)
{
    if (isLocked())
        return QGraphicsView::mousePressEvent(event);
    QGraphicsItem *item = itemAt(event->pos());
    if (!item)
        decorator.mousePressEvent(event);
    QGraphicsView::mousePressEvent(event);
}

void MapViewBase::mouseReleaseEvent(QMouseEvent *event)
{
    if (isLocked())
        return QGraphicsView::mouseReleaseEvent(event);
    QGraphicsItem *item = itemAt(event->pos());
    if (!item)
        decorator.mouseReleaseEvent(event);
    else if (event->button() == Qt::LeftButton && item->type() == RestaurantMarkerItem::Type)
    {
        emit restaurantMarkerClicked(static_cast<RestaurantMarkerItem*>(item));
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void MapViewBase::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (isLocked())
        return QGraphicsView::mouseDoubleClickEvent(event);
    QGraphicsItem *item = itemAt(event->pos());
    if (!item)
        decorator.mouseDoubleClickEvent(event);
    QGraphicsView::mouseDoubleClickEvent(event);
}

#ifndef QT_NO_WHEELEVENT
void MapViewBase::wheelEvent(QWheelEvent *event)
{
    if (isLocked())
        return;
    QGraphicsView::wheelEvent(event);
}

#endif

void MapViewBase::keyPressEvent(QKeyEvent *event){
    decorator.keyPressEvent(event);
    QGraphicsView::keyPressEvent(event);
}

void MapViewBase::keyReleaseEvent(QKeyEvent *event){
    decorator.keyReleaseEvent(event);
    QGraphicsView::keyReleaseEvent(event);
}

void MapViewBase::leaveEvent(QEvent *event){
    decorator.leaveEvent(event);
    QGraphicsView::leaveEvent(event);
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

void MapViewBase::panMapXhandler(qreal v)
{
    QPoint currentPos = _panOldCenter + _panDelta * v;
    centerOn(currentPos);
}

void MapViewBase::panMapStateChange(QTimeLine::State newState)
{
    if (newState == QTimeLine::NotRunning)
        updateBound();
}

void MapViewBase::scheduleRepaint(){
    update();
}

int MapViewBase::getZoomLevel(){
    return zoomLevel;
}

void MapViewBase::centerOn(int x, int y)
{
    xCenter = x;
    yCenter = y;
    adjustCenter();
    QGraphicsView::centerOn(xCenter, yCenter);
}

void MapViewBase::centerOn(QPoint p)
{
    centerOn(p.x(), p.y());
}

void MapViewBase::centerOn(QGraphicsItem *item, bool ignoreLockState)
{
    if (item && (ignoreLockState || !isLocked()))
    {
        QPoint p = item->pos().toPoint();
        centerOn(p);
    }
}

QPoint MapViewBase::getCoords(){
    return QPoint(
        CoordsHelper::remapToPow2(xCenter, zoomLevel, CoordsHelper::MaxZoomLevel),
        CoordsHelper::remapToPow2(yCenter, zoomLevel, CoordsHelper::MaxZoomLevel)
        );
}

void MapViewBase::setCoords(const QPoint& coords){
    if (!isLocked())
    {
        xCenter = CoordsHelper::remapToPow2(coords.x(), CoordsHelper::MaxZoomLevel, zoomLevel);
        yCenter = CoordsHelper::remapToPow2(coords.y(), CoordsHelper::MaxZoomLevel, zoomLevel);
        centerOn(xCenter, yCenter);
        updateBound();
    }
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

GeoPoint MapViewBase::getGeoCenter() const
{
    GeoPoint ret;
    CoordsHelper::InternalCoordToGeoCoord(QPoint(xCenter, yCenter), zoomLevel, ret.lat, ret.lng);
    return ret;
}

void MapViewBase::updateBound()
{
    // avoid emitting too much signals
    if ( abs(xCenter - last_xcenter) > 50 || abs(yCenter - last_ycenter) > 50 )
    {
        QRect exposed = exposedView();

        GeoPoint geoSW;
        CoordsHelper::InternalCoordToGeoCoord(exposed.bottomLeft(), zoomLevel, geoSW.lat, geoSW.lng);
        //if ( abs(currentBound.SW.lat.getDouble() - geoSW.lat.getDouble()) > 0.005 || abs(currentBound.SW.lng.getDouble() - geoSW.lng.getDouble()) > 0.005 )
        {
            last_xcenter = xCenter;
            last_ycenter = yCenter;
            currentBound.SW = geoSW;
            CoordsHelper::InternalCoordToGeoCoord(exposed.topRight(), zoomLevel, currentBound.NE.lat, currentBound.NE.lng);

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

    int dirtywidth = rect.right();
    int dirtyheight = rect.bottom();

    if (images)
    {
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
    if (this->isLocked())
        painter->fillRect(rect, this->backgroundBrush());
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

RouteItem* MapViewBase::addRoute( const QList<GeoPoint>& p )
{
    RouteItem * item = new RouteItem(p);
    item->setZoom(zoomLevel);
    scene->addItem(item);
    return item;
}

RouteItem* MapViewBase::addRoute( const QList<GeoPoint>& p, int user )
{
    RouteItem * item = new RouteItem(p, true);
    item->setZoom(zoomLevel);
    item->setRouteReceiverWhenDoneEditing(user);
    scene->addItem(item);
    return item;
}

void MapViewBase::setSelfLocation( const GeoPoint& coord )
{
    if (!self)
    {
        self = new SelfMarkerItem;
        self->setZoom(zoomLevel);
        scene->addItem(self);
    }
    self->setPos(coord);
}

void MapViewBase::updateUserLocation( int uid, const GeoPoint& coord )
{
    Q_ASSERT( !self || self->userInfo()->uid() != uid);
    if (users[uid] == NULL)
    {
        UserMarkerItem* newusr = new UserMarkerItem;
        newusr->setZoom(zoomLevel);
        scene->addItem(newusr);
        users[uid] = newusr;
    }
    users[uid]->setPos(coord);
}

PanelWidget* MapViewBase::addBlockingPanel(QWidget* panel, ZoomSensitiveItem* balloonOn)
{
    lockMap();
    panel->setAttribute(Qt::WA_DeleteOnClose, true);
    PanelWidget *proxy = new PanelWidget(this, NULL, Qt::Window);
    proxy->setWidget(panel, balloonOn);
    proxy->setZValue(1000);
    if (!balloonOn)
        proxy->setPos(xCenter - panel->width()/2, qMax(yCenter - panel->height(), yCenter - this->height()/2));
//    proxy->setPanelModality(QGraphicsItem::SceneModal);
    this->ensureVisible(proxy);
    scene->addItem(proxy);
    return proxy;
}

void MapViewBase::removeItem( ZoomSensitiveItem* item )
{
    removeFromLocal(item);
    scene->removeItem(item);
}

void MapViewBase::removeFromLocal(ZoomSensitiveItem *item)
{
    localItems.removeOne(item);
}

bool MapViewBase::hasLocalMarker() const
{
    return !localItems.empty();
}

QList<ZoomSensitiveItem*> MapViewBase::localMarkers() const
{
    return localItems;
}

void MapViewBase::addLocalMarker(ZoomSensitiveItem * e)
{
    e->setFlag( QGraphicsItem::ItemIsMovable, true );
    localItems.push_back(e);
    e->setPos(this->getGeoCenter());
    e->setZoom(zoomLevel);
    scene->addItem(e);
    // start the bounce effect
}

QRect MapViewBase::exposedView() const
{
    return QRect(
            xCenter - width()/2,
            yCenter - height()/2,
            width(),
            height());
}

RestaurantMarkerItem* MapViewBase::getRestaurantMarker(int rid)
{
    RestaurantMarkerItem* marker = NULL;
    QList<QGraphicsItem*> allitems = items();

    BOOST_FOREACH(QGraphicsItem* e, allitems)
        if (e->type() == RestaurantMarkerItem::Type)
        {
            RestaurantMarkerItem* r = static_cast<RestaurantMarkerItem*>(e);
            if (r->restaurantInfo()->rid() == rid)
            {
                marker = r;
                break;
            }
        }

    return marker;
}

void MapViewBase::panBy(QPoint p)
{
    if (p.manhattanLength() < 8)
    {
        xCenter += p.x();
        yCenter += p.y();
        centerOn(xCenter, yCenter);
    }
    else
    {
        _panDelta = p;
        _panOldCenter = QPoint(xCenter, yCenter);
        if (panMapTimeline.state() == QTimeLine::Running)
            panMapTimeline.stop();
        panMapTimeline.setDirection(QTimeLine::Forward);
        panMapTimeline.start();
    }
}

void MapViewBase::ensureVisible(const QGraphicsItem *item, int xmargin, int ymargin)
{
    if (item)
    {
        QPoint delta;
        QRect exposed = exposedView();
        QRect itemBound = item->sceneBoundingRect().toRect();

        delta.setX( qMin(itemBound.x() - exposed.x(), 0) );
        delta.setY( qMin(itemBound.y() - exposed.y(), 0) );
        if (delta.x() < 0)
            delta.setX(delta.x() - xmargin);
        else //delta.x() == 0
        {
            delta.setX( qMax(itemBound.right() - exposed.right(), 0) );
            if (delta.x() > 0)
                delta.setX(delta.x() + xmargin);
        }
        if (delta.y() < 0)
            delta.setY(delta.y() - ymargin);
        else
        {
            delta.setY( qMax(itemBound.bottom() - exposed.bottom(), 0) );
            if (delta.y() > 0)
                delta.setX(delta.y() + xmargin);
        }

        panBy(delta);
    }
}
