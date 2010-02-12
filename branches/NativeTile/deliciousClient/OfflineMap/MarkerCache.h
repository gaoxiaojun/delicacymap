#pragma once

#include "GeoCoord.h"

#include <utility>
#include <QString>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/global_fun.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/composite_key.hpp>

struct MarkerInfo
{
    QString info;
    GeoPoint location;
};

GeoCoord __Lat_Extractor(const MarkerInfo&);
GeoCoord __Lng_Extractor(const MarkerInfo&);

struct __Lat_Lng_Ordered_Tag{};

class MarkerCache
{
// public:
//     MarkerCache();
//     ~MarkerCache();
private:
    typedef boost::multi_index::multi_index_container<
        MarkerInfo,
        boost::multi_index::indexed_by<
            boost::multi_index::ordered_non_unique<
                boost::multi_index::tag<__Lat_Lng_Ordered_Tag>,
                boost::multi_index::composite_key<
                    MarkerInfo,
                    boost::multi_index::global_fun<const MarkerInfo&, GeoCoord, __Lat_Extractor>,
                    boost::multi_index::global_fun<const MarkerInfo&, GeoCoord, __Lng_Extractor>
                >
            >
        >
    > ContainerType;

public:
    typedef std::pair<ContainerType::const_iterator, ContainerType::const_iterator> RangeType;

    RangeType MarkersInBound(const GeoBound&);
    ContainerType::const_iterator AddMarker(const MarkerInfo&);
    ContainerType::const_iterator AddMarker(const QString &info, const GeoPoint&);

private:
    ContainerType container;
};
