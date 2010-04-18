#ifndef GEO_COORD_H
#define GEO_COORD_H

#include <QString>

class GeoCoord{
public:
    qint16 getDegrees() const;
    quint8 getMinutes() const;
    quint8 getSeconds() const;
    quint8 getFracSeconds() const;
    void setDegrees(const qint16 value);
    void setMinutes(const quint8 value);
    void setSeconds(const quint8 value);
    void setFracSeconds(const quint8 value);
    double getDouble() const;
    void setDouble(const double value);
    qint32 getValue() const;
    void setValue(const qint32 value);
    void setValue(const qint16 degrees, const quint8 minutes, const quint8 seconds, const quint8 fracSeconds);
    QString getString();
    void setString(const QString& str);
    GeoCoord();
    GeoCoord(const qint16 degrees, const quint8 minutes, const quint8 seconds, const quint8 fracSeconds);
    GeoCoord(const double degrees);
    bool isValid() const { return data != 0; }
    bool operator==(const GeoCoord& other) const { return data == other.data; }
    bool operator<(const GeoCoord& other) const { return data < other.data ; }
protected:
    enum {
        secondsMultiplier = 100, minutesMultiplier = secondsMultiplier*60,
        degreesMultiplier = minutesMultiplier*60, maxDegrees = 180, maxValue=degreesMultiplier*maxDegrees
    };
    qint32 data;
};

class GeoPoint
{
public:
    GeoPoint() {}
    GeoPoint(double Lat,double Lng) : lat(Lat), lng(Lng) {}
    GeoPoint(const GeoCoord& Lat, const GeoCoord& Lng) : lat(Lat), lng(Lng) {}

    bool isValid() const { return lat.isValid() && lng.isValid(); }

    bool operator==(const GeoPoint& other) const {return lat==other.lat && lng==other.lng;}
    bool operator!=(const GeoPoint& other) const {return !operator==(other);}

    GeoCoord lat, lng;
};

class InaccurateGeoPoint
{
public:
    GeoPoint p;
    int accuracy;
};

class GeoBound
{
public:
    GeoPoint SW,NE;

    bool operator==(const GeoBound& other) const {return SW==other.SW && NE==other.NE;}
};

#endif
