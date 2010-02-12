#include "MarkerCache.h"
#include <algorithm>

using namespace std;

template <typename KeyType>
struct GeoPointGreaterComparer
{
    GeoPointGreaterComparer(const GeoPoint &p) : point(p) {}
    bool operator()(const typename KeyType::result_type& info) const
    {
        return point.lat < info.value.location.lat && point.lng < info.value.location.lng;
    }
private:
    GeoPoint point;
};

template <typename KeyType>
struct GeoPointLesserComparer
{
    GeoPointLesserComparer(const GeoPoint &p) : point(p) {}
    bool operator()(const typename KeyType::result_type& info) const
    {
        return info.value.location.lat < point.lat && info.value.location.lng < point.lng;
    }
private:
    GeoPoint point;
};

MarkerCache::RangeType MarkerCache::MarkersInBound( const GeoBound& bound )
{
    typedef ContainerType::index<__Lat_Lng_Ordered_Tag>::type OrderedView;
    OrderedView &view = container.get<__Lat_Lng_Ordered_Tag>();
    return view.range(GeoPointGreaterComparer<OrderedView::key_from_value>(bound.SW), GeoPointLesserComparer<OrderedView::key_from_value>(bound.NE));
}

MarkerCache::ContainerType::const_iterator MarkerCache::AddMarker( const MarkerInfo& value )
{
    container.insert(value);
    return ContainerType::const_iterator();
}

MarkerCache::ContainerType::const_iterator MarkerCache::AddMarker( const QString& info, const GeoPoint& location )
{
    MarkerInfo temp;
    temp.info = info;
    temp.location = location;
    return AddMarker(temp);
}

GeoCoord __Lat_Extractor( const MarkerInfo& m )
{
    return m.location.lat;
}

GeoCoord __Lng_Extractor( const MarkerInfo& m )
{
    return m.location.lng;
}
