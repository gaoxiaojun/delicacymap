#include "MarkerCache.h"
#include <algorithm>

using namespace std;

MarkerCache::RangeType MarkerCache::MarkersInBound( const GeoBound& bound )
{
    MarkerInfo testagainst;
    testagainst.location = bound.SW;
    ContainerType::const_iterator lower = lower_bound(container.begin(), container.end(), testagainst);
    testagainst.location = bound.NE;
    ContainerType::const_iterator upper = upper_bound(container.begin(), container.end(), testagainst);
    return RangeType(lower, upper);
}

MarkerCache::ContainerType::iterator MarkerCache::AddMarker( const MarkerInfo& value )
{
    ContainerType::const_iterator upper = upper_bound(container.begin(), container.end(), value);
    return container.insert(upper, value);
}

MarkerCache::ContainerType::iterator MarkerCache::AddMarker( const QString& info, const GeoPoint& location )
{
    MarkerInfo temp;
    temp.info = info;
    temp.location = location;
    return AddMarker(temp);
}
