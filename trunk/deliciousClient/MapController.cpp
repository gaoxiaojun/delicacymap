#include "MapController.h"
#include "MapProtocol.pb.h"
#include "Session.h"
#include "OfflineMap/MapViewBase.h"
#include <QGeoPositionInfoSource>


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
            emit SysMsgRequestRouting(msg->fromuser());
            break;
        }
    }
}
