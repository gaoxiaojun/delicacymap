#pragma once

#include "OfflineMap/GeoCoord.h"
#include <QGeoPositionInfo>
#include <QObject>
#include <QString>
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
class QGeoPositionInfoSource;
QTM_END_NAMESPACE

class MapController : public QObject
{
    Q_OBJECT
public:
    MapController(void);
    ~MapController(void);

    void setMapView(MapViewBase* m);;
    MapViewBase* getMapView() { return map; }
    void setSession(Session* s) { session = s; }
    Session* getSession() { return session; }
    void setLocationSource(QGeoPositionInfoSource*);

signals:
    void newRestaurantMarker(const ProtocolBuffer::Restaurant*);
    void currentLocationUpdate(GeoPoint);
    void SysMsgRequestRouting(int, QString, QString);
    void SysMsgRoutingReply(int, QList<GeoPoint>);

public slots:
    void MapViewBoundsChange(const GeoBound&);
    void HandleSystemMessages(const ProtocolBuffer::DMessage*);
    void AddEditingRouteInFavorOf(const QList<GeoPoint>*, int);

private:
    void RestaurantListHandler(ProtocolBuffer::RestaurantList*, MapViewBase*);

private slots:
    void translateLocationSignal(QGeoPositionInfo);
    void finishedRouteEditing(RouteItem*);

private:
    QSet<int> _restaurants;
    MapViewBase *map;
    Session *session;
    QGeoPositionInfoSource* loc_svc;
};
