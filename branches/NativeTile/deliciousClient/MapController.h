#pragma once

#include <QObject>
#include <QSet>

namespace ProtocolBuffer{
    class Restaurant;
    class RestaurantList;
    class CommentList;
    class User;
    class DMessage;
}

class GeoBound;
class MapViewBase;
class Session;

class MapController : public QObject
{
    Q_OBJECT
public:
    MapController(void);
    ~MapController(void);

    void setMapView(MapViewBase* m) { map = m; };
    MapViewBase* getMapView() { return map; }
    void setSession(Session* s) { session = s; }
    Session* getSession() { return session; }

public slots:
    void MapViewBoundsChange(const GeoBound&);

private:
    void RestaurantListHandler(ProtocolBuffer::RestaurantList*, MapViewBase*);

private:
    QSet<int> _restaurants;
    MapViewBase *map;
    Session *session;
};
