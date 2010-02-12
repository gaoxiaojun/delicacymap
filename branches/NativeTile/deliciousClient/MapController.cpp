#include "MapController.h"
#include "MapProtocol.pb.h"
#include "Session.h"
#include "OfflineMap/MarkerCache.h"

MapController::MapController(void)
: markers(NULL), session(NULL)
{
    
}

MapController::~MapController(void)
{
}

void MapController::MapViewBoundsChange( const GeoBound& bound )
{
    if (getSession())
    {
        ProtocolBuffer::RestaurantList *rlist = new ProtocolBuffer::RestaurantList;
        google::protobuf::Closure *rClosure = google::protobuf::NewPermanentCallback(this, &MapController::RestaurantListHandler, rlist, getMarkerCache());
        getSession()->getDataSource().GetRestaurants(
            bound.SW.lat.getDouble(), bound.SW.lng.getDouble(),
            bound.NE.lat.getDouble(), bound.NE.lng.getDouble(),
            17,
            rlist,
            rClosure);
    }
}

void MapController::RestaurantListHandler( ProtocolBuffer::RestaurantList* rlist, MarkerCache* cache )
{
    for (int i=0;i<rlist->restaurants_size();i++)
    {
        const ProtocolBuffer::Restaurant& r = rlist->restaurants(i);
        MarkerInfo info;
        info.info = QString::fromUtf8( r.name().c_str(), r.name().length() );
        info.location.lat.setDouble(r.location().latitude());
        info.location.lng.setDouble(r.location().longitude());

        cache->AddMarker(info);
    }
    delete rlist;
}