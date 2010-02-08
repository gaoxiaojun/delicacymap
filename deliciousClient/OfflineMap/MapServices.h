#pragma once

#include <QObject>
#include <QSet>
#include <QString>

class QNetworkAccessManager;
class QNetworkReply;

class MapServices : public QObject
{
    Q_OBJECT
public:
    MapServices(void);
    ~MapServices(void);

    void GeoCode(const QString& where);
    void ReverseGeoCode(double latitude, double longitude);
    void QueryRoute(const QString& from, const QString& to);

signals:
    void GeoCodeResult(const QString originalQuery, double lattitude, double longitude);
    void ReverseGeoCodeResult(const QString originalQuery, const QString address);
    void NewRoutingResult();

protected slots:
    void ProcessJSONResult(QNetworkReply*);

private:
    QSet<QNetworkReply*> reverseGeoRequests;
    QNetworkAccessManager *network;
};
