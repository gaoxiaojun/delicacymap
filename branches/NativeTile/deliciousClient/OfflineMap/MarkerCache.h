#pragma once

#include "GeoCoord.h"

#include <utility>
#include <QString>
#include <vector>

struct MarkerInfo
{
    QString info;
    GeoPoint location;
    bool operator<(const MarkerInfo& other) const { return location.lat < other.location.lat ? true : location.lng < other.location.lng ? true : false; }
};

class MarkerCache
{
// public:
//     MarkerCache();
//     ~MarkerCache();
private:
    typedef std::vector<MarkerInfo> ContainerType;

public:
    typedef std::pair<ContainerType::const_iterator, ContainerType::const_iterator> RangeType;

    RangeType MarkersInBound(const GeoBound&);
    ContainerType::iterator AddMarker(const MarkerInfo&);
    ContainerType::iterator AddMarker(const QString &info, const GeoPoint&);

private:
    ContainerType container;
};
