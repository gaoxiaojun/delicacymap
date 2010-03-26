#pragma once

#include "OfflineMap/GeoCoord.h"
#include <QGeoPositionInfo>
#include <QObject>
#include <QSet>

QTM_USE_NAMESPACE

namespace ProtocolBuffer{
    class Restaurant;
    class RestaurantList;
    class CommentList;
    class User;
    class DMessage;
}

class MapViewBase;
class Session;
class RouteItem;

QTM_BEGIN_NAMESPACE
class QGeoPositionInfo;
class QGeoPositionInfoSource;
class QGeoSatelliteInfoSource;
QTM_END_NAMESPACE

class MapController : public QObject
{
    Q_OBJECT
public:
    MapController(void);
    ~MapController(void);

    void setMapView(MapViewBase* m);;
    MapViewBase* getMapView() { return map; }
    QGeoSatelliteInfoSource* getSatelliteInfoSource();
    QGeoPositionInfoSource* getPositionInfoSource() { return loc_svc; }
    void setSession(Session* s);
    Session* getSession() { return session; }
    void setLocationSource(QGeoPositionInfoSource*);

signals:
    void newRestaurantMarker(const ProtocolBuffer::Restaurant*);
    void currentLocationUpdate(GeoPoint);
    void SysMsgRequestRouting(int, QString, QString);
    void SysMsgRoutingReply(int, QList<GeoPoint>);
    void SysMsgUserLocationUpdate(int, GeoPoint);

public slots:
    void MapViewBoundsChange(const GeoBound&);
    void HandleSystemMessages(const ProtocolBuffer::DMessage*);
    void AddEditingRouteInFavorOf(const QList<GeoPoint>*, int);

private:
    void RestaurantListHandler(ProtocolBuffer::RestaurantList*, MapViewBase*);

private slots:
    void translateLocationSignal(const QGeoPositionInfo&);
    void finishedRouteEditing(RouteItem*);

private:
    QSet<int> _restaurants;
    MapViewBase *map;
    Session *session;
    QGeoSatelliteInfoSource* sat_svc;
    QGeoPositionInfoSource* loc_svc;
};
