#include "MarkerItem.h"
#include "CoordsHelper.h"
#include "MapProtocol.pb.h"
#include "OfflineMap/MapViewBase.h"
#include <QGraphicsSceneResizeEvent>
#include <QGraphicsEllipseItem>
#include <QPainter>
#include <QPixmap>
#include <QStyle>
#include <QStyleOptionButton>
#include <QWidget>
#include <QPushButton>
#include <QDebug>
#include <QTimer>
#include <boost/foreach.hpp>

void ZoomSensitiveItem::setZoom( int zoom )
{
    currentZoom = zoom;
    setPos(getPos());
}

void ZoomSensitiveItem::setPos( const GeoPoint& center )
{
    location = center;
    if (getZoom() != -1)
        setPos(CoordsHelper::InternalGeoCoordToCoord(location.lat, location.lng, getZoom()));
}

RestaurantMarkerItem::~RestaurantMarkerItem()
{
    if (isFakeMarker())
        delete r;
}

ProtocolBuffer::Restaurant* RestaurantMarkerItem::mutableRestaurantInfo()
{
    ProtocolBuffer::Restaurant* ret = NULL;
    if (isFakeMarker())
    {
        if (r == NULL)
        {
            ret = new ProtocolBuffer::Restaurant;
            ret->set_rid(-1);
            ret->set_rating(0);
            ret->set_name("");
            ret->set_commentcount(0);
            ret->mutable_averageexpense()->set_amount(0.);
            ret->mutable_type()->set_name("");
            ret->mutable_type()->set_tid(0);
            ret->mutable_location()->set_latitude(0.);
            ret->mutable_location()->set_longitude(0.);
            Q_ASSERT( ret->IsInitialized() );
            r = ret;
        }
        ret = const_cast<ProtocolBuffer::Restaurant*>(r);
    }
    return ret;
}

void RestaurantMarkerItem::paint( QPainter *p, const QStyleOptionGraphicsItem *, QWidget * )
{
    QPixmap& image = isFakeMarker() ? fakeMarkerImage() : markerImage();
    p->drawPixmap(-image.width()/2, -image.height(), image);
}

void RestaurantMarkerItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    ZoomSensitiveItem::mouseReleaseEvent(event);
    if (this->isFakeMarker())
    {
        GeoPoint point;
        CoordsHelper::InternalCoordToGeoCoord(this->pos().toPoint(), this->getZoom(), point.lat, point.lng);
        this->mutableRestaurantInfo()->mutable_location()->set_latitude(point.lat.getDouble());
        this->mutableRestaurantInfo()->mutable_location()->set_longitude(point.lng.getDouble());
        this->setPos(point);
    }
}

QRectF RestaurantMarkerItem::boundingRect() const
{
    return QRectF(-markerImage().width()/2, -markerImage().height(), markerImage().width(), markerImage().height());
}

QPixmap& RestaurantMarkerItem::markerImage()
{
    static QPixmap image(":/Icons/marker.png");
    return image;
}

QPixmap& RestaurantMarkerItem::fakeMarkerImage()
{
    static QPixmap image(":/Icons/black.png");
    return image;
}

void RestaurantMarkerItem::PromoteToRealMarker(const ProtocolBuffer::Restaurant* restaurant)
{
    Q_ASSERT( restaurant );
    isLocalMarker = false;
    if (r && r != restaurant)
        delete r;
    r = restaurant;
}

void UserMarkerItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *, QWidget * /* = 0 */ )
{
    const QPixmap& image = UserIcon();
    painter->drawPixmap(-image.width()/2, -image.height()/2, image);
}

QRectF UserMarkerItem::boundingRect() const
{
    const QPixmap& image = UserIcon();
    return QRectF(-image.width()/2, -image.height(), image.width(), image.height());
}

const QPixmap& UserMarkerItem::defaultUserIcon()
{
    static QPixmap image(":/Icons/anonymousUser.png");
    return image;
}

const QPixmap& UserMarkerItem::UserIcon() const
{
    return defaultUserIcon();
}

SelfMarkerItem::SelfMarkerItem()
{
    accuracy = 0.;
    area = NULL;
}

SelfMarkerItem::~SelfMarkerItem()
{
    delete area;
}

const QPixmap& SelfMarkerItem::UserIcon() const
{
    static QPixmap image(":/Icons/selfMarker.png");
    return image;
}

void SelfMarkerItem::setInaccuratePosition( const InaccurateGeoPoint& p )
{
    setPos(p.p);
    if (accuracy != p.accuracy)
    {
        if (!area)
        {
            area = new QGraphicsEllipseItem(this);
            QPen pen(QColor(0, 0, 150, 160));
            pen.setWidth(2);
            area->setPen(pen);
            area->setBrush(QBrush(QColor(0, 0, 200, 50)));
            area->setFlag(QGraphicsItem::ItemStacksBehindParent);
        }
        this->accuracy = p.accuracy;
        qreal radius = p.accuracy * ((1<<(CoordsHelper::TilePower2+getZoom())) / (cos(p.p.lat.getDouble() * CoordsHelper::pi/180) * 2 * CoordsHelper::pi * 6378137));
        area->setRect(-radius, -radius, radius*2, radius*2);
        //     double resolution = cos(p.p.lat.getDouble() * CoordsHelper::pi/180) * 2 * CoordsHelper::pi * 6378137 / 1<<(CoordsHelper::TilePower2+getZoom());
        //     radius = p.accuracy / resolution;
    }
}

void SelfMarkerItem::setPos(const GeoPoint& center)
{
    UserMarkerItem::setPos(center);
    if (area)
        area->setPos(this->pos());
}

void SelfMarkerItem::setZoom( int zoom )
{
    UserMarkerItem::setZoom(zoom);
    qreal radius = accuracy * ((1<<(CoordsHelper::TilePower2+getZoom())) / (cos(getPos().lat.getDouble() * CoordsHelper::pi/180) * 2 * CoordsHelper::pi * 6378137));
    if (area)
        area->setRect(-radius, -radius, radius*2, radius*2);
}

PanelWidget::PanelWidget(MapViewBase *map, QGraphicsItem* parent, Qt::WindowFlags wFlags)
    : QGraphicsProxyWidget(parent, wFlags)
{
    target = map;
    balloonTarget = NULL;
}

void PanelWidget::setWidget(QWidget *widget, ZoomSensitiveItem *balloonOn)
{
    connect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(handleWidgetDestroyed(QObject*)));
    tie(balloonOn);
    QGraphicsProxyWidget::setWidget(widget);
    setupCloseButton();
}

void PanelWidget::tie(ZoomSensitiveItem *balloon)
{
    balloonTarget = balloon;
    if (balloon)
    {
        this->setWindowFrameMargins(0., 0., 0., 12.);
        retieToTarget();
    }
    else
    {
        this->unsetWindowFrameMargins();
    }
}

void PanelWidget::handleWidgetDestroyed(QObject*)
{
    target->unlockMap();
    this->deleteLater();
}

void PanelWidget::retieToTarget()
{
    Q_ASSERT( balloonTarget );
    QRectF itemBound = balloonTarget->boundingRect(),
           panelBound = this->boundingRect();
    QPointF topCenter = balloonTarget->pos() + QPointF(itemBound.center().x(), itemBound.top());
    QPointF proxyBottomCenterPos = topCenter - QPointF(panelBound.width()/2, panelBound.height());
    this->setPos(proxyBottomCenterPos);
}

void PanelWidget::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    if (balloonTarget)
    {
        retieToTarget();
    }
    else
    {
       QPointF p = target->mapToScene(target->width(), target->height());
       if (this->y() + event->newSize().height() > p.y())
       {
           this->setPos(this->pos().x(), this->pos().y() - (this->y() + event->newSize().height() - p.y()) - 10);
       }
    }
    QGraphicsProxyWidget::resizeEvent(event);
    if (this->balloonTarget)
        target->ensureVisible(this);
}

QGraphicsProxyWidget* PanelWidget::setupCloseButton()
{
    closeButton = new QGraphicsProxyWidget(this);
    QPushButton* btn = new QPushButton;
    btn->setIcon(QPixmap(":/Icons/close.png"));
    btn->setIconSize(QSize(32, 32));
    btn->setGeometry(0, 0, 32, 32);
    btn->setMaximumSize(32, 32);
    btn->setFlat(true);
    btn->setStyleSheet("background-color: rgba(0, 0, 0,0%);");
    closeButton->setWidget(btn);
    closeButton->setGeometry(QRectF(this->widget()->width()-18, -14., 32, 32));
    connect(btn, SIGNAL(clicked()), SLOT(handleCloseButtonClick()));
    return closeButton;
}

void PanelWidget::handleCloseButtonClick()
{
    emit closing(this);
    widget()->close();
}

void PanelWidget::paintWindowFrame(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (balloonTarget)
    {
        painter->setBrush(this->widget()->palette().background());
        painter->setPen(Qt::NoPen);
        QPoint triangle[] = {
            QPoint(this->widget()->width()/2 - 10, this->widget()->height()),
            QPoint(this->widget()->width()/2, this->widget()->height() + 10),
            QPoint(this->widget()->width()/2 + 10, this->widget()->height())
        };
        painter->drawConvexPolygon(triangle, 3);
    }
}

TipPanel::TipPanel(MapViewBase *target)
{
    _isShown = false;
    this->target = target;
    this->setOpacity(0.);
    fadeTimeline.setDuration(500);
    fadeTimeline.setCurveShape(QTimeLine::EaseInOutCurve);
    fadeTimeline.setDirection(QTimeLine::Forward);
    connect(&fadeTimeline, SIGNAL(valueChanged(qreal)), SLOT(setOpacity(qreal)));
    connect(&fadeTimeline, SIGNAL(stateChanged(QTimeLine::State)), SLOT(stateChanged(QTimeLine::State)));
}

void TipPanel::closeEvent(QCloseEvent *event)
{
    _isShown = false;
}

void TipPanel::stateChanged(QTimeLine::State newState)
{
    if (newState == QTimeLine::NotRunning && fadeTimeline.direction() == QTimeLine::Forward)
    {
        fadeTimeline.setDirection(QTimeLine::Backward);
        QTimer::singleShot(20000, &fadeTimeline, SLOT(start()));
    }
    else if (newState == QTimeLine::NotRunning && fadeTimeline.direction() == QTimeLine::Backward)
    {
        this->close();
        this->deleteLater();
    }
}

void TipPanel::setOpacity(qreal opacity)
{
    QGraphicsProxyWidget::setOpacity(opacity);
}

void TipPanel::showTip()
{
    this->setPos( target->exposedView().bottomRight() - QPoint(this->size().width(), this->size().height()) - QPoint(3, 3) );
    fadeTimeline.start();
    _isShown = true;
}
