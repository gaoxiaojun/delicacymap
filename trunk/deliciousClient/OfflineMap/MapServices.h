#pragma once

#include <QObject>
#include <QMap>
#include <QString>
#include <QList>
#include "GeoCoord.h"

class QNetworkAccessManager;
class QNetworkReply;

class MapServices : public QObject
{
    Q_OBJECT
public:
    MapServices(void);
    ~MapServices(void);

    void GeoCode(const QString& where, void* data);
    void ReverseGeoCode(double latitude, double longitude, void* data);
    void QueryRoute(const QString& from, const QString& to, void* data);

signals:
    void GeoCodeResult(const QString originalQuery, double lattitude, double longitude, void* data);
    void ReverseGeoCodeResult(const QString originalQuery, const QString address, void* data);
    void RoutingResult(QList<GeoPoint>, void* data);

protected slots:
    void ProcessJSONResult(QNetworkReply*);

private:
    QMap<QNetworkReply*, void*> geoRequests;
    QMap<QNetworkReply*, void*> reverseGeoRequests;
    QMap<QNetworkReply*, void*> directionRequests;
    QNetworkAccessManager *network;
};
