#include "GPSLocationSvc_wince.h"

#include <Winbase.h>//sign,boost::thread doesn't work with evc9
#include <QDebug>
#include <boost/foreach.hpp>

class GPSLocation
    : public Location
{
public:
	GPSLocation(){};
    explicit GPSLocation(const GPS_POSITION& _pos) : pos(_pos){};
    double Latitude() const{return pos.dblLatitude;}
    double Longtitude() const{return pos.dblLongitude;}
    DateTime Time() const
    {
        DateTime t;
        t.year = pos.stUTCTime.wYear;
        t.month = pos.stUTCTime.wMonth;
        t.day = pos.stUTCTime.wDay;
        t.hour = pos.stUTCTime.wHour;
        t.minute = pos.stUTCTime.wMinute;
        t.millisecond = pos.stUTCTime.wMilliseconds;
        return t;
    }
    GPS_POSITION pos;
};

DWORD waitfornewposition(void* env)
{
    GPSLocationSvc_wince* svc = reinterpret_cast<GPSLocationSvc_wince*>(env);
    while (!svc->termflag)
    {
        ::WaitForSingleObject(svc->newpositionavail, INFINITE);
        BOOST_FOREACH(GPSLocationSvc_wince::CallBackPair &p, svc->callbackfuncs)
        {
            p.first(p.second, svc);
        }
    }
    return 0;
}

GPSLocationSvc_wince::GPSLocationSvc_wince(void)
{
    newpositionavail = CreateEvent(NULL, FALSE, FALSE, NULL);
    //devicestatechange = CreateEvent(NULL, FALSE, FALSE, NULL);
    gpsdevice = GPSOpenDevice(newpositionavail, 0, NULL, 0);
    newposthread = NULL;
    termflag = false;
}

GPSLocationSvc_wince::~GPSLocationSvc_wince(void)
{
    CloseHandle(newposthread);
    CloseHandle(gpsdevice);
    //CloseHandle(devicestatechange);
    CloseHandle(newpositionavail);
}

bool GPSLocationSvc_wince::canGetLocalLocation()
{
    return gpsdevice != NULL;
}

bool GPSLocationSvc_wince::getLocalLocation(Location* loc_in)
{
	GPS_POSITION pos = {0};
    pos.dwVersion = GPS_VERSION_1;
    pos.dwSize = sizeof(GPS_POSITION);
    DWORD errcode = GPSGetPosition(gpsdevice, &pos, INFINITE, 0);
    qDebug()<<"Error code: "<<errcode<<"\n"<<"valid fields"<<pos.dwValidFields;
    if (errcode == ERROR_SUCCESS
        && (pos.dwValidFields & GPS_VALID_LATITUDE)
        && (pos.dwValidFields & GPS_VALID_LONGITUDE))
    {
        GPSLocation* loc = (GPSLocation*)loc_in;
		loc->pos = pos;
        return true;
    }
    else
        return false;
}

bool GPSLocationSvc_wince::registerCallbackForNewPosition( CallBackFunc func, void* context )
{
    if (!newpositionavail)
        return false;

	callbackfuncs.push_back( CallBackPair(func, context) );

    termflag = false;
    if (!newposthread)
        newposthread = CreateThread(NULL, 0, waitfornewposition, this, 0, NULL);

    return true;
}

bool GPSLocationSvc_wince::removeCallbackFroNewPosition( CallBackFunc func, void* context )
{
    CallBackPair closure(func, context);

    for (std::vector<CallBackPair>::iterator it=callbackfuncs.begin();it!=callbackfuncs.end();++it)
        if (*it == closure)
        {
            callbackfuncs.erase(it);
            return true;
        }
    return false;
}

void GPSLocationSvc_wince::clearCallbacks()
{
    if (newposthread)
    {
        termflag = true;
        WaitForSingleObject(newposthread, INFINITE);
        newposthread = NULL;
        callbackfuncs.clear();
    }
}

Location* GPSLocationSvc_wince::newLocationInstance()
{
	return new GPSLocation();
}

void GPSLocationSvc_wince::deleteLocationInstance( Location*& loc )
{
	delete loc;
	loc = NULL;
}
