#include "MapServices.h"
#include "JSON/json_spirit_reader_template.h"
#include "JSON/json_spirit_writer_template.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QString>
#include <QDebug>

#include <boost/foreach.hpp>

//////////////////////////////////////////////////////////////////////////
//    for information about Geocode and reverse Geocode results, see
//    http://code.google.com/apis/maps/documentation/geocoding/index.html
//    for information about GDirection
//    try a query yourself and see what's in there!
//////////////////////////////////////////////////////////////////////////

using namespace std;

static const QString _APIKey("ABQIAAAAzZW9Kh-ejqMBSzkwxwi91RT3c6Xka6saTVQuYU1_pfXYA5CmJxS9Am9Z7YItvCYjbWHIjJKikswJ7Q");

static const QString _SearchURL("http://maps.google.com/maps?output=json&q=%1");
static const QString _GeoCodeURL("http://maps.google.com/maps/geo?q=%2&output=json&sensor=false&key=%1&gl=cn");
static const QString _RevGeoCodeURL("http://maps.google.com/maps/geo?q=%2,%3&output=json&sensor=false&key=%1&gl=cn");
static const QString _GDirectionURL("http://maps.google.com/maps/nav?key=%1&output=js&doflg=ptj&q=from:%2 to:%3");

MapServices::MapServices(void)
{
    network = new QNetworkAccessManager(this);
    connect(network, SIGNAL(finished(QNetworkReply*)), this, SLOT(ProcessJSONResult(QNetworkReply*)));
}

MapServices::~MapServices(void)
{
}

inline static
json_spirit::wValue& ReadSubItem(json_spirit::wObject& object, const std::wstring& objectname)
{
    static json_spirit::wValue invalidValue;
    BOOST_FOREACH(json_spirit::wPair& node, object)
        if (node.name_ == objectname)
        {
            return node.value_;
        }
    return invalidValue;
}

inline static
json_spirit::wObject& ReadSubObject(json_spirit::wObject& object, const std::wstring& objectname)
{
    static json_spirit::wObject invalidObject;
    json_spirit::wValue& value = ReadSubItem(object, objectname);
    return !value.is_null() && value.type() == json_spirit::obj_type ?
        value.get_obj() :
        invalidObject;
}

inline static
json_spirit::wArray& ReadSubArray(json_spirit::wObject& object, const std::wstring& objectname)
{
    static json_spirit::wArray invalidArray;
    json_spirit::wValue& value = ReadSubItem(object, objectname);
    return !value.is_null() && value.type() == json_spirit::array_type ?
        value.get_array() :
        invalidArray;
}

inline static
bool ReadPropertyValue(json_spirit::wObject& object, const std::wstring& propertyname, QString& value)
{
    bool propertyset = false;
    json_spirit::wValue& jsonvalue = ReadSubItem(object, propertyname);
    if (!jsonvalue.is_null())
    {
        value = QString::fromStdWString(jsonvalue.get_str());
        propertyset = true;
    }
    return propertyset;
}

inline static
bool ReadStatus(json_spirit::wObject& object, int& resultcode, QString& requesttype)
{
    bool codeset = false, typeset = false;
    json_spirit::wObject& status = ReadSubObject(object, L"Status");
    BOOST_FOREACH(json_spirit::wPair& statusnode, status)
    {
        if (statusnode.name_ == L"code")
        {
            resultcode = statusnode.value_.get_int();
            codeset = true;
        }
        else if (statusnode.name_ == L"request")
        {
            requesttype = QString::fromStdWString(statusnode.value_.get_str());
            typeset = true;
        }
    }
    return codeset && typeset;
}

inline static
bool ReadPoint(json_spirit::wObject& point, double& lat, double& lng )
{
    bool readSuccess;
    if (point.size() > 0 && point[0].name_ == L"coordinates" && point[0].value_.type() == json_spirit::array_type)
    {
        json_spirit::wArray& coordinates = point[0].value_.get_array();
        if (coordinates.size() >= 2 && coordinates[0].type() == json_spirit::real_type && coordinates[1].type() == json_spirit::real_type)
        {
            lng = coordinates[0].get_real();
            lat = coordinates[1].get_real();
            readSuccess =  true;
        }
    }
    return readSuccess;
}

inline static
bool ReadPlacementForCoordinate(json_spirit::wObject& object, double& lat, double& lng )
{
    json_spirit::wArray& placemarkarray = ReadSubArray(object, L"Placemark");
    if (placemarkarray.size() > 0)
    {
        json_spirit::wObject& placemark = placemarkarray[0].get_obj();
        json_spirit::wObject& point = ReadSubObject(placemark, L"Point");
        return ReadPoint(point, lat, lng);
    }
    return false;
}

inline static
bool ReadPlacementForAddress(json_spirit::wObject& object, QString& address)
{
    json_spirit::wArray& placemarkArray = ReadSubArray(object, L"Placemark");
    if (placemarkArray.size() > 0)
    {
        json_spirit::wObject& placemark = placemarkArray[0].get_obj();
        return ReadPropertyValue(placemark, L"address", address);
    }
    return false;
}

inline static
QList<GeoPoint> ReadRoute(json_spirit::wObject object)
{
    QList<GeoPoint> result;
    json_spirit::wObject& direction = ReadSubObject(object, L"Directions");
    json_spirit::wArray& route = ReadSubArray(direction, L"Routes");
    if (route.size() > 0)
    {
        json_spirit::wArray& steps = ReadSubArray(route[0].get_obj(), L"Steps");
        BOOST_FOREACH(json_spirit::wValue& jsonstep, steps)
            if (jsonstep.type() == json_spirit::obj_type)
            {
                json_spirit::wObject& step = jsonstep.get_obj();
                double lat, lng;
                json_spirit::wObject& point = ReadSubObject(step, L"Point");
                if (ReadPoint(point, lat, lng))
                {
                    result.push_back(GeoPoint(lat, lng));
                }
            }
    }
    return result;
}

void MapServices::ProcessJSONResult( QNetworkReply* reply )
{
    if (reply->isFinished())
    {
        QVariant &header = reply->header(QNetworkRequest::ContentTypeHeader);
        if (header.isValid() && header.toString().contains("text", Qt::CaseInsensitive))
        {
            QByteArray buf = reply->read(1024 * 1024); // shouldn't be more than 1 MB, if it does, probably something went wrong
            QString jsonret = QString::fromUtf8(buf.constData(), buf.size());
            json_spirit::wValue root;
            if (json_spirit::read_string(jsonret.toStdWString(), root) && root.type() == json_spirit::obj_type)
            {
// #ifndef NDEBUG
//                 wstring output = json_spirit::write_string(root, true);
//                 qDebug()<<QString::fromStdWString(output);
// #endif
                json_spirit::wObject &rootobj = root.get_obj();
                QString originalquery, requesttype;
                int returncode = 0;
                if (ReadStatus(rootobj, returncode, requesttype) && returncode==200 && ReadPropertyValue(rootobj, L"name", originalquery))
                {
                    if (requesttype == "geocode")
                    {
                        if (reverseGeoRequests.contains(reply))
                        {
                            QString address;
                            reverseGeoRequests.remove(reply);
                            if (ReadPlacementForAddress(rootobj, address))
                                emit ReverseGeoCodeResult(originalquery, address);
                            else
                                emit ReverseGeoCodeResult(originalquery, QString());
                        }
                        else
                        {
                            double lattitude, longitude;
                            if (ReadPlacementForCoordinate(rootobj, lattitude, longitude))
                                emit GeoCodeResult(originalquery, lattitude, longitude);
                            else
                                emit GeoCodeResult(originalquery, 0.0, 0.0);
                        }
                    }
                    else if (requesttype == "directions")
                    {
                        emit RoutingResult( ReadRoute(rootobj) );
                    }
                }
            }
            
        }
    }
    reply->deleteLater();
}

void MapServices::GeoCode( const QString& where )
{
    QUrl url(_GeoCodeURL.arg(_APIKey, where));
    QNetworkRequest request(url);
    network->get(request);
}

void MapServices::ReverseGeoCode( double latitude, double longitude )
{
    QUrl url(_RevGeoCodeURL.arg(_APIKey).arg(latitude, 0, 'g', 9).arg(longitude, 0, 'g', 9));
    QNetworkRequest request(url);
    reverseGeoRequests.insert(network->get(request));
}

void MapServices::QueryRoute( const QString& from, const QString& to )
{
    QString s = _GDirectionURL.arg(_APIKey, from, to);
    QUrl url(s);
    QNetworkRequest request(url);
    network->get(request);
}
