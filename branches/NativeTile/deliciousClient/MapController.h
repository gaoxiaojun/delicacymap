#pragma once

#include <QObject>

namespace ProtocolBuffer{
    class Restaurant;
    class RestaurantList;
    class CommentList;
    class User;
    class DMessage;
}

class GeoBound;
class MarkerCache;
class Session;

class MapController : public QObject
{
    Q_OBJECT
public:
    MapController(void);
    ~MapController(void);

    void setMarkerCache(MarkerCache* cache) { markers = cache; };
    MarkerCache* getMarkerCache() { return markers; }
    void setSession(Session* s) { session = s; }
    Session* getSession() { return session; }

public slots:
    void MapViewBoundsChange(const GeoBound&);

private:
    void RestaurantListHandler(ProtocolBuffer::RestaurantList*, MarkerCache*);

private:
    MarkerCache *markers;
    Session *session;
};
