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
bool ReadName(json_spirit::wObject& object, QString& name)
{
    bool nameset = false;
    BOOST_FOREACH(json_spirit::wPair& node, object)
        if (node.name_ == L"name")
        {
            name = QString::fromStdWString(node.value_.get_str());
            nameset = true;
        }
    return nameset;
}

inline static
bool ReadStatus(json_spirit::wObject& object, int& resultcode, QString& requesttype)
{
    bool codeset = false, typeset = false;
    BOOST_FOREACH(json_spirit::wPair& node, object)
        if (node.name_ == L"Status")
        {
            if (node.value_.type() == json_spirit::obj_type)
            {
                json_spirit::wObject& status = node.value_.get_obj();
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
            }
            break;
        }
    return codeset && typeset;
}

inline static
bool ReadPlacementForCoordinate(json_spirit::wObject& object, double& lat, double& lng )
{
    bool coordinateset = false;
    BOOST_FOREACH(json_spirit::wPair& node, object)
        if (node.name_ == L"Placemark" && node.value_.type() == json_spirit::array_type && node.value_.get_array().size() > 0)
        {
            json_spirit::wObject& placemark = node.value_.get_array()[0].get_obj();
            BOOST_FOREACH(json_spirit::wPair& item, placemark)
                if (item.name_ == L"Point" && item.value_.type() == json_spirit::obj_type)
                {
                    json_spirit::wObject& point = item.value_.get_obj();
                    if (point.size() > 0 && point[0].name_ == L"coordinates" && point[0].value_.type() == json_spirit::array_type)
                    {
                        json_spirit::wArray& coordinates = point[0].value_.get_array();
                        if (coordinates.size() >= 2 && coordinates[0].type() == json_spirit::real_type && coordinates[1].type() == json_spirit::real_type)
                        {
                            lng = coordinates[0].get_real();
                            lat = coordinates[1].get_real();
                            coordinateset = true;
                            break;
                        }
                    }
                }
            break;
        }
    return coordinateset;
}

inline static
bool ReadPlacementForAddress(json_spirit::wObject& object, QString& address)
{
    bool addressset = false;
    BOOST_FOREACH(json_spirit::wPair& node, object)
        if (node.name_ == L"Placemark" && node.value_.type() == json_spirit::array_type && node.value_.get_array().size() > 0)
        {
            json_spirit::wObject& placemark = node.value_.get_array()[0].get_obj();
            BOOST_FOREACH(json_spirit::wPair& item, placemark)
                if (item.name_ == L"address")
                {
                    address = QString::fromStdWString(item.value_.get_str());
                    addressset = true;
                    break;
                }
            break;
        }
    return addressset;
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
                if (ReadStatus(rootobj, returncode, requesttype) && returncode==200 && ReadName(rootobj, originalquery))
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
