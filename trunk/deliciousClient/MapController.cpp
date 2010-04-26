#include "MapController.h"
#include "MapProtocol.pb.h"
#include "Session.h"
#include "Configurations.h"
#include "OfflineMap/MapViewBase.h"
#include "OfflineMap/MarkerItem.h"
#include <QGeoPositionInfo>
#include <QGeoPositionInfoSource>
#include <QGeoSatelliteInfoSource>
#include <QMessageBox>
#include <QDebug>
#include <QList>

MapController::MapController(void)
: map(NULL), session(NULL)
{
    sat_svc = NULL;
    loc_svc = QGeoPositionInfoSource::createDefaultSource(this);
    if (loc_svc)
    {
        loc_svc->setUpdateInterval(2000);
        connect(loc_svc, SIGNAL(positionUpdated(QGeoPositionInfo)), this, SLOT(translateLocationSignal(const QGeoPositionInfo&)));
        loc_svc->startUpdates();
    }
}

MapController::~MapController(void)
{
}

QGeoSatelliteInfoSource* MapController::getSatelliteInfoSource()
{
    if (!sat_svc)
    {
        sat_svc = QGeoSatelliteInfoSource::createDefaultSource(this);
    }
    return sat_svc;
}

void MapController::MapViewBoundsChange( const GeoBound& bound )
{
    if (getSession() && getMapView())
    {
        ProtocolBuffer::RestaurantList *rlist = new ProtocolBuffer::RestaurantList;
		google::protobuf::Closure *rClosure = google::protobuf::NewCallback(this, &MapController::RestaurantListHandler, rlist, getMapView());
        getSession()->getDataSource().GetRestaurants(
            bound.SW.lat.getDouble(), bound.SW.lng.getDouble(),
            bound.NE.lat.getDouble(), bound.NE.lng.getDouble(),
            17,
            rlist,
            rClosure);
    }
}

void MapController::RestaurantListHandler( ProtocolBuffer::RestaurantList* rlist, MapViewBase* view )
{
    for (int i=0;i<rlist->restaurants_size();i++)
    {
        const ProtocolBuffer::Restaurant& r = rlist->restaurants(i);
        if (!_restaurants.contains(r.rid()))
        {
            _restaurants.insert(r.rid());
            emit newRestaurantMarker(new ProtocolBuffer::Restaurant(r));
        }
    }
    delete rlist;
}

void MapController::setMapView( MapViewBase* m )
{
    map = m;
    connect(this, SIGNAL(newRestaurantMarker(const ProtocolBuffer::Restaurant*)), map, SLOT(addRestaurantMarker(const ProtocolBuffer::Restaurant*)));
}

void MapController::translateLocationSignal( const QGeoPositionInfo& info )
{
//    qDebug()<<"New geo coordinate: lat="<<info.coordinate().latitude()<<"; lng="<<info.coordinate().longitude();
    if (info.isValid())
    {
        GeoPoint location(info.coordinate().latitude() + Configurations::Instance().GPSCorrection_Latitude(), info.coordinate().longitude() + Configurations::Instance().GPSCorrection_Longitude());
        InaccurateGeoPoint inaccurate;
        inaccurate.p = location;
        inaccurate.accuracy = info.hasAttribute( QGeoPositionInfo::HorizontalAccuracy) ? info.attribute(QGeoPositionInfo::HorizontalAccuracy) : 0;
        emit currentLocationUpdate(location);
        emit currentLocationUpdate(inaccurate);
    }
}

void MapController::HandleSystemMessages( const ProtocolBuffer::DMessage* msg )
{
    if (msg->issystemmessage())
    {
        switch (msg->systemmessagetype())
        {
        case ProtocolBuffer::RequestRouting:
            {
                ProtocolBuffer::AreaEx routing;
                routing.ParseFromString(msg->buffer());
                Q_ASSERT( (routing.northeast().has_location_geo() && routing.southwest().has_location_geo()) ||
                          (routing.northeast().has_location_st() && routing.southwest().has_location_st()) );
                emit SysMsgRequestRouting(msg->fromuser(), &routing.northeast(), &routing.southwest());
            }
            break;
        case ProtocolBuffer::RoutingReply:
            {
                ProtocolBuffer::Route r;
                QList<GeoPoint> route;
                r.ParseFromString(msg->buffer());
                for (int i=0;i<r.waypoints_size();i++)
                {
                    const ProtocolBuffer::Location& p = r.waypoints(i);
                    route.append( GeoPoint(p.latitude(), p.longitude()) );
                }
                emit SysMsgRoutingReply(msg->fromuser(), route);
            }
            break;
        case ProtocolBuffer::RejectRouting:
            break;
        case ProtocolBuffer::UserLocationUpdate:
            {
                ProtocolBuffer::Location loc;
                GeoPoint p;
                loc.ParseFromString(msg->buffer());
                p.lat.setDouble(loc.latitude());
                p.lng.setDouble(loc.longitude());
                emit SysMsgUserLocationUpdate(msg->fromuser(), p);
            }
            break;
        case ProtocolBuffer::SubscriptionData:
            {
                ProtocolBuffer::CommentList *clist = new ProtocolBuffer::CommentList;
                clist->ParseFromString(msg->buffer());
                emit subscriptionArrived(clist);
                // there is no safe way to release clist, we need to be careful.
            }
            break;
        default:
            qDebug()<<"Unhandled message type: "<< msg->systemmessagetype();
        }
    }
}

void MapController::AddEditingRouteInFavorOf( const QList<GeoPoint>* points, int uid )
{
    if (map)
    {
        RouteItem* item = map->addRoute(*points, uid);
        connect(item, SIGNAL(EditFinished(RouteItem*)), this, SLOT(finishedRouteEditing(RouteItem*)));
    }
    delete points;
}

void MapController::finishedRouteEditing( RouteItem* item )
{
    // Prompt the user if he wants to send this back
    if ( item->getRouteReceiverWhenDoneEditing() )
    {
        QMessageBox msgbox;
        QString text = tr("Do you want to send this route back to %1?").arg(QString::fromUtf8(getSession()->getUser(item->getRouteReceiverWhenDoneEditing())->nickname().c_str()));
        msgbox.setIcon(QMessageBox::Question);
        msgbox.setText(text);
        msgbox.setWindowTitle("Finished editing route");
        msgbox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        int ret = msgbox.exec();
        switch (ret)
        {
        case QMessageBox::Yes:
            // send this route back.
            getSession()->SendRoutingReply( item->getRoute(), item->getRouteReceiverWhenDoneEditing() );
            // assume it would success
            // remove this route
            map->removeItem(item);
            item->deleteLater();
            break;
        case QMessageBox::No:
            // do nothing
            break;
        }
    }
}

void MapController::setSession( Session* s )
{
    if (session)
        disconnect(this, SIGNAL(currentLocationUpdate(GeoPoint)), session, SLOT(UserLocationUpdate(const GeoPoint&)));
    if (s)
        connect(this, SIGNAL(currentLocationUpdate(GeoPoint)), s, SLOT(UserLocationUpdate(const GeoPoint&)));
    session = s;
}
