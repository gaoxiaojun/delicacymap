#include "MapController.h"
#include "MapProtocol.pb.h"
#include "Session.h"
#include "OfflineMap/MapViewBase.h"
#include "OfflineMap/MarkerItem.h"
#include <QGeoPositionInfoSource>
#include <QMessageBox>
#include <QDebug>
#include <QList>

MapController::MapController(void)
: map(NULL), session(NULL)
{
    
}

MapController::~MapController(void)
{
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

void MapController::setLocationSource( QGeoPositionInfoSource* loc )
{
    if (loc)
    {
        loc_svc = loc;
        loc_svc->setUpdateInterval(3000);
        connect(loc_svc, SIGNAL(positionUpdated(QGeoPositionInfo)), this, SLOT(translateLocationSignal(QGeoPositionInfo)));
        loc_svc->startUpdates();
    }
}

void MapController::translateLocationSignal( QGeoPositionInfo info )
{
    GeoPoint location(info.coordinate().latitude(), info.coordinate().longitude());
    emit currentLocationUpdate(location);
}

void MapController::HandleSystemMessages( const ProtocolBuffer::DMessage* msg )
{
    if (msg->issystemmessage())
    {
        switch (msg->systemmessagetype())
        {
        case ProtocolBuffer::RequestRouting:
            {
                int pos = msg->buffer().find('|');
                QString from = QString::fromUtf8(msg->buffer().c_str(), pos);
                QString to = QString::fromUtf8(msg->buffer().c_str() + pos + 1);
                emit SysMsgRequestRouting(msg->fromuser(), from, to);
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
        default:
            qDebug()<<"Unhandled message type: "<< msg->systemmessagetype();
        }
    }
}

void MapController::AddEditingRouteInFavorOf( const QList<GeoPoint>& points, void* data )
{
    if (data && map)
    {
        // for now, data could only be uid
        int uid = reinterpret_cast<intptr_t>( data ); // This is dangerous.
        RouteItem* item = map->addRoute(points, uid);
        connect(item, SIGNAL(EditFinished(RouteItem*)), this, SLOT(finishedRouteEditing(RouteItem*)));
    }
}

void MapController::finishedRouteEditing( RouteItem* item )
{
    // Prompt the user if he wants to send this back
    if ( item->getRouteReceiverWhenDoneEditing() )
    {
        QMessageBox msgbox;
        QString text = QString("Do you want to send this route back to %1?.").arg(QString::fromUtf8(getSession()->getUser(item->getRouteReceiverWhenDoneEditing())->nickname().c_str()));
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
