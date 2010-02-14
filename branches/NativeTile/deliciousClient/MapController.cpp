#include "MapController.h"
#include "MapProtocol.pb.h"
#include "Session.h"
#include "OfflineMap/MapViewBase.h"
#include "OfflineMap/GeoCoord.h"

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
        //v->addNewRestaurantMarker(&r);
    }
    delete rlist;
}