#pragma once

#include <QObject>
#include <QList>
#include <QMap>
#include <QSet>
#include <QString>
#include "GeoCoord.h"

class QNetworkAccessManager;
class QNetworkReply;

namespace google{
    namespace protobuf{
        class Closure;
    }
}

union ServiceResponse
{
    QString *address;
    GeoPoint *coord;
    QList<GeoPoint> *route;
};

class MapServices : public QObject
{
    Q_OBJECT
public:
    MapServices(void);
    ~MapServices(void);

    void GeoCode(const QString& where, GeoPoint& point, google::protobuf::Closure* callback);
    void ReverseGeoCode(double latitude, double longitude, QString& address, google::protobuf::Closure* callback);
    void QueryRoute(const QString& from, const QString& to, QList<GeoPoint>& route, google::protobuf::Closure* callback);
    // in the case where we can't locate by cellid, returns false, callback never runs.
    bool LocationByCellID(google::protobuf::Closure* callback);

protected slots:
    void ProcessJSONResult(QNetworkReply*);

private:
    QMap<QNetworkReply*, google::protobuf::Closure*> resultCallbacks;
    QMap<QNetworkReply*, ServiceResponse> results;
    QSet<QNetworkReply*> reverseGeoRequests;
    QNetworkAccessManager *network;
};
