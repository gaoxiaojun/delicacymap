#include "MapServices.h"
#include "JSON/json_spirit_reader_template.h"
#include "JSON/json_spirit_writer_template.h"
#include "google/protobuf/stubs/common.h"

#include <QSystemNetworkInfo>
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
//    for information about cell tower location, see following
//    http://www.anddev.org/poor_mans_gps_-_celltowerid_-_location_area_code_-lookup-t257.html
//////////////////////////////////////////////////////////////////////////

using namespace std;
QTM_USE_NAMESPACE

static const QString _APIKey("ABQIAAAAzZW9Kh-ejqMBSzkwxwi91RT3c6Xka6saTVQuYU1_pfXYA5CmJxS9Am9Z7YItvCYjbWHIjJKikswJ7Q");

static const QString _SearchURL("http://maps.google.com/maps?output=json&q=%1");
static const QString _GeoCodeURL("http://maps.google.com/maps/geo?q=%2&output=json&sensor=false&key=%1&gl=cn");
static const QString _RevGeoCodeURL("http://maps.google.com/maps/geo?q=%2,%3&output=json&sensor=false&key=%1&gl=cn");
static const QString _GDirectionURL("http://maps.google.com/maps/nav?key=%1&output=js&doflg=ptj&q=from:%2 to:%3");
static const QString _CellLocationURL("http://www.google.com/loc/json");

static const QString _CellRequest("{"
    "\"version\": \"1.1.0\","
    "\"host\": \"maps.google.com\","
    "\"cell_towers\": ["
      "{"
        "\"cell_id\": %1,"
        "\"location_area_code\": %2,"
        "\"mobile_country_code\": %3,"
        "\"mobile_network_code\": %4"
      "}]}");

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
bool ReadLocation(json_spirit::wObject& location, InaccurateGeoPoint& geopoint)
{
    bool readSuccess = false;
    json_spirit::wValue& lat = ReadSubItem(location, L"latitude");
    json_spirit::wValue& lng = ReadSubItem(location, L"longitude");
    json_spirit::wValue& acc = ReadSubItem(location, L"accuracy");
    if (!lat.is_null() && !lng.is_null() && !acc.is_null()
        && lat.type()==json_spirit::real_type
        && lng.type()==json_spirit::real_type
        && acc.type()==json_spirit::real_type)
    {
        readSuccess = true;
        geopoint.p.lat.setDouble(lat.get_real());
        geopoint.p.lng.setDouble(lng.get_real());
        geopoint.accuracy = acc.get_real();
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
bool ReadRoute(json_spirit::wObject object, QList<GeoPoint>& result)
{
    bool success = false;
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
        success = true;
    }
    return success;
}

void MapServices::ProcessJSONResult( QNetworkReply* reply )
{
    google::protobuf::Closure* done = resultCallbacks.take(reply);
    ServiceResponse result = results.take(reply);
    if (reply->isFinished())
    {
        const QVariant &header = reply->header(QNetworkRequest::ContentTypeHeader);
        if (header.isValid() && (header.toString().contains("text/javascript", Qt::CaseInsensitive) || header.toString().contains("application/json", Qt::CaseInsensitive)))
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
                json_spirit::wObject &locationObj = ReadSubObject(rootobj, L"location");
                QString originalquery, requesttype;
                int returncode = 0;
                if (locationObj.size() > 0)
                {
                    ReadLocation(locationObj, *result.acoord);
                }
                else if (ReadStatus(rootobj, returncode, requesttype) && returncode==200 && ReadPropertyValue(rootobj, L"name", originalquery))
                {
                    if (requesttype == "geocode")
                    {
                        if (reverseGeoRequests.contains(reply))
                        {
                            reverseGeoRequests.remove(reply);
                            if (ReadPlacementForAddress(rootobj, *result.address))
                                done->Run();
                        }
                        else
                        {
                            double lattitude, longitude;
                            if (ReadPlacementForCoordinate(rootobj, lattitude, longitude))
                            {
                                result.coord->lat.setDouble(lattitude);
                                result.coord->lng.setDouble(longitude);
                                done->Run();
                            }
                        }
                    }
                    else if (requesttype == "directions")
                    {
                        if (ReadRoute(rootobj, *result.route))
                            done->Run();
                    }
                }
            }
            
        }
    }
    reply->deleteLater();
}

void MapServices::GeoCode( const QString& where, GeoPoint& point, google::protobuf::Closure* callback )
{
    ServiceResponse ret;
    ret.coord = &point;
    QUrl url(_GeoCodeURL.arg(_APIKey, where));
    QNetworkRequest request(url);
    QNetworkReply* reply = network->get(request);
    resultCallbacks.insert(reply, callback);
    results.insert(reply, ret);
}

void MapServices::ReverseGeoCode( double latitude, double longitude, QString& address, google::protobuf::Closure* callback )
{
    ServiceResponse ret;
    ret.address = &address;
    QUrl url(_RevGeoCodeURL.arg(_APIKey).arg(latitude, 0, 'g', 9).arg(longitude, 0, 'g', 9));
    QNetworkRequest request(url);
    QNetworkReply* reply = network->get(request);
    resultCallbacks.insert(reply, callback);
    results.insert(reply, ret);
    reverseGeoRequests.insert(network->get(request));
}

void MapServices::QueryRoute( const QString& from, const QString& to, QList<GeoPoint>& route, google::protobuf::Closure* callback )
{
    ServiceResponse ret;
    ret.route = &route;
    QUrl url(_GDirectionURL.arg(_APIKey, from, to));
    QNetworkRequest request(url);
    QNetworkReply* reply = network->get(request);
    resultCallbacks.insert(reply, callback);
    results.insert(reply, ret);
}

bool MapServices::LocationByCellID(InaccurateGeoPoint& coord, google::protobuf::Closure* callback)
{
    QSystemNetworkInfo ninfo;
    if (ninfo.currentMobileCountryCode().size() > 0 && ninfo.currentMobileNetworkCode().size() > 0)
    {
        ServiceResponse ret;
        ret.acoord = &coord;
        QString requestbody = _CellRequest.arg(ninfo.cellId()).arg(ninfo.locationAreaCode()).arg(ninfo.currentMobileCountryCode(), ninfo.currentMobileNetworkCode());
        //QString requestbody = _CellRequest.arg(20442).arg(6015);
        QNetworkReply *reply = network->post(QNetworkRequest(QUrl(_CellLocationURL)), requestbody.toUtf8());
        resultCallbacks.insert(reply, callback);
        results.insert(reply, ret);
        return true;
    }
    return false;
}
